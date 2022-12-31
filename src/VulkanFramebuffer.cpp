// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanFramebuffer.cpp - Vulkan framebuffer management class implementation
// author: Karl-Mihkel Ott

#define VULKAN_FRAMEBUFFER_CPP
#include "deng/VulkanFramebuffer.h"


namespace DENG {
    namespace Vulkan {

        Framebuffer::Framebuffer(
            InstanceCreator &_ic, 
            VkBuffer &_uniform, 
            VkBuffer &_main, 
            VkSampleCountFlagBits _sample_c,
            TRS::Point2D<uint32_t> _extent,
            uint32_t _missing_2d,
            uint32_t _missing_3d,
            bool _no_swapchain
        ) :
            m_instance_creator(_ic),
            m_uniform(_uniform),
            m_main_buffer(_main),
            m_sample_count(_sample_c),
            m_no_swapchain(_no_swapchain),
            m_extent(_extent),
            m_missing_2d(_missing_2d),
            m_missing_3d(_missing_3d),
            m_current_frame(0)
        {
            if(m_no_swapchain) {
                m_renderpass = Vulkan::SwapchainCreator::CreateRenderPass(m_instance_creator.GetDevice(), VK_FORMAT_DEFAULT_IMAGE, VK_SAMPLE_COUNT_1_BIT, m_no_swapchain);
                _CreateFramebufferImage();
            } else {
                m_swapchain_creator = new SwapchainCreator(m_instance_creator, m_extent, m_sample_count);
                m_renderpass = Vulkan::SwapchainCreator::CreateRenderPass(m_instance_creator.GetDevice(), VK_FORMAT_DEFAULT_IMAGE, m_sample_count, m_no_swapchain);
                m_framebuffer_image_ids = m_swapchain_creator->GetSwapchainImageIds();
            }
            _CreateColorResources();
            _CreateDepthResources();
            _CreateFramebuffers();
            _CreateCommandPool();
            _AllocateCommandBuffers();
            CreateSynchronisationPrimitives();
        }


        Framebuffer::~Framebuffer() {
            const VkDevice device = m_instance_creator.GetDevice();

            TextureDatabase* db = TextureDatabase::GetInstance();

            // clean depth image resources
            Vulkan::TextureData tex_data = std::get<Vulkan::TextureData>(db->GetResource(m_depth_image_id).vendor);
            vkDestroyImageView(device, tex_data.image_view, nullptr);
            vkDestroyImage(device, tex_data.image, nullptr);
            vkFreeMemory(device, tex_data.memory, nullptr);

            // clean color image resources
            tex_data = std::get<Vulkan::TextureData>(db->GetResource(m_color_resolve_image_id).vendor);
            vkDestroyImageView(device, tex_data.image_view, nullptr);
            vkDestroyImage(device, tex_data.image, nullptr);
            vkFreeMemory(device, tex_data.memory, nullptr);

            // destroy framebuffer resources
            for(VkFramebuffer fb : m_framebuffers)
                vkDestroyFramebuffer(device, fb, nullptr);

            // free commandbuffers and -pools
            vkFreeCommandBuffers(device, m_command_pool, static_cast<uint32_t>(m_command_buffers.size()), m_command_buffers.data());
            vkDestroyCommandPool(device, m_command_pool, nullptr);

            _CleanPipelineResources();

            // destroy semaphores and fences
            DestroySynchronisationPrimitives();
        }


        void Framebuffer::_CreateColorResources() {
            const VkDevice device = m_instance_creator.GetDevice();
            const VkPhysicalDevice gpu = m_instance_creator.GetPhysicalDevice();
            
            TextureResource res = {};
            res.width = m_extent.x;
            res.height = m_extent.y;
            res.bit_depth = 4;
            res.load_type = TEXTURE_RESOURCE_LOAD_TYPE_EMBEDDED;
            res.resource_type = TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_2D_IMAGE;

            Vulkan::TextureData tex_data = {};

            VkMemoryRequirements mem_req = Vulkan::_CreateImage(
                device, 
                tex_data.image,
                m_extent.x, 
                m_extent.y, 
                1,
                1,
                VK_FORMAT_DEFAULT_IMAGE, 
                VK_IMAGE_TILING_OPTIMAL, 
                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
                m_sample_count,
                0);
            Vulkan::_AllocateMemory(device, gpu, mem_req.size, tex_data.memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            vkBindImageMemory(device, tex_data.image, tex_data.memory, 0);

            // create image view
            VkImageViewCreateInfo image_view_info = {};
            image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_info.image = tex_data.image;
            image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_info.format = VK_FORMAT_DEFAULT_IMAGE;
            
            image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_info.subresourceRange.baseMipLevel = 0;
            image_view_info.subresourceRange.levelCount = 1;
            image_view_info.subresourceRange.baseArrayLayer = 0;
            image_view_info.subresourceRange.layerCount = 1;

            if(vkCreateImageView(device, &image_view_info, NULL, &tex_data.image_view) != VK_SUCCESS)
                VK_RES_ERR("failed to create color image view");

            res.vendor = tex_data;
            
            TextureDatabase* db = TextureDatabase::GetInstance();

            if (m_color_resolve_image_id == UINT32_MAX) {
                m_color_resolve_image_id = db->AddResource(res);
            } else {
                db->GetResource(m_color_resolve_image_id) = res;
            }
        }


        void Framebuffer::_CreateDepthResources() {
            const VkDevice device = m_instance_creator.GetDevice();
            const VkPhysicalDevice gpu = m_instance_creator.GetPhysicalDevice();

            Vulkan::TextureData tex_data = {};
            TextureResource res;
            res.width = m_extent.x;
            res.height = m_extent.y;
            res.bit_depth = 1;
            res.load_type = TEXTURE_RESOURCE_LOAD_TYPE_EMBEDDED;
            res.resource_type = TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_2D_IMAGE;
            
            VkMemoryRequirements mem_req = Vulkan::_CreateImage(
                device, 
                tex_data.image,
                m_extent.x, 
                m_extent.y, 
                1,
                1,
                VK_FORMAT_D32_SFLOAT, 
                VK_IMAGE_TILING_OPTIMAL, 
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
                m_sample_count,
                0);

            Vulkan::_AllocateMemory(device, gpu, mem_req.size, tex_data.memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            vkBindImageMemory(device, tex_data.image, tex_data.memory, 0);

            // create image view info
            VkImageViewCreateInfo image_view_info = {};
            image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_info.image = tex_data.image;
            image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            image_view_info.format = VK_FORMAT_D32_SFLOAT;

            image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            image_view_info.subresourceRange.baseMipLevel = 0;
            image_view_info.subresourceRange.levelCount = 1;
            image_view_info.subresourceRange.baseArrayLayer = 0;
            image_view_info.subresourceRange.layerCount = 1;

            if(vkCreateImageView(device, &image_view_info, NULL, &tex_data.image_view) != VK_SUCCESS)
                VK_RES_ERR("failed to create color image view");
            
            res.vendor = tex_data;

            TextureDatabase* db = TextureDatabase::GetInstance();

            if (m_depth_image_id == UINT32_MAX) {
                m_depth_image_id = db->AddResource(res);
            } else {
                db->GetResource(m_depth_image_id) = res;
            }
        }


        void Framebuffer::_CreateFramebuffers() {
            m_framebuffers.resize(m_framebuffer_image_ids.size());
            std::array<VkImageView, 2> attachments;

            TextureDatabase* db = TextureDatabase::GetInstance();
            Vulkan::TextureData depth_img_data = std::get<Vulkan::TextureData>(db->GetResource(m_depth_image_id).vendor);

            for(size_t i = 0; i < m_framebuffer_image_ids.size(); i++) {
                Vulkan::TextureData fb_img_data = std::get<Vulkan::TextureData>(db->GetResource(m_framebuffer_image_ids[i]).vendor);
                attachments = { 
                    fb_img_data.image_view,
                    depth_img_data.image_view
                };

                VkFramebufferCreateInfo framebuffer_createinfo = {};
                framebuffer_createinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebuffer_createinfo.renderPass = m_renderpass;
                framebuffer_createinfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                framebuffer_createinfo.pAttachments = attachments.data();
                framebuffer_createinfo.width = m_extent.x;
                framebuffer_createinfo.height = m_extent.y;
                framebuffer_createinfo.layers = 1;

                if(vkCreateFramebuffer(m_instance_creator.GetDevice(), &framebuffer_createinfo, NULL, &m_framebuffers[i]) != VK_SUCCESS) {
                    VK_RES_ERR("failed to create framebuffer!");
                } else {
                    LOG("Framebuffer successfully created");
                }
            }
        }


        void Framebuffer::_CreateFramebufferImage() {
            // create a framebuffer image
            TextureResource res;
            res.width = m_extent.x;
            res.height = m_extent.y;
            res.bit_depth = 4;
            res.load_type = TEXTURE_RESOURCE_LOAD_TYPE_EMBEDDED;
            res.resource_type = TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_2D_IMAGE;

            Vulkan::TextureData data;
            VkMemoryRequirements mem_req = Vulkan::_CreateImage(
                m_instance_creator.GetDevice(),
                data.image,
                m_extent.x,
                m_extent.y,
                1,
                1,
                VK_FORMAT_DEFAULT_IMAGE,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_SAMPLE_COUNT_1_BIT,
                0);

            Vulkan::_AllocateMemory(
                m_instance_creator.GetDevice(),
                m_instance_creator.GetPhysicalDevice(),
                mem_req.size,
                data.memory,
                mem_req.memoryTypeBits,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            vkBindImageMemory(m_instance_creator.GetDevice(), data.image, data.memory, 0);

            // create image view
            VkImageViewCreateInfo image_view_info = Vulkan::_GetImageViewInfo(data.image, VK_FORMAT_DEFAULT_IMAGE, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);
            if (vkCreateImageView(m_instance_creator.GetDevice(), &image_view_info, nullptr, &data.image_view) != VK_SUCCESS)
                VK_RES_ERR("Failed to create a framebuffer image.");

            // image sampler
            data.sampler = Vulkan::_CreateTextureSampler(m_instance_creator.GetDevice(), m_instance_creator.GetMaxSamplerAnisotropy(), 1);
            res.vendor = data;

            TextureDatabase* db = TextureDatabase::GetInstance();
            m_framebuffer_image_ids.push_back(db->AddResource(res));
        }


        void Framebuffer::_CleanPipelineResources() {
            // free all allocated descriptor sets and pools
            m_shader_desc_allocators.clear();
            m_mesh_desc_allocators.clear();

            // destroy pipelines and descriptor set layouts
            m_pipeline_creators.clear();
            m_shader_descriptor_set_layout_creators.clear();
            m_mesh_descriptor_set_layout_creators.clear();
        }


        void Framebuffer::_CreateCommandPool() {
            VkCommandPoolCreateInfo cmd_pool_createinfo = {};
            cmd_pool_createinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            cmd_pool_createinfo.queueFamilyIndex = m_instance_creator.GetGraphicsFamilyIndex();
            cmd_pool_createinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

            // Create cmd_pool
            if(vkCreateCommandPool(m_instance_creator.GetDevice(), &cmd_pool_createinfo, NULL, &m_command_pool) != VK_SUCCESS)
                VK_DRAWCMD_ERR("failed to create command pool");
        }


        void Framebuffer::_AllocateCommandBuffers() {
            m_command_buffers.resize(m_framebuffers.size());

            // Set up command buffer allocation info
            VkCommandBufferAllocateInfo cmd_buf_alloc_info = {};
            cmd_buf_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmd_buf_alloc_info.commandPool = m_command_pool;
            cmd_buf_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmd_buf_alloc_info.commandBufferCount = static_cast<uint32_t>(m_command_buffers.size());

            // Allocate command buffers
            if(vkAllocateCommandBuffers(m_instance_creator.GetDevice(), &cmd_buf_alloc_info, m_command_buffers.data()) != VK_SUCCESS)
                VK_DRAWCMD_ERR("failed to allocate command buffers");
        }


        void Framebuffer::CreateSynchronisationPrimitives() {
            // swapchain image acquisition and presentation semaphores
            if(!m_no_swapchain) {
                m_image_available_semaphores.resize(m_framebuffer_image_ids.size());
                m_render_finished_semaphores.resize(m_framebuffer_image_ids.size());
            }

            m_flight_fences.resize(m_framebuffer_image_ids.size());
            VkSemaphoreCreateInfo semaphore_info = {};
            semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fence_createinfo = {};
            fence_createinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fence_createinfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for(size_t i = 0; i < m_framebuffer_image_ids.size(); i++) {
                const VkDevice device = m_instance_creator.GetDevice();
                if(!m_no_swapchain) {
                    if(vkCreateSemaphore(device, &semaphore_info, nullptr, &m_image_available_semaphores[i]) != VK_SUCCESS) {
                        VK_DRAWCMD_ERR("Failed to create swapchain image acquisition semaphore");
                    }
                    if(vkCreateSemaphore(device, &semaphore_info, nullptr, &m_render_finished_semaphores[i]) != VK_SUCCESS) {
                        VK_DRAWCMD_ERR("Failed to create graphics queue submission semaphore");
                    }
                }
                if(vkCreateFence(m_instance_creator.GetDevice(), &fence_createinfo, NULL, &m_flight_fences[i]) != VK_SUCCESS) 
                    VK_DRAWCMD_ERR("Failed to create frame rendering fence");
            }

            LOG("Successfully created synchronisation primitives for framebuffer");
        }


        void Framebuffer::_CheckAndCreatePipeline(const MeshReference& _ref, uint32_t _mesh_id, const std::vector<ShaderModule*>& _modules) {
            // check if referenced shader module is valid
            if (!_modules[_ref.shader_module_id]) {
                throw std::runtime_error("Invalid shader module id for mesh " + std::to_string(_mesh_id) + ".");
            }

            // check if requested shader module descriptors are not present
            if (_ref.shader_module_id >= static_cast<uint32_t>(m_pipeline_creators.size()) || !m_pipeline_creators[_ref.shader_module_id]) {
                // resize vectors if necessary
                if (_ref.shader_module_id >= static_cast<uint32_t>(m_pipeline_creators.size())) {
                    m_pipeline_creators.resize(_ref.shader_module_id + 1, nullptr);
                    m_shader_descriptor_set_layout_creators.resize(_ref.shader_module_id + 1, nullptr);
                    m_mesh_descriptor_set_layout_creators.resize(_ref.shader_module_id + 1, nullptr);
                    m_shader_desc_allocators.resize(_ref.shader_module_id + 1, nullptr);
                }

                auto& shader_ubo_layouts = _modules[_ref.shader_module_id]->ubo_data_layouts;
                auto& mesh_ubo_layouts = _ref.ubo_data_layouts;
                if (shader_ubo_layouts.size()) {
                    m_shader_descriptor_set_layout_creators[_ref.shader_module_id] = new DescriptorSetLayoutCreator(
                        m_instance_creator.GetDevice(), 
                        _modules[_ref.shader_module_id]->ubo_data_layouts);
                }
                if (mesh_ubo_layouts.size()) {
                    m_mesh_descriptor_set_layout_creators[_ref.shader_module_id] = new DescriptorSetLayoutCreator(
                        m_instance_creator.GetDevice(), 
                        mesh_ubo_layouts);
                }


                std::array<VkDescriptorSetLayout, 2> layouts = { VK_NULL_HANDLE, VK_NULL_HANDLE };
                if (shader_ubo_layouts.size())
                    layouts[0] = m_shader_descriptor_set_layout_creators[_ref.shader_module_id]->GetDescriptorSetLayout();
                if (mesh_ubo_layouts.size())
                    layouts[1] = m_mesh_descriptor_set_layout_creators[_ref.shader_module_id]->GetDescriptorSetLayout();

                m_pipeline_creators[_ref.shader_module_id] = new PipelineCreator(
                    m_instance_creator.GetDevice(),
                    m_renderpass,
                    VkExtent2D{ m_extent.x, m_extent.y },
                    m_sample_count,
                    layouts,
                    *_modules[_ref.shader_module_id]
                );

                // create descriptor sets for shader usage
                if (shader_ubo_layouts.size()) {
                    m_shader_desc_allocators[_ref.shader_module_id] = new DescriptorAllocator(
                        m_instance_creator.GetDevice(),
                        m_uniform,
                        layouts[0],
                        _modules[_ref.shader_module_id]->ubo_data_layouts,
                        m_missing_2d,
                        m_missing_3d,
                        static_cast<uint32_t>(m_framebuffer_image_ids.size()));
                }
            }
        }


        void Framebuffer::_CheckAndCreateMeshDescriptors(const MeshReference& _ref, uint32_t _mesh_id) {
            // check if mesh descriptor sets are not present
            if (_ref.ubo_data_layouts.size() && (_mesh_id >= (uint32_t)m_mesh_desc_allocators.size() || !m_mesh_desc_allocators[_mesh_id])) {
                if (_mesh_id >= (uint32_t)m_mesh_desc_allocators.size())
                    m_mesh_desc_allocators.resize(_mesh_id + 1, nullptr);
                
                // create descriptor sets for mesh
                m_mesh_desc_allocators[_mesh_id] = new DescriptorAllocator(
                    m_instance_creator.GetDevice(),
                    m_uniform,
                    m_mesh_descriptor_set_layout_creators[_ref.shader_module_id]->GetDescriptorSetLayout(),
                    _ref.ubo_data_layouts,
                    m_missing_2d,
                    m_missing_3d,
                    static_cast<uint32_t>(m_framebuffer_image_ids.size()));
            }
        }


        void Framebuffer::StartCommandBufferRecording(TRS::Vector4<float> _clear_color, uint32_t _image_id) {
            if (m_no_swapchain) {
                vkWaitForFences(m_instance_creator.GetDevice(), 1, &m_flight_fences[m_current_frame], VK_TRUE, UINT64_MAX);
                vkResetFences(m_instance_creator.GetDevice(), 1, &m_flight_fences[m_current_frame]);
            }

            vkResetCommandBuffer(m_command_buffers[m_current_frame], 0);
            
            VkCommandBufferBeginInfo cmd_beg_info = {};
            cmd_beg_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cmd_beg_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

            if (vkBeginCommandBuffer(m_command_buffers[m_current_frame], &cmd_beg_info) != VK_SUCCESS)
                VK_DRAWCMD_ERR("Failed to begin command buffer recording.");

            VkRenderPassBeginInfo rp_beg_info = {};
            rp_beg_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rp_beg_info.renderPass = m_renderpass;
            rp_beg_info.framebuffer = m_framebuffers[_image_id];
            rp_beg_info.renderArea.offset = { 0, 0 };
            rp_beg_info.renderArea.extent = { m_extent.x, m_extent.y };

            std::array<VkClearValue, 2> clear_values;
            clear_values[0].color = { {
                _clear_color.first,
                _clear_color.second,
                _clear_color.third,
                _clear_color.fourth
            } };
            clear_values[1].depthStencil = { 1.f, 0 };

            rp_beg_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
            rp_beg_info.pClearValues = clear_values.data();

            vkCmdBeginRenderPass(m_command_buffers[m_current_frame], &rp_beg_info, VK_SUBPASS_CONTENTS_INLINE);
            m_command_buffer_recording_bit = true;
        }


        void Framebuffer::Draw(const MeshReference &_ref, uint32_t _mesh_id, const std::vector<ShaderModule*> &_modules) {
            _CheckAndCreatePipeline(_ref, _mesh_id, _modules);
            _CheckAndCreateMeshDescriptors(_ref, _mesh_id);
            
            // check if custom viewport should be respected
            if (_modules[_ref.shader_module_id]->enable_custom_viewport) {
                VkViewport vp = {};
                vp.x = static_cast<float>(_modules[_ref.shader_module_id]->viewport.x);
                vp.y = static_cast<float>(_modules[_ref.shader_module_id]->viewport.y);
                vp.width = static_cast<float>(_modules[_ref.shader_module_id]->viewport.width);
                vp.height = static_cast<float>(_modules[_ref.shader_module_id]->viewport.height);
                vp.minDepth = 0.f;
                vp.maxDepth = 1.f;
                vkCmdSetViewport(m_command_buffers[m_current_frame], 0, 1, &vp);
            }

            // submit each draw command in mesh
            for (auto cmd_it = _ref.commands.begin(); cmd_it != _ref.commands.end(); cmd_it++) {
                vkCmdBindPipeline(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_creators[_ref.shader_module_id]->GetPipeline());
                std::vector<VkBuffer> buffers(_modules[_ref.shader_module_id]->attributes.size(), m_main_buffer);
                vkCmdBindVertexBuffers(
                    m_command_buffers[m_current_frame],
                    0,
                    static_cast<uint32_t>(buffers.size()),
                    buffers.data(),
                    cmd_it->attribute_offsets.data());

                // check if textures should be bound
                if (cmd_it->texture_ids.size() && (_modules[_ref.shader_module_id]->enable_2d_textures || _modules[_ref.shader_module_id]->enable_3d_textures)) {
                    m_mesh_desc_allocators[_mesh_id]->UpdateDescriptorSets(cmd_it->texture_ids);
                }


                // bind descriptor sets if necessary
                if (_modules[_ref.shader_module_id]->ubo_data_layouts.size() || _ref.ubo_data_layouts.size()) {
                    std::vector<VkDescriptorSet> desc_sets;
                    desc_sets.reserve(2);

                    // per shader descriptor sets
                    if (_modules[_ref.shader_module_id]->ubo_data_layouts.size())
                        desc_sets.push_back(m_shader_desc_allocators[_ref.shader_module_id]->RequestDescriptorSetByFrame(m_current_frame));
                    // per mesh descriptor sets
                    if (_ref.ubo_data_layouts.size())
                        desc_sets.push_back(m_mesh_desc_allocators[_mesh_id]->RequestDescriptorSetByFrame(m_current_frame));
                
                    
                    vkCmdBindDescriptorSets(
                        m_command_buffers[m_current_frame],
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        m_pipeline_creators[_ref.shader_module_id]->GetPipelineLayout(),
                        0,
                        static_cast<uint32_t>(desc_sets.size()),
                        desc_sets.data(),
                        0,
                        nullptr);
                }

                // check if scissor technique should be used
                if (_modules[_ref.shader_module_id]->enable_scissor) {
                    VkRect2D rect = {};
                    rect.offset = VkOffset2D{ cmd_it->scissor.offset.x, cmd_it->scissor.offset.y };
                    rect.extent = VkExtent2D{ cmd_it->scissor.ext.x, cmd_it->scissor.ext.y };
                    vkCmdSetScissor(m_command_buffers[m_current_frame], 0, 1, &rect);
                }

                // check if indexed or unindexed draw call should be submitted
                if (_modules[_ref.shader_module_id]->enable_indexing) {
                    vkCmdBindIndexBuffer(m_command_buffers[m_current_frame], m_main_buffer, static_cast<VkDeviceSize>(cmd_it->indices_offset), VK_INDEX_TYPE_UINT32);
                    vkCmdDrawIndexed(m_command_buffers[m_current_frame], cmd_it->draw_count, 1, 0, 0, 0);
                } else {
                    vkCmdDraw(m_command_buffers[m_current_frame], cmd_it->draw_count, 1, 0, 0);
                }
           }
        }


        void Framebuffer::EndCommandBufferRecording() {
            vkCmdEndRenderPass(m_command_buffers[m_current_frame]);
            if (vkEndCommandBuffer(m_command_buffers[m_current_frame]) != VK_SUCCESS)
                VK_DRAWCMD_ERR("Failed to end command buffer recording.");

            m_command_buffer_recording_bit = false;
        }


        void Framebuffer::Render() {
            VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

            VkSubmitInfo submit_info = {};
            submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            if (!m_no_swapchain) {
                submit_info.waitSemaphoreCount = 1;
                submit_info.pWaitSemaphores = &m_image_available_semaphores[m_current_frame];
                submit_info.pWaitDstStageMask = wait_stages;
                submit_info.signalSemaphoreCount = 1;
                submit_info.pSignalSemaphores = &m_render_finished_semaphores[m_current_frame];
            }
            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = &m_command_buffers[m_current_frame];

            // submit the graphics queue
            if (vkQueueSubmit(m_instance_creator.GetGraphicsQueue(), 1, &submit_info, m_flight_fences[m_current_frame]) != VK_SUCCESS) {
                VK_FRAME_ERR("Failed to submit graphics queue.");
            }
        }


        VkResult Framebuffer::Present(uint32_t _image_id) {
            VkPresentInfoKHR present_info = {};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = &m_render_finished_semaphores[m_current_frame];
            present_info.swapchainCount = 1;
            present_info.pImageIndices = &_image_id;
            present_info.pSwapchains = &m_swapchain_creator->GetSwapchain();

            m_current_frame = (m_current_frame + 1) % static_cast<uint32_t>(m_framebuffer_image_ids.size());
            return vkQueuePresentKHR(m_instance_creator.GetPresentationQueue(), &present_info);
        }


        void Framebuffer::RecreateDescriptorSets() {
            for (auto alloc : m_shader_desc_allocators) {
                if (alloc) alloc->RecreateDescriptorSets(m_uniform, {});
            }

            for (auto alloc : m_mesh_desc_allocators) {
                if (alloc) alloc->RecreateDescriptorSets(m_uniform, {});
            }
        }

        
        void Framebuffer::RecreateFramebuffer() {
            _CreateColorResources();
            _CreateDepthResources();

            if (m_no_swapchain && m_framebuffer_image_ids.empty()) {
                _CreateFramebufferImage();
            }

            if(m_no_swapchain) {
                m_renderpass = Vulkan::SwapchainCreator::CreateRenderPass(m_instance_creator.GetDevice(), VK_FORMAT_DEFAULT_IMAGE, VK_SAMPLE_COUNT_1_BIT);
            } else {
                m_renderpass = Vulkan::SwapchainCreator::CreateRenderPass(m_instance_creator.GetDevice(), VK_FORMAT_DEFAULT_IMAGE, m_sample_count);
            }
            _CreateFramebuffers();
            for(auto pc : m_pipeline_creators) {
                const VkExtent2D ext = { m_extent.x, m_extent.y };

                if (pc) pc->RecreatePipeline(m_renderpass, ext);
            }
        }


        void Framebuffer::DestroyFramebuffer(bool _remove_from_registry) {
            TextureDatabase* db = TextureDatabase::GetInstance();
            
            // destroy color resolve image resources
            Vulkan::TextureData data = std::get<Vulkan::TextureData>(db->GetResource(m_color_resolve_image_id).vendor);
            vkDestroyImageView(m_instance_creator.GetDevice(), data.image_view, nullptr);
            vkDestroyImage(m_instance_creator.GetDevice(), data.image, nullptr);
            vkFreeMemory(m_instance_creator.GetDevice(), data.memory, nullptr);

            // destroy depth image resources
            data = std::get<Vulkan::TextureData>(db->GetResource(m_depth_image_id).vendor);
            vkDestroyImageView(m_instance_creator.GetDevice(), data.image_view, nullptr);
            vkDestroyImage(m_instance_creator.GetDevice(), data.image, nullptr);
            vkFreeMemory(m_instance_creator.GetDevice(), data.memory, nullptr);

            // destroy command pool, framebuffers and pipelines
            vkResetCommandPool(m_instance_creator.GetDevice(), m_command_pool, 0);
            for(auto &fb : m_framebuffers)
                vkDestroyFramebuffer(m_instance_creator.GetDevice(), fb, nullptr);
            for(auto pc : m_pipeline_creators)
                pc->DestroyPipelineData();

            // destroy framebuffer images if possible
            if (m_no_swapchain) {
                for (size_t i = 0; i < m_framebuffer_image_ids.size(); i++) {
                    data = std::get<Vulkan::TextureData>(db->GetResource(m_framebuffer_image_ids[i]).vendor);
                    vkDestroySampler(m_instance_creator.GetDevice(), data.sampler, nullptr);
                    vkDestroyImageView(m_instance_creator.GetDevice(), data.image_view, nullptr);
                    vkDestroyImage(m_instance_creator.GetDevice(), data.image, nullptr);
                    vkFreeMemory(m_instance_creator.GetDevice(), data.memory, nullptr);
                }

                // check if registry removal is required
                if (_remove_from_registry) {
                    db->DeleteResource(m_color_resolve_image_id);
                    db->DeleteResource(m_depth_image_id);

                    for (auto it = m_framebuffer_image_ids.begin(); it != m_framebuffer_image_ids.end(); it++) {
                        db->DeleteResource(*it);
                    }

                    m_color_resolve_image_id = UINT32_MAX;
                    m_depth_image_id = UINT32_MAX;
                    m_framebuffer_image_ids.clear();
                }
            }
        }

        
        void Framebuffer::DestroySynchronisationPrimitives() {
            // destroy semaphores and fences
            for (size_t i = 0; i < m_framebuffer_image_ids.size(); i++) {
                if (!m_no_swapchain) {
                    vkDestroySemaphore(m_instance_creator.GetDevice(), m_render_finished_semaphores[i], nullptr);
                    vkDestroySemaphore(m_instance_creator.GetDevice(), m_image_available_semaphores[i], nullptr);
                }
                vkDestroyFence(m_instance_creator.GetDevice(), m_flight_fences[i], nullptr);
            }

            m_render_finished_semaphores.clear();
            m_image_available_semaphores.clear();
            m_flight_fences.clear();
        }



        void Framebuffer::ClearShaderResources(uint32_t _id) {
            if (_id < (uint32_t)m_pipeline_creators.size()) {
                delete m_pipeline_creators[_id];
                m_pipeline_creators[_id] = nullptr;
                delete m_shader_desc_allocators[_id];
                m_shader_desc_allocators[_id] = nullptr;
                delete m_shader_descriptor_set_layout_creators[_id];
                m_shader_descriptor_set_layout_creators[_id] = nullptr;
                delete m_mesh_descriptor_set_layout_creators[_id];
                m_mesh_descriptor_set_layout_creators[_id] = nullptr;
            }
        }


        void Framebuffer::ClearMeshResources(uint32_t _id) {
            if (_id < (uint32_t)m_mesh_desc_allocators.size()) {
                delete m_mesh_desc_allocators[_id];
                m_mesh_desc_allocators[_id] = nullptr;
            }
        }
    }
}
