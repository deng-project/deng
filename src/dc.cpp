/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: dc.h - Vulkan draw call wrapper class implementation
/// author: Karl-Mihkel Ott

#define __VK_DC_CPP
#include <deng/vulkan/dc.h>

namespace deng {
    namespace vulkan {

        __vk_DrawCaller::__vk_DrawCaller (
            VkDevice device,
            const __vk_QueueManager &qff,
            const std::vector<deng_Id> &assets,
            const std::vector<deng_Id> &textures,
            const std::vector<VkDescriptorSet> &ui_sets,
            deng::Registry &reg
        ) : m_assets(assets), m_textures(textures), m_ui_sets(ui_sets), m_reg(reg), m_qff(qff)
        {
            __mkSynchronisation(device);
        }


        /// Create command pool
        void __vk_DrawCaller::mkCommandPool(VkDevice device) {
            VkCommandPoolCreateInfo cmd_pool_createinfo = {};
            cmd_pool_createinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            cmd_pool_createinfo.queueFamilyIndex = m_qff.getGraphicsQFIndex();
            cmd_pool_createinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

            // Create cmd_pool
            if(vkCreateCommandPool(device, &cmd_pool_createinfo, NULL, &m_cmd_pool) != VK_SUCCESS)
                VK_DRAWCMD_ERR("failed to create command pool");
        }


        /// Create semaphores and fences for synchronising frames 
        void __vk_DrawCaller::__mkSynchronisation(VkDevice &device) {
            // Resize semaphores 
            image_available_semaphore_set.resize(__max_frame_c);
            render_finished_semaphore_set.resize(__max_frame_c);
            flight_fences.resize(__max_frame_c);
            VkSemaphoreCreateInfo semaphore_info = {};
            semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fence_createInfo = {};
            fence_createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fence_createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for(size_t i = 0; i < __max_frame_c; i++) {
                if(vkCreateSemaphore(device, &semaphore_info, NULL, 
                   &image_available_semaphore_set[i]) != VK_SUCCESS ||
                   vkCreateSemaphore(device, &semaphore_info, NULL, 
                   &render_finished_semaphore_set[i]) != VK_SUCCESS ||
                   vkCreateFence(device, &fence_createInfo, NULL, 
                   &flight_fences[i]) != VK_SUCCESS) 
                    VK_DRAWCMD_ERR("Failed to create semaphores");
                else LOG("Successfully created semaphores");
            }
        }

        
        /// Bind asset resources to command buffers
        void __vk_DrawCaller::__bindAssetResources (
            das_Asset &asset, 
            VkCommandBuffer cur_buf,
            const __vk_BufferData &bd,
            const deng::BufferSectionInfo &buf_sec
        ) {
            // Vertex normal binding number variable, since it can change according to the asset mode
            deng_ui32_t nor_bind_nr = 1;

            // Bind the position vertex location in buffer
            vkCmdBindVertexBuffers(cur_buf, 0, 1, &bd.main_buffer, &asset.offsets.pos_offset);


            // Check if texture vertices should be bound
            if(asset.asset_mode == DAS_ASSET_MODE_2D_TEXTURE_MAPPED ||
               asset.asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED) {
                // Bind texture vertex location in buffer
                vkCmdBindVertexBuffers(cur_buf, 1, 1, &bd.main_buffer, &asset.offsets.tex_offset);

                nor_bind_nr = 2;
            }

            // Check if vertex normals should be bound
            if(asset.asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED ||
               asset.asset_mode == DAS_ASSET_MODE_3D_UNMAPPED) {
                // Bind vertex normals location in buffer
                vkCmdBindVertexBuffers(cur_buf, nor_bind_nr, 1, &bd.main_buffer, &asset.offsets.nor_offset);
            }

            // Bind the position indices location in buffer
            vkCmdBindIndexBuffer(cur_buf, bd.main_buffer, asset.offsets.ind_offset, VK_INDEX_TYPE_UINT32);
        }


        /// Bind ImGui resources to command buffers
        void __vk_DrawCaller::__bindUIElementResources (
            __ImGuiEntity *ent, 
            VkCommandBuffer cur_buf, 
            const __vk_BufferData &bd,
            const deng::BufferSectionInfo &buf_sec
        ) {
            VkDeviceSize vert_offset = buf_sec.asset_cap + m_p_ui_data->cmd_data[ent->cmd_list_ind].voffset;
            vkCmdBindVertexBuffers(cur_buf, 0, 1, &bd.main_buffer, &vert_offset);
            vkCmdBindIndexBuffer(cur_buf, bd.main_buffer, buf_sec.asset_cap + ent->buf_offset, VK_INDEX_TYPE_UINT32);
        }


        /// Bind asset pipeline and return its pipeline layout
        VkPipelineLayout *__vk_DrawCaller::__bindAssetPipeline(das_Asset &asset, VkCommandBuffer cmd_buf) {
            VkPipeline *p_pl = NULL;
            VkPipelineLayout *p_pl_layout = NULL;
            // Find correct pipelines and their layouts according to the asset mode
            switch(asset.asset_mode) {
            case DAS_ASSET_MODE_2D_UNMAPPED:
                p_pl = &m_p_pl_data->at(UM2D_I).pipeline;
                p_pl_layout = m_p_pl_data->at(UM2D_I).p_pipeline_layout;
                break;

            case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                p_pl = &m_p_pl_data->at(TM2D_I).pipeline;
                p_pl_layout = m_p_pl_data->at(TM2D_I).p_pipeline_layout;
                break;

            case DAS_ASSET_MODE_3D_UNMAPPED:
                p_pl = &m_p_pl_data->at(UM3D_I).pipeline;
                p_pl_layout = m_p_pl_data->at(UM3D_I).p_pipeline_layout;
                break;

            case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                p_pl = &m_p_pl_data->at(TM3D_I).pipeline;
                p_pl_layout = m_p_pl_data->at(TM3D_I).p_pipeline_layout;
                break;

            default:
                UNDEFINED_ASSET_MODE(asset.src, asset.uuid);
                break;
            }

            vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, *p_pl);

            // Pipeline layout is necessary for binding descriptor sets
            // That is why it is returned with this method call
            return p_pl_layout;
        }


        /// Set miscellanious data arrays 
        void __vk_DrawCaller::setMiscData (
            std::array<__vk_PipelineData, PIPELINE_C> *pl_data, 
            std::vector<VkFramebuffer> *fb
        ) {
            m_p_pl_data = pl_data;
            m_p_framebuffers = fb;
        }


        /// Allocate memory for commandbuffers
        void __vk_DrawCaller::allocateCmdBuffers (
            VkDevice device, 
            const dengMath::vec4<deng_vec_t> &background,
            const __vk_BufferData &bd,
            const deng::BufferSectionInfo &buf_sec
        ) {
            m_cmd_bufs.resize(m_p_framebuffers->size());

            // Set up commandbuffer allocate info
            VkCommandBufferAllocateInfo cmd_buf_alloc_info = {};
            cmd_buf_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmd_buf_alloc_info.commandPool = m_cmd_pool;
            cmd_buf_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmd_buf_alloc_info.commandBufferCount = static_cast<deng_ui32_t>(m_cmd_bufs.size());

            // Allocate command buffers
            if(vkAllocateCommandBuffers(device, &cmd_buf_alloc_info, m_cmd_bufs.data())) 
                VK_DRAWCMD_ERR("failed to allocate command buffers");
        }


        /// Record command buffers for drawing assets and optionally ui elements
        void __vk_DrawCaller::recordCmdBuffers (
            VkRenderPass renderpass,
            VkExtent2D ext,
            const dengMath::vec4<deng_vec_t> &background,
            const __vk_BufferData &bd,
            const deng::BufferSectionInfo &buf_sec
        ) {
            // Record each command buffer
            for(size_t i = 0; i < m_cmd_bufs.size(); i++) {
                VkCommandBufferBeginInfo cmd_buf_info = {};
                cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                cmd_buf_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

                // Begin recording command buffer
                if(vkBeginCommandBuffer(m_cmd_bufs[i], &cmd_buf_info) != VK_SUCCESS)
                    VK_DRAWCMD_ERR("failed to begin recording command buffers");

                // Set up renderpass begin info
                VkRenderPassBeginInfo renderpass_begininfo = {};
                renderpass_begininfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderpass_begininfo.renderPass = renderpass;
                renderpass_begininfo.framebuffer = m_p_framebuffers->at(i);
                renderpass_begininfo.renderArea.offset = { 0, 0 };
                renderpass_begininfo.renderArea.extent = ext;

                // Set up clear values
                std::array<VkClearValue, 2> clear_values;
                clear_values[0].color = {{
                    background.first,
                    background.second,
                    background.third,
                    background.fourth
                }};
                clear_values[1].depthStencil = { 1.0f, 0 };

                // Add clear values to renderpass begin info
                renderpass_begininfo.clearValueCount = static_cast<deng_ui32_t>(clear_values.size());
                renderpass_begininfo.pClearValues = clear_values.data();

                // Start a new render pass for recording asset draw commands
                vkCmdBeginRenderPass(m_cmd_bufs[i], &renderpass_begininfo, VK_SUBPASS_CONTENTS_INLINE);

                    // Iterate through every asset, bind resources and issue an index draw to commandbuffer
                    for(size_t j = 0; j < m_assets.size(); j++) {
                        RegData reg_asset = m_reg.retrieve(m_assets[j], DENG_REGISTRY_TYPE_ASSET, NULL);
                        RegData reg_vk_asset = m_reg.retrieve(reg_asset.asset.vk_id, DENG_REGISTRY_TYPE_VK_ASSET, NULL);

                        if(reg_asset.asset.is_shown) {
                            __bindAssetResources(reg_asset.asset, m_cmd_bufs[i], bd, buf_sec);
                            VkPipelineLayout *p_pl_layout = __bindAssetPipeline(reg_asset.asset, m_cmd_bufs[i]);

                            vkCmdBindDescriptorSets(m_cmd_bufs[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                *p_pl_layout, 0, 1, &reg_vk_asset.vk_asset.desc_sets[i], 0, NULL);

                            vkCmdDrawIndexed(m_cmd_bufs[i], static_cast<deng_ui32_t>(reg_asset.asset.indices.n), 
                                1, 0, 0, 0);
                        }
                    }

                    // Check if ui elements should be drawn
                    if(m_p_ui_data) {
                        for(deng_i64_t j = 0; j < m_p_ui_data->entities.size(); j++) {
                            __bindUIElementResources(&m_p_ui_data->entities[j], m_cmd_bufs[i], bd, buf_sec);

                            vkCmdBindPipeline(m_cmd_bufs[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_p_pl_data->at(UI_I).pipeline);

                            vkCmdBindDescriptorSets(m_cmd_bufs[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                *m_p_pl_data->at(UI_I).p_pipeline_layout, 0, 1, &m_ui_sets[i], 0, NULL);

                            // Convert scissor coordinates to VkRect2D structure
                            const VkRect2D sc_rect = VkRect2D {
                                VkOffset2D { m_p_ui_data->entities.at(j).sc_rec_offset.first, m_p_ui_data->entities.at(j).sc_rec_offset.second },
                                VkExtent2D { m_p_ui_data->entities.at(j).sc_rec_size.first, m_p_ui_data->entities.at(j).sc_rec_size.second }
                            };

                            vkCmdSetScissor(m_cmd_bufs[i], 0, 1, &sc_rect);
                            vkCmdDrawIndexed(m_cmd_bufs[i], m_p_ui_data->entities[j].ind_c, 1, 0, 0, 0);
                        }
                    }

                // End render pass
                vkCmdEndRenderPass(m_cmd_bufs[i]);
                
                // Stop recording commandbuffer
                if(vkEndCommandBuffer(m_cmd_bufs[i]) != VK_SUCCESS)
					VK_DRAWCMD_ERR("failed to end recording command buffer");
            }
        }


        /// Getter and setter methods
        void __vk_DrawCaller::setUIDataPtr(__ImGuiData *p_gui) { m_p_ui_data = p_gui; }
        VkCommandPool __vk_DrawCaller::getComPool() { return m_cmd_pool; }
        const std::vector<VkCommandBuffer> &__vk_DrawCaller::getComBufs() { 
            return m_cmd_bufs;
        }
    }   
}
