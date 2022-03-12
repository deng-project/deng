// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanRenderer.cpp - Vulkan renderer class implementation
// author: Karl-Mihkel Ott

#define VULKAN_RENDERER_CPP
#include <VulkanRenderer.h>


namespace DENG {

    VulkanRenderer::VulkanRenderer(const Window &_win) : Renderer(_win) {
        mp_instance_creator = new Vulkan::InstanceCreator(m_window);
        mp_swapchain_creator = new Vulkan::SwapchainCreator(mp_instance_creator, m_window.GetSize(), m_sample_count);
        _CreateCommandPool();
        _CreateSemaphores();
        _AllocateBufferResources();
        _CreateColorResources();
        _CreateDepthResources();
        _CreateFrameBuffers();
        _AllocateCommandBuffers();
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
            vkDestroySampler(mp_instance_creator->GetDevice(), it->sampler, nullptr);
            vkDestroyImageView(mp_instance_creator->GetDevice(), it->image_view, nullptr);
            vkDestroyImage(mp_instance_creator->GetDevice(), it->image, nullptr);
            vkFreeMemory(mp_instance_creator->GetDevice(), it->memory, nullptr);
        }


        // Free all command buffers and framebuffers
        for(size_t i = 0; i < m_framebuffers.size(); i++)
            vkDestroyFramebuffer(mp_instance_creator->GetDevice(), m_framebuffers[i], NULL);

        vkFreeCommandBuffers(mp_instance_creator->GetDevice(), m_command_pool, static_cast<uint32_t>(m_command_buffers.size()), m_command_buffers.data());

        // free all allocated descriptor sets and pools
        while(m_descriptor_sets_creators.size())
            m_descriptor_sets_creators.erase(m_descriptor_sets_creators.end() - 1);

        while(m_descriptor_pool_creators.size())
            m_descriptor_pool_creators.erase(m_descriptor_pool_creators.end() - 1);

        while(m_pipeline_creators.size())
            m_pipeline_creators.erase(m_pipeline_creators.end() - 1);

        while(m_descriptor_set_layout_creators.size())
            m_descriptor_set_layout_creators.erase(m_descriptor_set_layout_creators.end() - 1);

        for(size_t i = 0; i < mp_swapchain_creator->GetSwapchainImages().size(); i++) {
            vkDestroySemaphore(mp_instance_creator->GetDevice(), m_render_finished_semaphores[i], NULL);
            vkDestroySemaphore(mp_instance_creator->GetDevice(), m_image_available_semaphores[i], NULL);
            vkDestroyFence(mp_instance_creator->GetDevice(), m_flight_fences[i], NULL);
        }

        delete mp_ubo_allocator;

        // free main buffers
        vkDestroyBuffer(mp_instance_creator->GetDevice(), m_main_buffer, NULL);
        vkFreeMemory(mp_instance_creator->GetDevice(), m_main_memory, NULL);

        delete mp_swapchain_creator;
        delete mp_instance_creator;
    }


    uint32_t VulkanRenderer::PushTextureFromFile(const DENG::TextureReference &_tex, const std::string &_file_name) {
        Libdas::TextureReader reader(_file_name, true);
        int x, y;
        size_t len;
        const char *raw = reader.GetRawBuffer(x, y, len);

        return PushTextureFromMemory(_tex, raw, static_cast<uint32_t>(x), static_cast<uint32_t>(y), 4);
    }
    

    uint32_t VulkanRenderer::PushTextureFromMemory(const DENG::TextureReference &_tex, const char *_raw_data, uint32_t _width, uint32_t _height, uint32_t _bit_depth) {
        // image data size
        const VkDeviceSize size = static_cast<VkDeviceSize>(_width * _height * _bit_depth);
        m_textures.push_back(_tex);
        m_textures.back().r_identifier = static_cast<uint32_t>(m_vulkan_texture_handles.size());

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
        m_textures.back().r_identifier = static_cast<uint32_t>(m_vulkan_texture_handles.size());
        m_vulkan_texture_handles.push_back(texture_data);

        return m_textures.back().r_identifier;
    }


    void VulkanRenderer::ShrinkTextures() {
        std::vector<bool> shrink_table;
        shrink_table.resize(m_vulkan_texture_handles.size());
        std::fill(shrink_table.begin(), shrink_table.end(), true);

        for(size_t i = 0; i < m_textures.size(); i++)
            shrink_table[m_textures[i].r_identifier] = false;

        size_t delta_shrink = 0;
        for(size_t i = 0; i < shrink_table.size(); i++) {
            if(shrink_table[i]) {
                vkDestroySampler(mp_instance_creator->GetDevice(), m_vulkan_texture_handles[i - delta_shrink].sampler, nullptr);
                vkDestroyImageView(mp_instance_creator->GetDevice(), m_vulkan_texture_handles[i - delta_shrink].image_view, nullptr);
                vkDestroyImage(mp_instance_creator->GetDevice(), m_vulkan_texture_handles[i - delta_shrink].image, nullptr);
                vkFreeMemory(mp_instance_creator->GetDevice(), m_vulkan_texture_handles[i - delta_shrink].memory, nullptr);
                delta_shrink++;
                m_vulkan_texture_handles.erase(m_vulkan_texture_handles.begin() + (i - delta_shrink));
            }
        }
    }


    void VulkanRenderer::LoadShaders() {
        mp_ubo_allocator = new Vulkan::UniformBufferAllocator(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), mp_instance_creator->GetGraphicsQueue(),
                                                              m_command_pool, mp_instance_creator->GetMinimalUniformBufferAlignment(), static_cast<uint32_t>(mp_swapchain_creator->GetSwapchainImages().size()), m_shaders);

        // for each shader module create descriptor pool creators
        m_descriptor_pool_creators.reserve(m_shaders.size());
        m_descriptor_sets_creators.reserve(m_shaders.size());
        m_descriptor_set_layout_creators.reserve(m_shaders.size());
        m_pipeline_creators.reserve(m_shaders.size());
        for(size_t i = 0; i < m_shaders.size(); i++) {
            m_descriptor_set_layout_creators.emplace_back(mp_instance_creator->GetDevice(), m_shaders[i]);
            m_pipeline_creators.emplace_back(mp_instance_creator->GetDevice(), mp_swapchain_creator->GetRenderPass(), mp_swapchain_creator->GetExtent(), m_sample_count, 
                                                 m_descriptor_set_layout_creators.back().GetDescriptorSetLayout(), m_shaders[i]);
            if(m_shaders[i].ubo_data_layouts.size()) {
                m_descriptor_pool_creators.emplace_back(mp_instance_creator->GetDevice(), static_cast<uint32_t>(mp_swapchain_creator->GetSwapchainImages().size()), static_cast<uint32_t>(m_textures.size()), m_shaders[i].ubo_data_layouts);
                m_descriptor_sets_creators.emplace_back(mp_instance_creator->GetDevice(), static_cast<uint32_t>(mp_swapchain_creator->GetSwapchainImages().size()), m_shaders[i], i, mp_ubo_allocator, m_descriptor_pool_creators[i].GetDescriptorPool(),
                                                        m_descriptor_set_layout_creators[i].GetDescriptorSetLayout(), m_vulkan_texture_handles);
            }
            else {
                m_descriptor_pool_creators.emplace_back();
                m_descriptor_sets_creators.emplace_back();
            }
        }
    }


    void VulkanRenderer::UpdateUniform(char *_raw_data, uint32_t _shader_id, uint32_t _ubo_id) {
        UniformDataLayout &layout = m_shaders[_shader_id].ubo_data_layouts[_ubo_id];
        DENG_ASSERT(layout.type == UNIFORM_DATA_TYPE_BUFFER);

        Vulkan::_CopyToBufferMemory(mp_instance_creator->GetDevice(), static_cast<VkDeviceSize>(layout.ubo_size), _raw_data, 
                                    mp_ubo_allocator->GetUniformMemory(), mp_ubo_allocator->GetAreaOffset(_shader_id, _ubo_id));
    }


    void VulkanRenderer::UpdateVertexBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset) {
        // check if reallocation should occur
        if(static_cast<VkDeviceSize>(_raw_data.second + _offset) >= m_vertices_size) {
            VkDeviceSize old_size = m_vertices_size;
            m_vertices_size = (_raw_data.second + _offset) * 3 / 2;
            _ReallocateBufferResources(old_size, m_indices_size);
        }

        // create a staging buffer to hold data in
        VkBuffer staging_buffer = VK_NULL_HANDLE;
        VkDeviceMemory staging_memory = VK_NULL_HANDLE;

        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(mp_instance_creator->GetDevice(), static_cast<VkDeviceSize>(_raw_data.second), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging_buffer);
        Vulkan::_AllocateMemory(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), mem_req.size, 
                                staging_memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        vkBindBufferMemory(mp_instance_creator->GetDevice(), staging_buffer, staging_memory, 0);
        Vulkan::_CopyToBufferMemory(mp_instance_creator->GetDevice(), static_cast<VkDeviceSize>(_raw_data.second), _raw_data.first, staging_memory, 0);

        // copy data from staging buffer to main buffer
        Vulkan::_CopyBufferToBuffer(mp_instance_creator->GetDevice(), m_command_pool, mp_instance_creator->GetGraphicsQueue(), staging_buffer, m_main_buffer, 
                                    static_cast<VkDeviceSize>(_raw_data.second), 0, static_cast<VkDeviceSize>(_offset));

        // destroy the staging buffer
        vkFreeMemory(mp_instance_creator->GetDevice(), staging_memory, NULL);
        vkDestroyBuffer(mp_instance_creator->GetDevice(), staging_buffer, NULL);
    }


    void VulkanRenderer::UpdateIndexBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset) {
        // check if reallocation should occur
        if(static_cast<VkDeviceSize>(_raw_data.second + _offset) >= m_indices_size) {
            VkDeviceSize old_size = m_indices_size;
            m_indices_size = (_raw_data.second + _offset) * 3 / 2;
            _ReallocateBufferResources(m_vertices_size, old_size);
        }

        // create a staging buffer to hold data in
        VkBuffer staging_buffer = VK_NULL_HANDLE;
        VkDeviceMemory staging_memory = VK_NULL_HANDLE;

        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(mp_instance_creator->GetDevice(), static_cast<VkDeviceSize>(_raw_data.second), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging_buffer);
        Vulkan::_AllocateMemory(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), mem_req.size, 
                                staging_memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        vkBindBufferMemory(mp_instance_creator->GetDevice(), staging_buffer, staging_memory, 0);
        Vulkan::_CopyToBufferMemory(mp_instance_creator->GetDevice(), static_cast<VkDeviceSize>(_raw_data.second), _raw_data.first, staging_memory, 0);

        // copy data from staging buffer to main buffer
        Vulkan::_CopyBufferToBuffer(mp_instance_creator->GetDevice(), m_command_pool, mp_instance_creator->GetGraphicsQueue(), staging_buffer, m_main_buffer, 
                                    static_cast<VkDeviceSize>(_raw_data.second), 0, m_vertices_size + static_cast<VkDeviceSize>(_offset));

        // destroy the staging buffer
        vkFreeMemory(mp_instance_creator->GetDevice(), staging_memory, NULL);
        vkDestroyBuffer(mp_instance_creator->GetDevice(), staging_buffer, NULL);
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
        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(mp_instance_creator->GetDevice(), m_vertices_size + m_indices_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | 
                                                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_main_buffer);

        Vulkan::_AllocateMemory(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), mem_req.size, m_main_memory, 
                                mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vkBindBufferMemory(mp_instance_creator->GetDevice(), m_main_buffer, m_main_memory, 0);
    }


    void VulkanRenderer::_ReallocateBufferResources(VkDeviceSize _old_vert_size, VkDeviceSize _old_ind_size) {
        // step 1: create staging buffer
        VkBuffer staging_buffer;
        VkDeviceMemory staging_memory;
        VkMemoryRequirements mem_req = Vulkan::_CreateBuffer(mp_instance_creator->GetDevice(), _old_vert_size + _old_ind_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, staging_buffer);
        Vulkan::_AllocateMemory(mp_instance_creator->GetDevice(), mp_instance_creator->GetPhysicalDevice(), mem_req.size, staging_memory, mem_req.memoryTypeBits, 
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vkBindBufferMemory(mp_instance_creator->GetDevice(), staging_buffer, staging_memory, 0);

        Vulkan::_CopyBufferToBuffer(mp_instance_creator->GetDevice(), m_command_pool, mp_instance_creator->GetGraphicsQueue(), m_main_buffer, staging_buffer, _old_vert_size + _old_ind_size, 0, 0);

        // step 2: free to old buffer instance and allocate new resources
        // NOTE: new supported sizes must be calculated beforehand
        vkFreeMemory(mp_instance_creator->GetDevice(), m_main_memory, NULL);
        vkDestroyBuffer(mp_instance_creator->GetDevice(), m_main_buffer, NULL);
        _AllocateBufferResources();

        // step 3: copy data to new offsets in new buffer
        Vulkan::_CopyBufferToBuffer(mp_instance_creator->GetDevice(), m_command_pool, mp_instance_creator->GetGraphicsQueue(), staging_buffer, m_main_buffer, _old_vert_size, 0, 0);
        Vulkan::_CopyBufferToBuffer(mp_instance_creator->GetDevice(), m_command_pool, mp_instance_creator->GetGraphicsQueue(), staging_buffer, m_main_buffer, _old_ind_size, _old_vert_size, m_vertices_size);
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

        // Set up renderpass begin info
        VkRenderPassBeginInfo renderpass_begininfo = {};
        renderpass_begininfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderpass_begininfo.renderPass = mp_swapchain_creator->GetRenderPass();
        renderpass_begininfo.framebuffer = m_framebuffers[_imgi];
        renderpass_begininfo.renderArea.offset = { 0, 0 };
        renderpass_begininfo.renderArea.extent = mp_swapchain_creator->GetExtent();

        // Set up clear values
        std::array<VkClearValue, 2> clear_values;
        clear_values[0].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
        clear_values[1].depthStencil = { 1.0f, 0 };

        // Add clear values to renderpass begin info
        renderpass_begininfo.clearValueCount = static_cast<uint32_t>(clear_values.size());
        renderpass_begininfo.pClearValues = clear_values.data();

        // Start a new render pass for recording asset draw commands
        vkCmdBeginRenderPass(m_command_buffers[m_current_frame], &renderpass_begininfo, VK_SUBPASS_CONTENTS_INLINE);

            // Iterate through every mesh, bind resources and issue an index draw to commandbuffer
            for(size_t i = 0; i < m_meshes.size(); i++) {
                const uint32_t shader_id = m_meshes[i].shader_module_id;
                for(size_t j = 0; j < m_meshes[i].commands.size(); j++) {
                    vkCmdBindPipeline(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_creators[shader_id].GetPipeline());
                    VkDeviceSize offset = m_meshes[i].commands[j].vertices_offset;
                    vkCmdBindVertexBuffers(m_command_buffers[m_current_frame], 0, 1, &m_main_buffer, &offset);
                    offset = m_vertices_size + m_meshes[i].commands[j].indices_offset;
                    vkCmdBindIndexBuffer(m_command_buffers[m_current_frame], m_main_buffer, offset, VK_INDEX_TYPE_UINT32);

                    // check if descriptor sets should be bound
                    if(m_shaders[shader_id].ubo_data_layouts.size()) {
                        uint32_t descriptor_set_index = m_meshes[i].commands[j].texture_id == UINT32_MAX ? static_cast<uint32_t>(m_current_frame) : m_meshes[i].commands[j].texture_id * static_cast<uint32_t>(mp_swapchain_creator->GetSwapchainImages().size()) + static_cast<uint32_t>(m_current_frame);
                        VkDescriptorSet desc_set = m_descriptor_sets_creators[shader_id].GetDescriptorSetById(descriptor_set_index);

                        vkCmdBindDescriptorSets(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_creators[shader_id].GetPipelineLayout(), 0, 1, &desc_set, 0, NULL);
                    }

                    if(m_meshes[i].commands[j].scissor.enabled) {
                        VkRect2D rect = {};
                        rect.offset = VkOffset2D { m_meshes[i].commands[j].scissor.offset.x, m_meshes[i].commands[j].scissor.offset.y };
                        rect.extent = VkExtent2D { m_meshes[i].commands[j].scissor.ext.x, m_meshes[i].commands[j].scissor.ext.y };
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
