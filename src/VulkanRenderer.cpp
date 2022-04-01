// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanRenderer.cpp - Vulkan renderer class implementation
// author: Karl-Mihkel Ott

#define VULKAN_RENDERER_CPP
#include <VulkanRenderer.h>


namespace DENG {

    VulkanRenderer::VulkanRenderer(const Window &_win, const RendererConfig &_conf) : Renderer(_win, _conf) {
        mp_instance_creator = new Vulkan::InstanceCreator(m_window);
        mp_swapchain_creator = new Vulkan::SwapchainCreator(mp_instance_creator, m_window.GetSize(), m_sample_count);

        _CreateCommandPool();
        _CreateSemaphores();
        _AllocateBufferResources();
        _CreateColorResources();
        _CreateDepthResources();
        _CreateFrameBuffers();
        _AllocateCommandBuffers();

        // push missing texture 
        int x, y, size;
        const char *data = GetMissingTexture(x, y, size);
        PushTextureFromMemory(MISSING_TEXTURE_NAME, data, x, y, 4);
    }


    VulkanRenderer::~VulkanRenderer() {
        vkDeviceWaitIdle(mp_instance_creator->GetDevice());
        // depth image resources
        vkDestroyImageView(mp_instance_creator->GetDevice(), m_depth_image_view, NULL);
        vkDestroyImage(mp_instance_creator->GetDevice(), m_depth_image, NULL);
        vkFreeMemory(mp_instance_creator->GetDevice(), m_depth_image_memory, NULL);

        // color image resources
        vkDestroyImageView(mp_instance_creator->GetDevice(), m_color_image_view, NULL);
        vkDestroyImage(mp_instance_creator->GetDevice(), m_color_image, NULL);
        vkFreeMemory(mp_instance_creator->GetDevice(), m_color_image_memory, NULL);

        // destroy texture resources
        for(auto it = m_vulkan_texture_handles.begin(); it != m_vulkan_texture_handles.end(); it++) {
            vkDestroySampler(mp_instance_creator->GetDevice(), it->second.sampler, nullptr);
            vkDestroyImageView(mp_instance_creator->GetDevice(), it->second.image_view, nullptr);
            vkDestroyImage(mp_instance_creator->GetDevice(), it->second.image, nullptr);
            vkFreeMemory(mp_instance_creator->GetDevice(), it->second.memory, nullptr);
        }


        // Free all command buffers and framebuffers
        for(size_t i = 0; i < m_framebuffers.size(); i++)
            vkDestroyFramebuffer(mp_instance_creator->GetDevice(), m_framebuffers[i], NULL);

        vkFreeCommandBuffers(mp_instance_creator->GetDevice(), m_command_pool, static_cast<uint32_t>(m_command_buffers.size()), m_command_buffers.data());

        // free all allocated descriptor sets and pools
        // per shader descriptors
        while(m_shader_desc_allocators.size()) 
            m_shader_desc_allocators.erase(m_shader_desc_allocators.end() - 1);

        // per mesh descriptors
        while(m_mesh_desc_allocators.size()) 
            m_mesh_desc_allocators.erase(m_shader_desc_allocators.end() - 1);

        while(m_pipeline_creators.size())
            m_pipeline_creators.erase(m_pipeline_creators.end() - 1);

        while(m_descriptor_set_layout_creators.size())
            m_descriptor_set_layout_creators.erase(m_descriptor_set_layout_creators.end() - 1);

        for(size_t i = 0; i < mp_swapchain_creator->GetSwapchainImages().size(); i++) {
            vkDestroySemaphore(mp_instance_creator->GetDevice(), m_render_finished_semaphores[i], NULL);
            vkDestroySemaphore(mp_instance_creator->GetDevice(), m_image_available_semaphores[i], NULL);
            vkDestroyFence(mp_instance_creator->GetDevice(), m_flight_fences[i], NULL);
        }

        // free main buffers
        vkDestroyBuffer(mp_instance_creator->GetDevice(), m_main_buffer, NULL);
        vkFreeMemory(mp_instance_creator->GetDevice(), m_main_memory, NULL);

        // free uniform buffers
        vkDestroyBuffer(mp_instance_creator->GetDevice(), m_uniform_buffer, nullptr);
        vkFreeMemory(mp_instance_creator->GetDevice(), m_uniform_memory, nullptr);

        delete mp_swapchain_creator;
        delete mp_instance_creator;
    }


    void VulkanRenderer::PushTextureFromFile(const std::string &_name, const std::string &_file_name) {
        Libdas::TextureReader reader(_file_name, true);
        int x, y;
        size_t len;
        const char *raw = reader.GetRawBuffer(x, y, len);

        PushTextureFromMemory(_name, raw, static_cast<uint32_t>(x), static_cast<uint32_t>(y), 4);
    }
    

    void VulkanRenderer::PushTextureFromMemory(const std::string &_name, const char *_raw_data, uint32_t _width, uint32_t _height, uint32_t _bit_depth) {
        // check if texture with specified key already exists
        if(m_vulkan_texture_handles.find(_name) != m_vulkan_texture_handles.end()) {
            std::cerr << "Texture with name " << _name << " already exists" << std::endl;
            DENG_ASSERT(false);
            return;
        }
        
        // image data size
        const VkDeviceSize size = static_cast<VkDeviceSize>(_width * _height * _bit_depth);

        VkBuffer staging_buffer = VK_NULL_HANDLE;
        VkDeviceMemory staging_memory = VK_NULL_HANDLE;
        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(mp_instance_creator->GetDevice(), size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging_buffer);
        Vulkan::_AllocateMemory(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), mem_req.size, staging_memory, mem_req.memoryTypeBits,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkBindBufferMemory(mp_instance_creator->GetDevice(), staging_buffer, staging_memory, 0);

        Vulkan::_CopyToBufferMemory(mp_instance_creator->GetDevice(), size, _raw_data, staging_memory, 0);
        
        // create texture image instances
        Vulkan::TextureData texture_data;

        uint32_t mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(_width, _height)))) + 1;
        mem_req = Vulkan::_CreateImage(mp_instance_creator->GetDevice(), texture_data.image, _width, _height, mip_levels, VK_FORMAT_R8G8B8A8_SRGB, 
                                       VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                       VK_SAMPLE_COUNT_1_BIT);

        Vulkan::_AllocateMemory(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), mem_req.size, texture_data.memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vkBindImageMemory(mp_instance_creator->GetDevice(), texture_data.image, texture_data.memory, 0);
        Vulkan::_TransitionImageLayout(mp_instance_creator->GetDevice(), texture_data.image, m_command_pool, mp_instance_creator->GetGraphicsQueue(), VK_FORMAT_R8G8B8A8_SRGB, 
                                       VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mip_levels);

        Vulkan::_CopyBufferToImage(mp_instance_creator->GetDevice(), m_command_pool, mp_instance_creator->GetGraphicsQueue(), staging_buffer, texture_data.image, _width, _height);

        vkDestroyBuffer(mp_instance_creator->GetDevice(), staging_buffer, nullptr);
        vkFreeMemory(mp_instance_creator->GetDevice(), staging_memory, nullptr);

        // Either create mipmaps or transition the layout
        _CreateMipmaps(texture_data.image, _width, _height, mip_levels);

        // create texture image view
        VkImageViewCreateInfo image_view_info = Vulkan::_GetImageViewInfo(texture_data.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mip_levels);
        if(vkCreateImageView(mp_instance_creator->GetDevice(), &image_view_info, nullptr, &texture_data.image_view) != VK_SUCCESS)
            VK_RES_ERR("Failed to create texture image view");

        _CreateTextureSampler(texture_data, mip_levels);

        m_vulkan_texture_handles[_name] = texture_data;
    }


    void VulkanRenderer::RemoveTexture(const std::string &_name) {
        if(m_vulkan_texture_handles.find(_name) == m_vulkan_texture_handles.end()) {
            std::cerr << "Texture with name " << _name << " does not exist" << std::endl;
            DENG_ASSERT(false);
            return;
        }

        Vulkan::TextureData &tex = m_vulkan_texture_handles[_name];

        vkDestroySampler(mp_instance_creator->GetDevice(), tex.sampler, nullptr);
        vkDestroyImageView(mp_instance_creator->GetDevice(), tex.image_view, nullptr);
        vkDestroyImage(mp_instance_creator->GetDevice(), tex.image, nullptr);
        vkFreeMemory(mp_instance_creator->GetDevice(), tex.memory, nullptr);

        m_vulkan_texture_handles.erase(_name);
    }


    std::vector<std::string> VulkanRenderer::GetTextureNames() {
        std::vector<std::string> names;
        names.reserve(m_vulkan_texture_handles.size());

        for(auto it = m_vulkan_texture_handles.begin(); it != m_vulkan_texture_handles.end(); it++)
            names.push_back(it->first);

        return names;
    }


    uint32_t VulkanRenderer::AlignUniformBufferOffset(uint32_t _req) {
        return AlignData(_req, mp_instance_creator->GetMinimalUniformBufferAlignment());
    }


    void VulkanRenderer::LoadShaders() {
        _AllocateUniformBuffer();

        // reserve memory for shader descriptor set creators
        m_shader_desc_allocators.reserve(m_shaders.size());
        m_descriptor_set_layout_creators.reserve(m_shaders.size());
        m_pipeline_creators.reserve(m_shaders.size());

        // reserve memory for mesh descriptor set creators 
        m_mesh_desc_allocators.reserve(m_meshes.size());

        const uint32_t sc_img_count = static_cast<uint32_t>(mp_swapchain_creator->GetSwapchainImages().size());
        const Vulkan::TextureData &missing = m_vulkan_texture_handles[MISSING_TEXTURE_NAME];

        // for each shader create pipelines
        for(size_t i = 0; i < m_shaders.size(); i++) {
            m_descriptor_set_layout_creators.emplace_back(mp_instance_creator->GetDevice(), m_shaders[i]);

            // construct a temporary array containing all descriptor set layouts [ per shader, per mesh ]
            std::array<VkDescriptorSetLayout, 2> layouts = { 
                m_descriptor_set_layout_creators.back().GetPerShaderDescriptorSetLayout(), 
                m_descriptor_set_layout_creators.back().GetPerMeshDescriptorSetLayout() 
            };

            m_pipeline_creators.emplace_back(mp_instance_creator->GetDevice(), mp_swapchain_creator->GetRenderPass(), mp_swapchain_creator->GetExtent(), m_sample_count, layouts, m_shaders[i]);
            
            if(m_shaders[i].ubo_data_layouts.size()) {
                // check if per shader descriptor set layout is present
                if(layouts[0] != VK_NULL_HANDLE) {
                    const uint32_t pool_cap = static_cast<uint32_t>(m_vulkan_texture_handles.size()) > 0 ? static_cast<uint32_t>(m_vulkan_texture_handles.size()) : 1;

                    m_shader_desc_allocators.emplace_back(mp_instance_creator->GetDevice(), m_uniform_buffer, layouts[0], &m_shaders[i].ubo_data_layouts, sc_img_count, missing, pool_cap);
                    m_shader_descriptor_set_index_table.push_back(static_cast<uint32_t>(m_shader_desc_allocators.size() - 1));
                } else {
                    m_shader_descriptor_set_index_table.push_back(UINT32_MAX);
                }
            }
        }

        // for each mesh create descriptor sets if required
        for(size_t i = 0; i < m_meshes.size(); i++) {
            if(m_meshes[i].ubo_blocks.size()) {
                //m_mesh_descriptor_pool_creators.emplace_back(mp_instance_creator->GetDevice(), sc_img_count, m_meshes[i].ubo_data_layouts);
                //m_mesh_descriptor_sets_creators.emplace_back(mp_instance_creator->GetDevice(), sc_img_count, m_meshes[i], m_uniform_buffer, pool, layout);
                const uint32_t pool_cap = static_cast<uint32_t>(m_vulkan_texture_handles.size()) > 0 ? static_cast<uint32_t>(m_vulkan_texture_handles.size()) : 1;

                const VkDescriptorSetLayout layout = m_descriptor_set_layout_creators[m_meshes[i].shader_module_id].GetPerMeshDescriptorSetLayout();
                m_mesh_desc_allocators.emplace_back(mp_instance_creator->GetDevice(), m_uniform_buffer, layout, &m_meshes[i].ubo_blocks, sc_img_count, missing, pool_cap);
                m_mesh_descriptor_set_index_table.push_back(static_cast<uint32_t>(m_mesh_desc_allocators.size() - 1));
            } else {
                m_mesh_descriptor_set_index_table.push_back(UINT32_MAX);
            }
        }
    }


    void VulkanRenderer::UpdateUniform(const char *_raw_data, uint32_t _size, uint32_t _offset) {
        // check if reallocation should occur
        if(static_cast<VkDeviceSize>(_size + _offset) >= m_uniform_size) {
            const VkDeviceSize old_size = m_uniform_size;
            m_uniform_size = (_size + _offset) * 3 / 2;
            void *data = Vulkan::_CopyToDeviceMemory(mp_instance_creator->GetDevice(), static_cast<VkDeviceSize>(old_size), m_uniform_memory, 0);

            // free the current uniform buffer instance
            vkFreeMemory(mp_instance_creator->GetDevice(), m_uniform_memory, nullptr);
            vkDestroyBuffer(mp_instance_creator->GetDevice(), m_uniform_buffer, nullptr);

            // allocate new uniform buffer instances
            VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(mp_instance_creator->GetDevice(), m_uniform_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, m_uniform_buffer);
            Vulkan::_AllocateMemory(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), mem_req.size, m_uniform_memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            vkBindBufferMemory(mp_instance_creator->GetDevice(), m_uniform_buffer, m_uniform_memory, 0);

            Vulkan::_CopyToBufferMemory(mp_instance_creator->GetDevice(), static_cast<VkDeviceSize>(_size), data, m_uniform_memory, 0);

            std::free(data);
        }

        Vulkan::_CopyToBufferMemory(mp_instance_creator->GetDevice(), static_cast<VkDeviceSize>(_size), _raw_data, m_uniform_memory, static_cast<VkDeviceSize>(_offset));
    }


    void VulkanRenderer::UpdateCombinedBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset) {
        // check if reallocation should occur
        if(static_cast<VkDeviceSize>(_raw_data.second + _offset) >= m_combined_size) {
            VkDeviceSize old_size = m_combined_size;
            m_combined_size = (_raw_data.second + _offset) * 3 / 2;
            m_vertices_size = m_combined_size / 2;
            m_indices_size = m_combined_size - m_vertices_size;
            _ReallocateBufferResources(old_size);
        }

        Vulkan::_ImplicitDataToBufferCopy(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), m_command_pool, mp_instance_creator->GetGraphicsQueue(), static_cast<VkDeviceSize>(_raw_data.second), 
                                          reinterpret_cast<const void*>(_raw_data.first), m_main_buffer, static_cast<VkDeviceSize>(_offset));
    }


    void VulkanRenderer::UpdateVertexBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset) {
        // check if reallocation should occur
        if(static_cast<VkDeviceSize>(_raw_data.second + _offset) >= m_vertices_size) {
            const VkDeviceSize old_size = m_combined_size;
            const VkDeviceSize old_idx_offset = m_vertices_size;
            m_vertices_size = (_raw_data.second + _offset) * 3 / 2;
            m_combined_size = m_vertices_size + m_indices_size;
            _ReallocateBufferResources(old_size, m_indices_size, old_idx_offset);
        }

        Vulkan::_ImplicitDataToBufferCopy(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), m_command_pool, mp_instance_creator->GetGraphicsQueue(), static_cast<VkDeviceSize>(_raw_data.second), 
                                          reinterpret_cast<const void*>(_raw_data.first), m_main_buffer, static_cast<VkDeviceSize>(_offset));
    }


    void VulkanRenderer::UpdateIndexBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset) {
        // check if reallocation should occur
        if(static_cast<VkDeviceSize>(_raw_data.second + _offset) >= m_indices_size) {
            const VkDeviceSize old_size = m_combined_size;
            const VkDeviceSize old_idx_size = m_indices_size;
            const VkDeviceSize old_idx_offset = m_vertices_size;
            m_indices_size = (_raw_data.second + _offset) * 3 / 2;
            m_combined_size = m_vertices_size + m_indices_size;
            _ReallocateBufferResources(old_size, old_idx_size, old_idx_offset);
        }

        Vulkan::_ImplicitDataToBufferCopy(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), m_command_pool, mp_instance_creator->GetGraphicsQueue(), static_cast<VkDeviceSize>(_raw_data.second), 
                                          reinterpret_cast<const void*>(_raw_data.first), m_main_buffer, m_vertices_size + static_cast<VkDeviceSize>(_offset));
    }


    void VulkanRenderer::ClearFrame() {
        vkWaitForFences(mp_instance_creator->GetDevice(), 1, &m_flight_fences[m_current_frame], VK_TRUE, UINT64_MAX);
    }


    void VulkanRenderer::RenderFrame() {
        // acquire next images to draw
        uint32_t imgi;
        VkResult res = vkAcquireNextImageKHR(mp_instance_creator->GetDevice(), mp_swapchain_creator->GetSwapchain(), UINT64_MAX, m_image_available_semaphores[m_current_frame], VK_NULL_HANDLE, &imgi);

        if(res == VK_ERROR_OUT_OF_DATE_KHR) {
            _Resize();
            return;
        }
        else if(res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
            VK_FRAME_ERR("failed to acquire next swapchain image");

        vkResetFences(mp_instance_creator->GetDevice(), 1, &m_flight_fences[m_current_frame]);

        vkResetCommandBuffer(m_command_buffers[m_current_frame], 0);
        _RecordCommandBuffer(imgi);

        VkSubmitInfo submitinfo = {};
        submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitinfo.waitSemaphoreCount = 1;
        submitinfo.pWaitSemaphores = &m_image_available_semaphores[m_current_frame];
        submitinfo.pWaitDstStageMask = wait_stages;
        submitinfo.commandBufferCount = 1;
        submitinfo.pCommandBuffers = &m_command_buffers[m_current_frame];
        submitinfo.signalSemaphoreCount = 1;
        submitinfo.pSignalSemaphores = &m_render_finished_semaphores[m_current_frame];

        if(vkQueueSubmit(mp_instance_creator->GetGraphicsQueue(), 1, &submitinfo, m_flight_fences[m_current_frame]) != VK_SUCCESS) 
            VK_FRAME_ERR("failed to submit draw command; error code: " + std::to_string(res));

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &m_render_finished_semaphores[m_current_frame];
        present_info.pImageIndices = &imgi;
        
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &mp_swapchain_creator->GetSwapchain();

        res = vkQueuePresentKHR(mp_instance_creator->GetPresentationQueue(), &present_info);

        if(res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || m_window.IsResized())
            _Resize();
        else if(res != VK_SUCCESS)
            VK_FRAME_ERR("Failed to present swapchain image");

        m_current_frame = (m_current_frame + 1) % static_cast<size_t>(mp_swapchain_creator->GetSwapchainImages().size());
    }


    void VulkanRenderer::_CreateCommandPool() {
        VkCommandPoolCreateInfo cmd_pool_createinfo = {};
        cmd_pool_createinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmd_pool_createinfo.queueFamilyIndex = mp_instance_creator->GetGraphicsFamilyIndex();
        cmd_pool_createinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        // Create cmd_pool
        if(vkCreateCommandPool(mp_instance_creator->GetDevice(), &cmd_pool_createinfo, NULL, &m_command_pool) != VK_SUCCESS)
            VK_DRAWCMD_ERR("failed to create command pool");
    }


    void VulkanRenderer::_CreateSemaphores() {
        // Resize semaphores 
        m_image_available_semaphores.resize(mp_swapchain_creator->GetSwapchainImages().size());
        m_render_finished_semaphores.resize(mp_swapchain_creator->GetSwapchainImages().size());
        m_flight_fences.resize(mp_swapchain_creator->GetSwapchainImages().size());
        VkSemaphoreCreateInfo semaphore_info = {};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_createinfo = {};
        fence_createinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_createinfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for(size_t i = 0; i < mp_swapchain_creator->GetSwapchainImages().size(); i++) {
            if(vkCreateSemaphore(mp_instance_creator->GetDevice(), &semaphore_info, NULL, &m_image_available_semaphores[i]) != VK_SUCCESS ||
               vkCreateSemaphore(mp_instance_creator->GetDevice(), &semaphore_info, NULL, &m_render_finished_semaphores[i]) != VK_SUCCESS ||
               vkCreateFence(mp_instance_creator->GetDevice(), &fence_createinfo, NULL, &m_flight_fences[i]) != VK_SUCCESS) 
                VK_DRAWCMD_ERR("Failed to create semaphores or fences");
            else LOG("Successfully created semaphores");
        }
    }


    void VulkanRenderer::_AllocateBufferResources() {
        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(mp_instance_creator->GetDevice(), m_combined_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | 
                                                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_main_buffer);

        Vulkan::_AllocateMemory(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), mem_req.size, m_main_memory, 
                                mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vkBindBufferMemory(mp_instance_creator->GetDevice(), m_main_buffer, m_main_memory, 0);
    }


    void VulkanRenderer::_AllocateUniformBuffer() {
        uint32_t max_size = 0;

        // find the maximum offset and size uniform block
        for(auto shader = m_shaders.begin(); shader != m_shaders.end(); shader++) {
            for(auto ubo = shader->ubo_data_layouts.begin(); ubo != shader->ubo_data_layouts.end(); ubo++) {
                if(ubo->block.size + ubo->block.offset > max_size)
                    max_size = ubo->block.size + ubo->block.offset;
            }
        }

        for(auto mesh = m_meshes.begin(); mesh != m_meshes.end(); mesh++) {
            for(auto ubo = mesh->ubo_blocks.begin(); ubo != mesh->ubo_blocks.end(); ubo++) {
                if(ubo->size + ubo->offset > max_size)
                    max_size = ubo->size + ubo->offset;
            }
        }

        m_uniform_size = DEFAULT_UNIFORM_SIZE > max_size ? DEFAULT_UNIFORM_SIZE : max_size;

        // create and allocate uniform buffer resourses
        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(mp_instance_creator->GetDevice(), m_uniform_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, m_uniform_buffer);
        Vulkan::_AllocateMemory(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), mem_req.size, m_uniform_memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkBindBufferMemory(mp_instance_creator->GetDevice(), m_uniform_buffer, m_uniform_memory, 0);
    }


    void VulkanRenderer::_ReallocateBufferResources(VkDeviceSize _old_size, VkDeviceSize _old_index_size, VkDeviceSize _old_index_offset) {
        // step 1: create staging buffer
        VkBuffer staging_buffer = VK_NULL_HANDLE;
        VkDeviceMemory staging_memory = VK_NULL_HANDLE;
        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(mp_instance_creator->GetDevice(), _old_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, staging_buffer);
        Vulkan::_AllocateMemory(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), mem_req.size, staging_memory, mem_req.memoryTypeBits, 
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkBindBufferMemory(mp_instance_creator->GetDevice(), staging_buffer, staging_memory, 0);

        Vulkan::_CopyBufferToBuffer(mp_instance_creator->GetDevice(), m_command_pool, mp_instance_creator->GetGraphicsQueue(), m_main_buffer, staging_buffer, _old_size, 0, 0);

        // step 2: free to old buffer instance and allocate new resources
        // NOTE: new supported sizes must be calculated beforehand
        vkFreeMemory(mp_instance_creator->GetDevice(), m_main_memory, NULL);
        vkDestroyBuffer(mp_instance_creator->GetDevice(), m_main_buffer, NULL);
        _AllocateBufferResources();

        // step 3: copy data to new offsets in new buffer
        Vulkan::_CopyBufferToBuffer(mp_instance_creator->GetDevice(), m_command_pool, mp_instance_creator->GetGraphicsQueue(), staging_buffer, m_main_buffer, _old_size, 0, 0);

        // attempt to copy indices over to the new reallocated buffer
        if(_old_index_offset)
            Vulkan::_CopyBufferToBuffer(mp_instance_creator->GetDevice(), m_command_pool, mp_instance_creator->GetGraphicsQueue(), staging_buffer, m_main_buffer, _old_index_size, _old_index_offset, m_vertices_size);
    }


    void VulkanRenderer::_CreateColorResources() {
        VkMemoryRequirements mem_req = Vulkan::_CreateImage(mp_instance_creator->GetDevice(), m_color_image, mp_swapchain_creator->GetExtent().width, mp_swapchain_creator->GetExtent().height, 1, mp_swapchain_creator->GetSwapchainFormat(), 
                                                            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_sample_count);
        Vulkan::_AllocateMemory(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), mem_req.size, m_color_image_memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vkBindImageMemory(mp_instance_creator->GetDevice(), m_color_image, m_color_image_memory, 0);

        // create image view
        VkImageViewCreateInfo img_view_create_info = Vulkan::_GetImageViewInfo(m_color_image, mp_swapchain_creator->GetSwapchainFormat(), VK_IMAGE_ASPECT_COLOR_BIT, 1);

        if(vkCreateImageView(mp_instance_creator->GetDevice(), &img_view_create_info, NULL, &m_color_image_view) != VK_SUCCESS)
            VK_RES_ERR("failed to create color image view");
    }


    void VulkanRenderer::_CreateDepthResources() {
        VkMemoryRequirements mem_req = Vulkan::_CreateImage(mp_instance_creator->GetDevice(), m_depth_image, mp_swapchain_creator->GetExtent().width, mp_swapchain_creator->GetExtent().height, 1, VK_FORMAT_D32_SFLOAT, 
                                                            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, m_sample_count);
        Vulkan::_AllocateMemory(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), mem_req.size, m_depth_image_memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vkBindImageMemory(mp_instance_creator->GetDevice(), m_depth_image, m_depth_image_memory, 0);

        // create image view
        VkImageViewCreateInfo img_view_create_info = Vulkan::_GetImageViewInfo(m_depth_image, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

        if(vkCreateImageView(mp_instance_creator->GetDevice(), &img_view_create_info, NULL, &m_depth_image_view) != VK_SUCCESS)
            VK_RES_ERR("failed to create color image view");
    }


    void VulkanRenderer::_CreateFrameBuffers() {
        m_framebuffers.resize(mp_swapchain_creator->GetSwapchainImages().size());
        std::array<VkImageView, 3> attachments;

        for(size_t i = 0; i < mp_swapchain_creator->GetSwapchainImages().size(); i++) {
            attachments = { m_color_image_view, m_depth_image_view, mp_swapchain_creator->GetSwapchainImageViews()[i] };

            VkFramebufferCreateInfo framebuffer_createinfo = {};
            framebuffer_createinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_createinfo.renderPass = mp_swapchain_creator->GetRenderPass();
            framebuffer_createinfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebuffer_createinfo.pAttachments = attachments.data();
            framebuffer_createinfo.width = mp_swapchain_creator->GetExtent().width;
            framebuffer_createinfo.height = mp_swapchain_creator->GetExtent().height;
            framebuffer_createinfo.layers = 1;

            if(vkCreateFramebuffer(mp_instance_creator->GetDevice(), &framebuffer_createinfo, NULL, &m_framebuffers[i]) != VK_SUCCESS)
                VK_RES_ERR("failed to create framebuffer!");
            
            else LOG("Framebuffer successfully created");
        }
    }


    void VulkanRenderer::_AllocateCommandBuffers() {
        m_command_buffers.resize(m_framebuffers.size());

        // Set up command buffer allocation info
        VkCommandBufferAllocateInfo cmd_buf_alloc_info = {};
        cmd_buf_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmd_buf_alloc_info.commandPool = m_command_pool;
        cmd_buf_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmd_buf_alloc_info.commandBufferCount = static_cast<uint32_t>(m_command_buffers.size());

        // Allocate command buffers
        if(vkAllocateCommandBuffers(mp_instance_creator->GetDevice(), &cmd_buf_alloc_info, m_command_buffers.data()) != VK_SUCCESS)
            VK_DRAWCMD_ERR("failed to allocate command buffers");
    }


    void VulkanRenderer::_RecordCommandBuffer(uint32_t _imgi) {
        // Record each command buffer
        VkCommandBufferBeginInfo cmd_buf_info = {};
        cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmd_buf_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        // Begin recording command buffer
        if(vkBeginCommandBuffer(m_command_buffers[m_current_frame], &cmd_buf_info) != VK_SUCCESS)
            VK_DRAWCMD_ERR("failed to begin recording command buffers");

        // set up renderpass begin info
        VkRenderPassBeginInfo renderpass_begininfo = {};
        renderpass_begininfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderpass_begininfo.renderPass = mp_swapchain_creator->GetRenderPass();
        renderpass_begininfo.framebuffer = m_framebuffers[_imgi];
        renderpass_begininfo.renderArea.offset = { 0, 0 };
        renderpass_begininfo.renderArea.extent = mp_swapchain_creator->GetExtent();

        // set up clear values
        std::array<VkClearValue, 2> clear_values;
        clear_values[0].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
        clear_values[1].depthStencil = { 1.0f, 0 };

        // add clear values to renderpass begin info
        renderpass_begininfo.clearValueCount = static_cast<uint32_t>(clear_values.size());
        renderpass_begininfo.pClearValues = clear_values.data();

        // start a new render pass for recording asset draw commands
        vkCmdBeginRenderPass(m_command_buffers[m_current_frame], &renderpass_begininfo, VK_SUBPASS_CONTENTS_INLINE);

            // Iterate through every mesh, bind resources and issue an index draw to commandbuffer
            for(size_t i = 0; i < m_meshes.size(); i++) {
                const uint32_t shader_id = m_meshes[i].shader_module_id;
                for(size_t j = 0; j < m_meshes[i].commands.size(); j++) {
                    vkCmdBindPipeline(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_creators[shader_id].GetPipeline());
                    VkDeviceSize offset = m_meshes[i].commands[j].vertices_offset;

                    // 1. bind all attribute bindings
                    // 2. bind single attribute binding
                    if(m_shaders[shader_id].use_seperate_attribute_strides) {
                        std::vector<VkBuffer> buffers(m_shaders[shader_id].attributes.size(), m_main_buffer);
                        vkCmdBindVertexBuffers(m_command_buffers[m_current_frame], 0, static_cast<uint32_t>(m_shaders[shader_id].attributes.size()), buffers.data(), m_shaders[shader_id].offsets.data());
                    } else {
                        vkCmdBindVertexBuffers(m_command_buffers[m_current_frame], 0, 1, &m_main_buffer, &offset);
                    }

                    // 1. index and vertex buffers are shared resources
                    // 2. index and vertex buffers are not shared resources
                    if(!m_conf.share_buffers)
                        offset = m_vertices_size + m_meshes[i].commands[j].indices_offset;
                    else offset = m_meshes[i].commands[j].indices_offset;

                    vkCmdBindIndexBuffer(m_command_buffers[m_current_frame], m_main_buffer, offset, VK_INDEX_TYPE_UINT32);

                    // check if descriptor sets should be bound
                    std::array<VkDescriptorSet, 2> desc_sets;
                    uint32_t used_desc_sets = 0;

                    // per shader descriptor sets
                    if(m_shaders[shader_id].ubo_data_layouts.size()) {
                        if(m_shaders[shader_id].use_texture_mapping) {
                            DENG_ASSERT(m_vulkan_texture_handles.find(m_meshes[i].commands[j].texture_name) != m_vulkan_texture_handles.end());
                            const Vulkan::TextureData &tex = m_vulkan_texture_handles[m_meshes[i].commands[j].texture_name];
                            desc_sets[0] = m_shader_desc_allocators[m_shader_descriptor_set_index_table[shader_id]].RequestDescriptorSetByTexture(m_meshes[i].commands[j].texture_name, tex, m_current_frame);
                        } else {
                            desc_sets[0] = m_shader_desc_allocators[m_shader_descriptor_set_index_table[shader_id]].RequestDescriptorSetByFrame(m_current_frame);
                        }

                        used_desc_sets++;
                    }

                    // per mesh descriptor sets
                    if(m_meshes[i].ubo_blocks.size()) {
                        if(used_desc_sets == 1)
                            desc_sets[1] = m_mesh_desc_allocators[m_mesh_descriptor_set_index_table[i]].RequestDescriptorSetByFrame(m_current_frame);
                        else desc_sets[0] = m_mesh_desc_allocators[m_mesh_descriptor_set_index_table[i]].RequestDescriptorSetByFrame(m_current_frame);

                        used_desc_sets++;
                    }

                    if(used_desc_sets)
                        vkCmdBindDescriptorSets(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_creators[shader_id].GetPipelineLayout(), 0, used_desc_sets, desc_sets.data(), 0, nullptr);

                    if(m_shaders[shader_id].enable_scissor) {
                        VkRect2D rect = {};
                        if(m_meshes[i].commands[j].scissor.enabled) {
                            rect.offset = VkOffset2D { m_meshes[i].commands[j].scissor.offset.x, m_meshes[i].commands[j].scissor.offset.y };
                            rect.extent = VkExtent2D { m_meshes[i].commands[j].scissor.ext.x, m_meshes[i].commands[j].scissor.ext.y };
                        } else {
                            rect.offset = { 0, 0 };
                            rect.extent = { static_cast<uint32_t>(m_window.GetSize().x), static_cast<uint32_t>(m_window.GetSize().y) };
                        }
                        vkCmdSetScissor(m_command_buffers[m_current_frame], 0, 1, &rect);
                    }

                    vkCmdDrawIndexed(m_command_buffers[m_current_frame], m_meshes[i].commands[j].indices_count, 1, 0, 0, 0);
                }
            }

        // End render pass
        vkCmdEndRenderPass(m_command_buffers[m_current_frame]);
        
        // Stop recording commandbuffer
        if(vkEndCommandBuffer(m_command_buffers[m_current_frame]) != VK_SUCCESS)
            VK_DRAWCMD_ERR("failed to end recording command buffer");
    }


    void VulkanRenderer::_CreateMipmaps(VkImage _img, uint32_t _width, uint32_t _height, uint32_t _mip_levels) {
        // OPTIONAL:
        // check if image format supports linear blitting
        VkFormatProperties format_props;
        vkGetPhysicalDeviceFormatProperties(mp_instance_creator->GetPhysicalDevice(), VK_FORMAT_R8G8B8A8_SRGB, &format_props);
        DENG_ASSERT(format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT);

        VkCommandBuffer cmd_buf = VK_NULL_HANDLE;
        Vulkan::_BeginCommandBufferSingleCommand(mp_instance_creator->GetDevice(), m_command_pool, cmd_buf);

        VkImageMemoryBarrier mem_barrier = {};
        mem_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        mem_barrier.image = _img;
        mem_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        mem_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        mem_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        mem_barrier.subresourceRange.baseArrayLayer = 0;
        mem_barrier.subresourceRange.layerCount = 1;
        mem_barrier.subresourceRange.levelCount = 1;

        for(uint32_t i = 1; i < _mip_levels; i++) {
            // set pipeline ready for transfer processing
            mem_barrier.subresourceRange.baseMipLevel = i - 1;
            mem_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            mem_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            mem_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            mem_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &mem_barrier);
            
            // set up image blit structure
            VkImageBlit blit = {};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { static_cast<int32_t>(_width), static_cast<int32_t>(_height), 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { 
                static_cast<int32_t>(_width) > 1 ? static_cast<int32_t>(_width) / 2 : 1, 
                static_cast<int32_t>(_height) > 1 ? static_cast<int32_t>(_height) / 2 : 1,
                1
            };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(cmd_buf, _img, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

            // set pipeline ready for final transfer into fragment shader
            mem_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            mem_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            mem_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            mem_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &mem_barrier);

            if(_width > 1) _width /= 2;
            if(_height > 1) _height /= 2;
        }

        // final mip level transitioning
        mem_barrier.subresourceRange.baseMipLevel = _mip_levels - 1;
        mem_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        mem_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        mem_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        mem_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &mem_barrier);

        Vulkan::_EndCommandBufferSingleCommand(mp_instance_creator->GetDevice(), mp_instance_creator->GetGraphicsQueue(), m_command_pool, cmd_buf);
    }


    void VulkanRenderer::_CreateTextureSampler(Vulkan::TextureData &_tex, uint32_t _mip_levels) {
        VkSamplerCreateInfo sampler_info = {};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable = VK_TRUE;
        sampler_info.maxAnisotropy = mp_instance_creator->GetMaxSamplerAnisotropy();
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = static_cast<float>(_mip_levels);
        sampler_info.mipLodBias = 0.0f;

        if(vkCreateSampler(mp_instance_creator->GetDevice(), &sampler_info, nullptr, &_tex.sampler) != VK_SUCCESS)
            VK_RES_ERR("Failed to create a texture sampler");
    }


    void VulkanRenderer::_Resize() {
        while(m_window.IsResized())
            m_window.Update();

        vkDeviceWaitIdle(mp_instance_creator->GetDevice());

        // destroy color and depth resources
        vkDestroyImageView(mp_instance_creator->GetDevice(), m_color_image_view, nullptr);
        vkDestroyImage(mp_instance_creator->GetDevice(), m_color_image, nullptr);
        vkFreeMemory(mp_instance_creator->GetDevice(), m_color_image_memory, nullptr);

        vkDestroyImageView(mp_instance_creator->GetDevice(), m_depth_image_view, nullptr);
        vkDestroyImage(mp_instance_creator->GetDevice(), m_depth_image, nullptr);
        vkFreeMemory(mp_instance_creator->GetDevice(), m_depth_image_memory, nullptr);

        // free framebuffers
        for(auto it = m_framebuffers.begin(); it != m_framebuffers.end(); it++)
            vkDestroyFramebuffer(mp_instance_creator->GetDevice(), *it, nullptr);

        // destroy all previously created pipelines 
        vkResetCommandPool(mp_instance_creator->GetDevice(), m_command_pool, 0);

        // remove shader pipeline resources
        for(auto &pc : m_pipeline_creators)
            pc.DestroyPipelineData();

        mp_swapchain_creator->RecreateSwapchain(m_window.GetSize());

        _CreateColorResources();
        _CreateDepthResources();

        // create new pipelines
        for(auto &pc : m_pipeline_creators)
            pc.RecreatePipeline(mp_swapchain_creator->GetRenderPass(), mp_swapchain_creator->GetExtent());

        _CreateFrameBuffers();

        vkDeviceWaitIdle(mp_instance_creator->GetDevice());
    }
}
