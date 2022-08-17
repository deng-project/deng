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
            VkFormat _format, 
            VkSampleCountFlagBits _sample_c, 
            const std::string &_fb_name,
            std::unordered_map<std::string, FramebufferDrawData> &_fb_draws, 
            const std::vector<Vulkan::TextureData> &_fb_images, 
            const std::unordered_map<std::string, Vulkan::TextureData> &_misc_images, 
            bool _no_swapchain
        ) :
            m_instance_creator(_ic),
            m_uniform(_uniform),
            m_main_buffer(_main),
            m_format(_format),
            m_sample_count(_sample_c),
            m_framebuffer_name(_fb_name),
            m_framebuffer_draws(_fb_draws),
            m_framebuffer_images(_fb_images),
            m_misc_textures(_misc_images),
            m_no_swapchain(_no_swapchain),
            m_current_frame(0)
        {
            DENG_ASSERT(m_framebuffer_draws.find(m_framebuffer_name) != m_framebuffer_draws.end());
            if(m_no_swapchain) {
                m_renderpass = Vulkan::SwapchainCreator::CreateRenderPass(m_instance_creator.GetDevice(), m_format, VK_SAMPLE_COUNT_1_BIT, m_no_swapchain);
            } else {
                m_renderpass = Vulkan::SwapchainCreator::CreateRenderPass(m_instance_creator.GetDevice(), m_format, m_sample_count, m_no_swapchain);
            }
            _CreateColorResources();
            _CreateDepthResources();
            _CreateFramebuffers();
            _CreateCommandPool();
            _AllocateCommandBuffers();
            _CreateSynchronisationPrimitives();
        }


        Framebuffer::~Framebuffer() {
            const VkDevice device = m_instance_creator.GetDevice();

            // clean depth image resources
            vkDestroyImageView(device, m_depth_image_view, nullptr);
            vkDestroyImage(device, m_depth_image, nullptr);
            vkFreeMemory(device, m_depth_image_memory, nullptr);

            // clean color image resources
            vkDestroyImageView(device, m_color_resolve_image_view, nullptr);
            vkDestroyImage(device, m_color_resolve_image, nullptr);
            vkFreeMemory(device, m_color_resolve_image_memory, nullptr);

            // destroy framebuffer resources
            for(VkFramebuffer fb : m_framebuffers)
                vkDestroyFramebuffer(device, fb, nullptr);

            // free commandbuffers and -pools
            vkFreeCommandBuffers(device, m_command_pool, static_cast<uint32_t>(m_command_buffers.size()), m_command_buffers.data());
            vkDestroyCommandPool(device, m_command_pool, nullptr);

            _CleanPipelineResources();

            // destroy semaphores and fences
            for(size_t i = 0; i < m_framebuffer_images.size(); i++) {
                if(!m_no_swapchain) {
                    vkDestroySemaphore(device, m_render_finished_semaphores[i], nullptr);
                    vkDestroySemaphore(device, m_image_available_semaphores[i], nullptr);
                }
                vkDestroyFence(device, m_flight_fences[i], nullptr);
            }
        }


        void Framebuffer::_CreateColorResources() {
            const VkDevice device = m_instance_creator.GetDevice();
            const VkPhysicalDevice gpu = m_instance_creator.GetPhysicalDevice();
            const FramebufferDrawData &fb_draw = m_framebuffer_draws.find(m_framebuffer_name)->second;

            VkMemoryRequirements mem_req = Vulkan::_CreateImage(device, m_color_resolve_image, fb_draw.extent.x, fb_draw.extent.y, 1, m_format, 
                                                                VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_sample_count);
            Vulkan::_AllocateMemory(device, gpu, mem_req.size, m_color_resolve_image_memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            vkBindImageMemory(device, m_color_resolve_image, m_color_resolve_image_memory, 0);

            // create image view
            VkImageViewCreateInfo img_view_create_info = Vulkan::_GetImageViewInfo(m_color_resolve_image, m_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);

            if(vkCreateImageView(device, &img_view_create_info, NULL, &m_color_resolve_image_view) != VK_SUCCESS)
                VK_RES_ERR("failed to create color image view");
        }


        void Framebuffer::_CreateDepthResources() {
            const VkDevice device = m_instance_creator.GetDevice();
            const VkPhysicalDevice gpu = m_instance_creator.GetPhysicalDevice();
            const FramebufferDrawData &fb_draw = m_framebuffer_draws.find(m_framebuffer_name)->second;

            VkMemoryRequirements mem_req = Vulkan::_CreateImage(device, m_depth_image, fb_draw.extent.x, fb_draw.extent.y, 1, VK_FORMAT_D32_SFLOAT, 
                                                                VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, m_sample_count);
            Vulkan::_AllocateMemory(device, gpu, mem_req.size, m_depth_image_memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            vkBindImageMemory(device, m_depth_image, m_depth_image_memory, 0);

            // create image view
            VkImageViewCreateInfo img_view_create_info = Vulkan::_GetImageViewInfo(m_depth_image, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

            if(vkCreateImageView(device, &img_view_create_info, NULL, &m_depth_image_view) != VK_SUCCESS)
                VK_RES_ERR("failed to create color image view");
        }


        void Framebuffer::_CreateFramebuffers() {
            const FramebufferDrawData &fb_draw = m_framebuffer_draws.find(m_framebuffer_name)->second;
            m_framebuffers.resize(m_framebuffer_images.size());
            std::array<VkImageView, 2> attachments;

            for(size_t i = 0; i < m_framebuffer_images.size(); i++) {
                attachments = { 
                    m_framebuffer_images[i].image_view,
                    m_depth_image_view
                };

                VkFramebufferCreateInfo framebuffer_createinfo = {};
                framebuffer_createinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebuffer_createinfo.renderPass = m_renderpass;
                framebuffer_createinfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                framebuffer_createinfo.pAttachments = attachments.data();
                framebuffer_createinfo.width = fb_draw.extent.x;
                framebuffer_createinfo.height = fb_draw.extent.y;
                framebuffer_createinfo.layers = 1;

                if(vkCreateFramebuffer(m_instance_creator.GetDevice(), &framebuffer_createinfo, NULL, &m_framebuffers[i]) != VK_SUCCESS) {
                    VK_RES_ERR("failed to create framebuffer!");
                } else {
                    LOG("Framebuffer successfully created");
                }
            }
        }


        void Framebuffer::_CleanPipelineResources() {
            // free all allocated descriptor sets and pools
            m_shader_desc_allocators.clear();
            m_mesh_desc_allocators.clear();

            // destroy pipelines and descriptor set layouts
            m_pipeline_creators.clear();
            m_descriptor_set_layout_creators.clear();
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


        void Framebuffer::_CreateSynchronisationPrimitives() {
            // swapchain image acquisition and presentation semaphores
            if(!m_no_swapchain) {
                m_image_available_semaphores.resize(m_framebuffer_images.size());
                m_render_finished_semaphores.resize(m_framebuffer_images.size());
            }

            m_flight_fences.resize(m_framebuffer_images.size());
            VkSemaphoreCreateInfo semaphore_info = {};
            semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fence_createinfo = {};
            fence_createinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fence_createinfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for(size_t i = 0; i < m_framebuffer_images.size(); i++) {
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

            LOG("Successfully created synchronisation primitives for framebuffer " + m_framebuffer_name);
        }


        void Framebuffer::_RecordCommandBuffers(const TRS::Vector4<float> _clear_color, uint32_t _imgi) {
            const FramebufferDrawData &fb_draw = m_framebuffer_draws.find(m_framebuffer_name)->second;
            // Record each command buffer
            VkCommandBufferBeginInfo cmd_buf_info = {};
            cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cmd_buf_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

            // complete pool transfer whenever possible
            for(auto it = m_shader_desc_allocators.begin(); it != m_shader_desc_allocators.end(); it++)
                it->CompletePoolTransfer();

            // Begin recording command buffer
            if(vkBeginCommandBuffer(m_command_buffers[m_current_frame], &cmd_buf_info) != VK_SUCCESS)
                VK_DRAWCMD_ERR("failed to begin recording command buffers");

            // set up renderpass begin info
            VkRenderPassBeginInfo renderpass_begininfo = {};
            renderpass_begininfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderpass_begininfo.renderPass = m_renderpass;
            renderpass_begininfo.framebuffer = m_framebuffers[_imgi];
            renderpass_begininfo.renderArea.offset = { 0, 0 };
            const VkExtent2D ext = { fb_draw.extent.x, fb_draw.extent.y };
            renderpass_begininfo.renderArea.extent = ext;

            // set up clear values
            std::array<VkClearValue, 2> clear_values;
            clear_values[0].color = {{ 
                _clear_color.first, 
                _clear_color.second,
                _clear_color.third, 
                _clear_color.fourth
            }};
            clear_values[1].depthStencil = { 1.0f, 0 };

            // add clear values to renderpass begin info
            renderpass_begininfo.clearValueCount = static_cast<uint32_t>(clear_values.size());
            renderpass_begininfo.pClearValues = clear_values.data();

            // start a new render pass for recording asset draw commands
            vkCmdBeginRenderPass(m_command_buffers[m_current_frame], &renderpass_begininfo, VK_SUBPASS_CONTENTS_INLINE);

                // Iterate through every mesh, bind resources and issue a draw call to commandbuffer
                for(auto mesh_it = fb_draw.meshes.begin(); mesh_it != fb_draw.meshes.end(); mesh_it++) {
                    const uint32_t shader_id = mesh_it->first.shader_module_id;
                    const uint32_t mesh_id = static_cast<uint32_t>(mesh_it - fb_draw.meshes.begin());
                    const ShaderModule &shader = fb_draw.shaders[shader_id].first;

                    // set the viewport
                    VkViewport vp = {};
                    if(shader.enable_custom_viewport) {
                        vp.x = static_cast<float>(shader.viewport.x);
                        vp.y = static_cast<float>(shader.viewport.height) + static_cast<float>(shader.viewport.y);
                        vp.width = static_cast<float>(shader.viewport.width);
                        vp.height = -static_cast<float>(shader.viewport.height);
                        vp.minDepth = 0.0f;
                        vp.maxDepth = 1.0f;
                        vkCmdSetViewport(m_command_buffers[m_current_frame], 0, 1, &vp);
                    }

                    for(auto cmd_it = mesh_it->first.commands.begin(); cmd_it != mesh_it->first.commands.end(); cmd_it++) {
                        vkCmdBindPipeline(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_creators[shader_id].GetPipeline());
                        VkDeviceSize offset = 0;

                        // bind all attribute bindings
                        DENG_ASSERT(shader.attributes.size() == cmd_it->attribute_offsets.size());
                        DENG_ASSERT(shader.attributes.size() == shader.attribute_strides.size());
                        std::vector<VkBuffer> buffers(shader.attributes.size(), m_main_buffer);
                        vkCmdBindVertexBuffers(m_command_buffers[m_current_frame], 0, static_cast<uint32_t>(shader.attributes.size()), buffers.data(), cmd_it->attribute_offsets.data());

                        // check if descriptor sets should be bound
                        std::vector<VkDescriptorSet> desc_sets;

                        // per shader descriptor sets
                        if(shader.ubo_data_layouts.size()) {
                            if(shader.enable_texture_mapping) {
                                std::vector<std::string> names;
                                std::vector<Vulkan::TextureData> textures;
                                names.reserve(cmd_it->texture_names.size());
                                textures.reserve(cmd_it->texture_names.size());
                                for(auto tex_it = cmd_it->texture_names.begin(); tex_it != cmd_it->texture_names.end(); tex_it++) {
                                    DENG_ASSERT(m_misc_textures.find(*tex_it) != m_misc_textures.end());
                                    names.push_back(*tex_it);
                                    textures.push_back(m_misc_textures.find(*tex_it)->second);
                                }

                                Vulkan::DescriptorAllocator &alloc = m_shader_desc_allocators[m_shader_descriptor_set_index_table[shader_id]];
                                const uint32_t request_size = static_cast<uint32_t>(alloc.GetSamplerCount() * m_misc_textures.size() - alloc.GetBoundTextureCount());
                                desc_sets.push_back(alloc.RequestDescriptorSetByTextures(names, textures, m_current_frame, request_size));
                            } else {
                                desc_sets.push_back(m_shader_desc_allocators[m_shader_descriptor_set_index_table[shader_id]].RequestDescriptorSetByFrame(m_current_frame));
                            }
                        }

                        // per mesh descriptor sets
                        if(mesh_it->first.ubo_blocks.size()) {
                            desc_sets.push_back(m_mesh_desc_allocators[m_mesh_descriptor_set_index_table[mesh_id]].RequestDescriptorSetByFrame(m_current_frame));
                        }

                        if(desc_sets.size())
                            vkCmdBindDescriptorSets(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_creators[shader_id].GetPipelineLayout(), 0, static_cast<uint32_t>(desc_sets.size()), desc_sets.data(), 0, nullptr);

                        // check if scissor technique should be used
                        if(fb_draw.shaders[shader_id].first.enable_scissor) {
                            VkRect2D rect = {};
                            rect.offset = VkOffset2D { cmd_it->scissor.offset.x, cmd_it->scissor.offset.y };
                            rect.extent = VkExtent2D { cmd_it->scissor.ext.x, cmd_it->scissor.ext.y };
                            vkCmdSetScissor(m_command_buffers[m_current_frame], 0, 1, &rect);
                        } 

                        // check if indexed draw is required
                        if(fb_draw.shaders[shader_id].first.enable_indexing) {
                            offset = cmd_it->indices_offset;
                            vkCmdBindIndexBuffer(m_command_buffers[m_current_frame], m_main_buffer, offset, VK_INDEX_TYPE_UINT32);
                            vkCmdDrawIndexed(m_command_buffers[m_current_frame], cmd_it->draw_count, 1, 0, 0, 0);
                        } else {
                            vkCmdDraw(m_command_buffers[m_current_frame], cmd_it->draw_count, 1, 0, 0);
                        }
                    }
                }

            // End render pass
            vkCmdEndRenderPass(m_command_buffers[m_current_frame]);
            
            // Stop recording commandbuffer
            if(vkEndCommandBuffer(m_command_buffers[m_current_frame]) != VK_SUCCESS)
                VK_DRAWCMD_ERR("failed to end recording command buffer");
        }


        void Framebuffer::LoadData() {
            FramebufferDrawData &fb_draw = m_framebuffer_draws.find(m_framebuffer_name)->second;
            m_shader_desc_allocators.reserve(fb_draw.shaders.size());
            m_descriptor_set_layout_creators.reserve(fb_draw.shaders.size());
            m_pipeline_creators.reserve(fb_draw.shaders.size());

            // reserve memory for mesh descriptor set creators 
            m_mesh_desc_allocators.reserve(fb_draw.meshes.size());
            auto &missing = m_misc_textures.find(MISSING_TEXTURE_NAME)->second;
            const uint32_t fb_img_count = static_cast<uint32_t>(m_framebuffer_images.size());

            // for each shader create pipelines
            for(auto it = fb_draw.shaders.begin(); it != fb_draw.shaders.end(); it++) {
                // is new shader?
                if(it->second == RESOURCE_ADDED) {
                    m_descriptor_set_layout_creators.emplace_back(m_instance_creator.GetDevice(), it->first);

                    // construct a temporary array containing all descriptor set layouts [ per shader, per mesh ]
                    std::array<VkDescriptorSetLayout, 2> layouts = { 
                        m_descriptor_set_layout_creators.back().GetPerShaderDescriptorSetLayout(), 
                        m_descriptor_set_layout_creators.back().GetPerMeshDescriptorSetLayout() 
                    };

                    {
                        VkExtent2D ext = { fb_draw.extent.x, fb_draw.extent.y };
                        m_pipeline_creators.emplace_back(m_instance_creator.GetDevice(), m_renderpass, ext, m_sample_count, layouts, it->first);
                    }

                    if(it->first.ubo_data_layouts.size()) {
                        // check if per shader descriptor set layout is present
                        if(layouts[0] != VK_NULL_HANDLE) {
                            const uint32_t pool_cap = static_cast<uint32_t>(m_misc_textures.size()) > 0 ? static_cast<uint32_t>(m_misc_textures.size()) : 1;
                            m_shader_desc_allocators.emplace_back(m_instance_creator.GetDevice(), m_uniform, layouts[0], it->first.ubo_data_layouts, fb_img_count, missing, pool_cap);
                            m_shader_descriptor_set_index_table.push_back(static_cast<uint32_t>(m_shader_desc_allocators.size() - 1));
                        } else {
                            m_shader_descriptor_set_index_table.push_back(UINT32_MAX);
                        }
                    }
                    it->second = RESOURCE_LOADED;
                }
            }

            // for each mesh create descriptor sets if required
            for(auto it = fb_draw.meshes.begin(); it != fb_draw.meshes.end(); it++) {
                // is new mesh?
                if(it->second == RESOURCE_ADDED) {
                    if(it->first.ubo_blocks.size()) {
                        const uint32_t pool_cap = static_cast<uint32_t>(m_misc_textures.size()) > 0 ? static_cast<uint32_t>(m_misc_textures.size()) : 1;

                        const VkDescriptorSetLayout layout = m_descriptor_set_layout_creators[it->first.shader_module_id].GetPerMeshDescriptorSetLayout();
                        m_mesh_desc_allocators.emplace_back(m_instance_creator.GetDevice(), m_uniform, layout, it->first.ubo_blocks, fb_img_count, missing, pool_cap);
                        m_mesh_descriptor_set_index_table.push_back(static_cast<uint32_t>(m_mesh_desc_allocators.size() - 1));
                    } else {
                        m_mesh_descriptor_set_index_table.push_back(UINT32_MAX);
                    }
                    it->second = RESOURCE_LOADED;
                }
            }
        }


        void Framebuffer::ReloadResources() {
            _CleanPipelineResources();
            LoadData();
        }


        void Framebuffer::RecreateDescriptorSets() {
            for(auto &alloc : m_shader_desc_allocators)
                alloc.RecreateDescriptorSets(m_uniform);

            for(auto &alloc : m_mesh_desc_allocators)
                alloc.RecreateDescriptorSets(m_uniform);
        }

        
        void Framebuffer::RecreateFramebuffer() {
            _CreateColorResources();
            _CreateDepthResources();
            const FramebufferDrawData &fb_draw = m_framebuffer_draws.find(m_framebuffer_name)->second;
            if(m_no_swapchain) {
                m_renderpass = Vulkan::SwapchainCreator::CreateRenderPass(m_instance_creator.GetDevice(), m_format, VK_SAMPLE_COUNT_1_BIT);
            } else {
                m_renderpass = Vulkan::SwapchainCreator::CreateRenderPass(m_instance_creator.GetDevice(), m_format, m_sample_count);
            }
            _CreateFramebuffers();
            for(auto &pc : m_pipeline_creators) {
                const VkExtent2D ext = { fb_draw.extent.x, fb_draw.extent.y };
                pc.RecreatePipeline(m_renderpass, ext);
            }
        }


        void Framebuffer::DestroyFramebuffer() {
            vkDestroyImageView(m_instance_creator.GetDevice(), m_color_resolve_image_view, nullptr);
            vkDestroyImage(m_instance_creator.GetDevice(), m_color_resolve_image, nullptr);
            vkFreeMemory(m_instance_creator.GetDevice(), m_color_resolve_image_memory, nullptr);

            vkDestroyImageView(m_instance_creator.GetDevice(), m_depth_image_view, nullptr);
            vkDestroyImage(m_instance_creator.GetDevice(), m_depth_image, nullptr);
            vkFreeMemory(m_instance_creator.GetDevice(), m_depth_image_memory, nullptr);

            vkResetCommandPool(m_instance_creator.GetDevice(), m_command_pool, 0);
            for(auto &fb : m_framebuffers)
                vkDestroyFramebuffer(m_instance_creator.GetDevice(), fb, nullptr);
            for(auto &pc : m_pipeline_creators)
                pc.DestroyPipelineData();
        }


        void Framebuffer::Render(const TRS::Vector4<float> _clear_color, uint32_t _imgi) {
            const VkDevice device = m_instance_creator.GetDevice();
            const VkQueue graphics_queue = m_instance_creator.GetGraphicsQueue();

            vkResetFences(device, 1, &m_flight_fences[m_current_frame]);

            vkResetCommandBuffer(m_command_buffers[m_current_frame], 0);

            if(!m_no_swapchain) _RecordCommandBuffers(_clear_color, _imgi);
            else _RecordCommandBuffers(_clear_color, m_current_frame);

            VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

            VkSubmitInfo submit_info = {};
            submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            if(!m_no_swapchain) {
                submit_info.waitSemaphoreCount = 1;
                submit_info.pWaitSemaphores = &m_image_available_semaphores[m_current_frame];
                submit_info.pWaitDstStageMask = wait_stages;
                submit_info.signalSemaphoreCount = 1;
                submit_info.pSignalSemaphores = &m_render_finished_semaphores[m_current_frame];
            } 
            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = &m_command_buffers[m_current_frame];

            // submit the graphics queue
            VkResult res;
            if((res = vkQueueSubmit(graphics_queue, 1, &submit_info, m_flight_fences[m_current_frame])) != VK_SUCCESS) {
                VK_FRAME_ERR("Failed to submit graphics queue; error code: " + std::to_string(res));
            }

            if(m_no_swapchain) {
                m_current_frame = (m_current_frame + 1) % static_cast<uint32_t>(m_framebuffer_images.size());
            }
        }


        VkResult Framebuffer::Present(VkSwapchainKHR &_swapchain, uint32_t _imgi) {
            VkPresentInfoKHR present_info = {};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = &m_render_finished_semaphores[m_current_frame];
            present_info.swapchainCount = 1;
            present_info.pImageIndices = &_imgi;
            present_info.pSwapchains = &_swapchain;

            m_current_frame = (m_current_frame + 1) % static_cast<uint32_t>(m_framebuffer_images.size());
            return vkQueuePresentKHR(m_instance_creator.GetPresentationQueue(), &present_info);
        }
    }
}
