/*    ____         ________     __      _        ______  
 *   |  _ \_      |  ______|   |  \    | |     _/  ____\
 *   | | \_ \_    |  |         |   \   | |    /  _/   
 *   | |   \  |   |  |_____    | |\ \  | |   |  |   _____ 
 *   | |    | |   |  ______|   | | \ \ | |   |  |  |___  | 
 *   | |  _/ _|   |  |         | |  \ \| |    \  \    / /
 *   | |_/ _/     |  |_____    | |   \   |     \_ \__/ /
 *   |____/       |________|   |_|    \__|       \____/
 *               
 * __________________________________________________________
 * __________________________________________________________
 *                       Project DENG 
 *
 *
 * Copyright (C) 2020 - 2021
 * This Software is licensed under Apache License as described 
 * in the LICENSE.md file, which you should have recieved with 
 * this distribution.
 * 
 * You may reproduce and distribute copies of the
 * Work or Derivative Works thereof in any medium, with or without
 * modifications, and in Source or Object form, provided that You
 * meet the following conditions:
 *
 * (a) You must give any other recipients of the Work or
 *     Derivative Works a copy of this License; and
 *
 * (b) You must cause any modified files to carry prominent notices
 *     stating that You changed the files; and
 *
 * (c) You must retain, in the Source form of any Derivative Works
 *     that You distribute, all copyright, patent, trademark, and
 *     attribution notices from the Source form of the Work,
 *     excluding those notices that do not pertain to any part of
 *     the Derivative Works; and
 *
 * (d) If the Work includes a "NOTICE" text file as part of its
 *     distribution, then any Derivative Works that You distribute must
 *     include a readable copy of the attribution notices contained
 *     within such NOTICE file, excluding those notices that do not
 *     pertain to any part of the Derivative Works, in at least one
 *     of the following places: within a NOTICE text file distributed
 *     as part of the Derivative Works; within the Source form or
 *     documentation, if provided along with the Derivative Works; or,
 *     within a display generated by the Derivative Works, if and
 *     wherever such third-party notices normally appear. The contents
 *     of the NOTICE file are for informational purposes only and
 *     do not modify the License. You may add Your own attribution
 *     notices within Derivative Works that You distribute, alongside
 *     or as an addendum to the NOTICE text from the Work, provided
 *     that such additional attribution notices cannot be construed
 *     as modifying the License.
 *
 * You may add Your own copyright statement to Your modifications and
 * may provide additional or different license terms and conditions
 * for use, reproduction, or distribution of Your modifications, or
 * for any such Derivative Works as a whole, provided Your use,
 * reproduction, and distribution of the Work otherwise complies with
 * the conditions stated in this License.
 */ 


#define __VULKAN_RENDERER_CPP
#include <deng/vulkan/vulkan_renderer.h>


namespace deng {
    namespace vulkan {
        extern deng_ui32_t __max_frame_c;

        /**************************************************/
        /**************************************************/
        /**************************************************/
        /*********** __vk_RuntimeUpdater Class ************/
        /**************************************************/
        /**************************************************/
        __vk_RuntimeUpdater::__vk_RuntimeUpdater(const __vk_AssetsInfo &asset_info) {
            m_p_assets = asset_info.p_assets;
            m_p_textures = asset_info.p_tex;

            m_p_asset_map = asset_info.p_asset_map;
            m_p_tex_map = asset_info.p_tex_map;
        }


        /*
         * This method updates the vertices buffer that is allocated by
         * given assets
         */
        void __vk_RuntimeUpdater::__updateAssetVerts(const dengMath::vec2<deng_ui32_t> &asset_bounds) {
            m_p_rm->remapAssetVerts (
                m_p_ic->getDev(),
                m_p_ic->getGpu(),
                m_p_dc->getComPool(),
                m_p_ic->getQFF().graphics_queue,
                asset_bounds
            );
        }

        
        /*
         * Free and reallocate new commandbuffers 
         */
        void __vk_RuntimeUpdater::__updateCmdBuffers(const dengMath::vec4<deng_vec_t> &background) {
            m_p_dc->recordMainCmdBuffers (
                m_p_scc->getRp(),
                m_p_scc->getExt(),
                background,
                m_p_rm->getBD()
            );
        }


        /*
         * Reallocate texture buffer 
         */
        void __vk_RuntimeUpdater::__realignTextureBuffers() {
            m_p_rm->getBD()->img_memory_offset = 0;

            // Free all textue images, imageviews and samplers
            for(size_t i = 0; i < m_p_textures->size(); i++) {
                vkDestroyImageView (
                    m_p_ic->getDev(),
                    m_p_textures->at(i).image_view,
                    NULL
                );

                vkDestroyImage (
                    m_p_ic->getDev(),
                    m_p_textures->at(i).image,
                    NULL
                );
            }
        }

        
        /*
         * Free all buffers related to asset data and reallocate them
         */
        void __realignAssetBuffers();

        
        /********************************************/
        /********************************************/
        /********************************************/
        /*********** __vk_Renderer Class ************/
        /********************************************/
        /********************************************/

        __vk_Renderer::__vk_Renderer (
            deng::Window *p_win,
            const __vk_ConfigVars &cnf
        ) : __vk_RuntimeUpdater (
            {&m_assets, &m_textures, &m_asset_map, &m_tex_map}
        ) {
            m_p_win = p_win;
            m_config = cnf;

            // Create new hashmap instances for assets and textures
            newHashmap(&m_asset_map, __DEFAULT_ASSET_HM_CAP);
            newHashmap(&m_tex_map, __DEFAULT_TEX_HM_CAP);

            __initConstruct();
        }


        __vk_Renderer::~__vk_Renderer() {
            idle();
            __cleanup();
            
            delete m_p_desc_c;
            delete m_p_dc;
            delete m_p_rm;
            delete m_p_scc;
            delete m_p_ic;
        }


        /*
         * Create new renderer creator instances
         */
        void __vk_Renderer::__initConstruct() {
            // Create new VkInstace
            m_p_ic = new __vk_InstanceCreator (
                m_p_win,
                m_config.enable_validation_layers
            );
            
            // Create new swapchain 
            m_p_scc = new __vk_SwapChainCreator (
                m_p_ic->getDev(),
                m_p_win,
                m_p_ic->getGpu(), 
                m_p_ic->getSu(), 
                m_p_ic->getQFF(),
                m_config.msaa_sample_count
            );

            // Create new draw caller instance and make command pool
            m_p_dc = new __vk_DrawCaller (
                m_p_ic->getDev(),
                m_p_ic->getQFF()
            );
            
            m_p_dc->mkCommandPool(m_p_ic->getDev());

            __vk_AssetsInfo asset_info;
            asset_info.p_asset_map = &m_asset_map;
            asset_info.p_tex_map = &m_tex_map;
            asset_info.p_assets = &m_assets;
            asset_info.p_tex = &m_textures;
            __max_frame_c = m_p_scc->getSCImg().size();

            // Create new buffer resources allocator
            m_p_rm = new __vk_ResourceManager (
                m_p_ic->getDev(), 
                m_p_ic->getGpu(), 
                m_p_scc->getExt(), 
                m_config.msaa_sample_count,
                m_p_scc->getRp(), 
                m_p_dc->getComPool(),
                m_p_ic->getQFF().graphics_queue,
                m_p_scc->getSCImgViews(),
                asset_info,
                m_p_scc->getSF(),
                m_p_ic->getGpuLimits()
            );

            // Create new vulkan descriptor creator
            m_p_desc_c = new __vk_DescriptorCreator (
                m_p_ic->getDev(),
                m_p_scc->getExt(),
                m_p_scc->getRp(),
                asset_info,
                m_config.msaa_sample_count
            );
        }


        /*
         * Free and destroy all active vulkan instances
         */
        void __vk_Renderer::__cleanup() {
            // Clean depth image resources
            vkDestroyImageView (
                m_p_ic->getDev(), 
                m_p_rm->getDepImgView(), 
                nullptr
            );
            vkDestroyImage (
                m_p_ic->getDev(), 
                m_p_rm->getDepImg(), 
                nullptr
            );
            vkFreeMemory (
                m_p_ic->getDev(), 
                m_p_rm->getDepImgMem(), 
                nullptr
            );

            // Clean color image resources
            vkDestroyImageView (
                m_p_ic->getDev(),
                m_p_rm->getColorImgView(),
                nullptr
            );
            vkDestroyImage (
                m_p_ic->getDev(),
                m_p_rm->getColorImg(),
                nullptr
            );
            vkFreeMemory (
                m_p_ic->getDev(),
                m_p_rm->getColorImgMem(),
                nullptr
            );

            // Clean framebuffers
            std::vector<VkFramebuffer> fb;
            fb = m_p_rm->getFB();
            for(size_t i = 0; i < fb.size(); i++) {
                vkDestroyFramebuffer (
                    m_p_ic->getDev(), 
                    fb[i], 
                    nullptr
                );
            }

            // Clean commandbuffers and commandpools
            vkFreeCommandBuffers (
                m_p_ic->getDev(), 
                m_p_dc->getComPool(), 
                m_p_dc->getComBufs().size(), 
                m_p_dc->getComBufs().data()
            );

            // Clean pipeline related data
            std::array<__vk_PipelineData, __DENG_PIPELINE_COUNT> pd = m_p_desc_c->getPipelines();
            for(size_t i = 0; i < pd.size(); i++) { 
                vkDestroyPipeline (
                    m_p_ic->getDev(), 
                    pd[i].pipeline, 
                    nullptr
                );
            }

            // Clean pipeline layout data 
            vkDestroyPipelineLayout (
                m_p_ic->getDev(), 
                m_p_desc_c->getUnmappedPL(), 
                nullptr
            );
            
            vkDestroyPipelineLayout (
                m_p_ic->getDev(), 
                m_p_desc_c->getTexMappedPL(), 
                nullptr
            );

            // Destroy renderpass
            vkDestroyRenderPass (
                m_p_ic->getDev(), 
                m_p_scc->getRp(), 
                nullptr
            );

            // Destroy swapchain
            m_p_scc->SCCleanup();

            // Clean ubo buffer data 
            vkDestroyBuffer (
                m_p_ic->getDev(), 
                m_p_rm->getBD()->uniform_buffer, 
                nullptr
            );

            vkFreeMemory (
                m_p_ic->getDev(), 
                m_p_rm->getBD()->uniform_buffer_mem, 
                nullptr
            );

            // Destroy descriptor pools 
            vkDestroyDescriptorPool (
                m_p_ic->getDev(), 
                m_p_desc_c->getUnmappedDP(), 
                nullptr
            );

            vkDestroyDescriptorPool (
                m_p_ic->getDev(), 
                m_p_desc_c->getTexMappedDP(), 
                nullptr
            );

            // Clean texture images
            __cleanAssets();

            // Clean descriptor set layouts
            vkDestroyDescriptorSetLayout (
                m_p_ic->getDev(), 
                m_p_desc_c->getUnmappedDSL(), 
                nullptr
            );

            vkDestroyDescriptorSetLayout (
                m_p_ic->getDev(), 
                m_p_desc_c->getTexMappedDSL(), 
                nullptr
            );

            // Clean main buffer data
            vkDestroyBuffer (
                m_p_ic->getDev(), 
                m_p_rm->getBD()->main_buffer, 
                nullptr
            );

            vkFreeMemory (
                m_p_ic->getDev(), 
                m_p_rm->getBD()->main_buffer_memory, 
                nullptr
            );


            // Clean semaphores and fences
            for(size_t i = 0; i < __max_frame_c; i++) {
                vkDestroySemaphore (
                    m_p_ic->getDev(), 
                    m_p_dc->image_available_semaphore_set[i], 
                    nullptr
                );
                vkDestroySemaphore (
                    m_p_ic->getDev(), 
                    m_p_dc->render_finished_semaphore_set[i], 
                    nullptr
                );
                vkDestroyFence (
                    m_p_ic->getDev(), 
                    m_p_dc->flight_fences[i], 
                    nullptr
                );
            }

            vkDestroyCommandPool (
                m_p_ic->getDev(), 
                m_p_dc->getComPool(), 
                nullptr
            );

            // Clean instance and devices
            vkDestroyDevice (
                m_p_ic->getDev(), 
                nullptr
            );

            if(m_config.enable_validation_layers) {
                __vk_InstanceCreator::destroyDebugUtils (
                    m_p_ic->getIns(), 
                    m_p_ic->getDMEXT()
                );
            }

            vkDestroySurfaceKHR (
                m_p_ic->getIns(), 
                m_p_ic->getSu(), 
                nullptr
            );

            
            vkDestroyInstance (
                m_p_ic->getIns(), 
                nullptr
            );
        }


        /*
         * Free asset and texture memory
         */
        void __vk_Renderer::__cleanAssets() {
            // Destroy all descriptor sets and free all vertices' and indices' memory
            for(size_t i = 0; i < m_assets.size(); i++) {
                vkFreeDescriptorSets (
                    m_p_ic->getDev(), 
                    m_p_desc_c->getTexMappedDP(),
                    (deng_ui32_t) m_assets[i].desc_sets.size(),
                    m_assets[i].desc_sets.data()
                );

                // Free vertices and indices memory
                switch(m_assets[i].asset.asset_mode) 
                {
                case DAS_ASSET_MODE_2D_UNMAPPED:
                    free(m_assets[i].asset.vertices.vu2d);
                    break;

                case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                    free(m_assets[i].asset.vertices.vm2d);
                    break;

                case DAS_ASSET_MODE_3D_UNMAPPED:
                    free(m_assets[i].asset.vertices.vuu);
                    break;

                case DAS_ASSET_MODE_3D_UNMAPPED_NORMALISED:
                    free(m_assets[i].asset.vertices.vun);
                    break;

                case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                    free(m_assets[i].asset.vertices.vmu);
                    break;

                case DAS_ASSET_MODE_3D_TEXTURE_MAPPED_NORMALISED:
                    free(m_assets[i].asset.vertices.vmn);
                    break;

                default:
                    break;
                }

                free(m_assets[i].asset.indices.indices);
                free(m_assets[i].asset.uuid);
            }

            // Destroy all VkImage instances for textures
            for(size_t i = 0; i < m_textures.size(); i++) {
                vkDestroySampler (
                    m_p_ic->getDev(),
                    m_textures[i].sampler,
                    NULL
                );

                vkDestroyImageView (
                    m_p_ic->getDev(),
                    m_textures[i].image_view,
                    NULL
                );

                vkDestroyImage (
                    m_p_ic->getDev(),
                    m_textures[i].image,
                    NULL
                );
            }

            // Free texture images memory
            vkFreeMemory (
                m_p_ic->getDev(),
                m_p_rm->getBD()->img_memory,
                NULL
            );
        }


        /*
         * Add new assets to renderer asset list.
         * PS! UUIDs must be previously generated!
         * Also no descriptor sets are allocated since that
         * will be done when renderer is being fully initialised.
         */
        void __vk_Renderer::submitAssets (
            das_Asset *assets,
            deng_i32_t asset_c
        ) {
            size_t old_size = m_assets.size();
            m_assets.resize(m_assets.size() + asset_c);

            LOG("Attempting to allocate " + std::to_string(__max_frame_c * sizeof(__vk_UniformColorData)) + " bytes of memory");
            __vk_UniformColorData *ubo_mem = (__vk_UniformColorData*) malloc(__max_frame_c * sizeof(__vk_UniformColorData));

            // Create new assets with their descriptor sets
            for(deng_i32_t i = 0; i < asset_c; i++) { 
                m_assets[old_size + i].asset = assets[i];
                pushToHashmap (
                    &m_asset_map,
                    assets[i].uuid,
                    strlen(assets[i].uuid),
                    &m_assets[old_size + i]
                );

                // Allocate uniform buffer memory for uniform color data
                m_p_rm->reserveAssetUniformData (
                    m_p_ic->getDev(), 
                    m_p_ic->getGpu(),
                    m_p_dc->getComPool(),
                    m_p_ic->getQFF().graphics_queue,
                    ubo_mem,
                    m_assets[old_size + i]
                );
            }

            free(ubo_mem);
        }

        
        /*
         * Add new textures to renderer texture list.
         * PS! UUIDs must be previously generated!
         */
        void __vk_Renderer::submitTextures (
            das_Texture *textures,
            deng_i32_t tex_c
        ) {
            size_t old_size = m_textures.size();
            m_textures.resize(m_textures.size() + tex_c);

            for(size_t i = old_size; i < m_textures.size(); i++) {
                m_textures[i].texture = textures[i - old_size];
                pushToHashmap (
                    &m_tex_map,
                    m_textures[i].texture.uuid,
                    strlen(m_textures[i].texture.uuid),
                    &m_textures[i]
                );

                LOG("Pushing texture with name and uuid: " + std::string(m_textures[i].texture.name) + ", " + std::string(m_textures[i].texture.uuid));
            }

            // Create vulkan image resources
            m_p_rm->mkTextures (
                m_p_ic->getDev(), 
                m_p_ic->getGpu(),
                m_p_dc->getComPool(),
                m_p_ic->getLFSupport(),
                {(deng_ui32_t) old_size, (deng_ui32_t) m_textures.size()},
                m_p_ic->getQFF().graphics_queue
            );
        }


        /*
         * Submit new draw calls and update uniform data
         */
        void __vk_Renderer::makeFrame() {
            // Call Vulkan fence waiter function
            vkWaitForFences (
                m_p_ic->getDev(), 
                1, 
                &m_p_dc->flight_fences[m_p_dc->current_frame], 
                VK_TRUE, 
                UINT64_MAX
            );

            // Call Vulkan next image acquire method
            deng_ui32_t image_index;
            VkResult result = vkAcquireNextImageKHR (
                m_p_ic->getDev(), 
                m_p_scc->getSC(), 
                UINT64_MAX, 
                m_p_dc->image_available_semaphore_set[m_p_dc->current_frame], 
                VK_NULL_HANDLE, 
                &image_index
            );
            
            if(result == VK_ERROR_OUT_OF_DATE_KHR) {
                VK_FRAME_ERR("image acquiring timed out");
                return;
            }
            
            else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
                VK_FRAME_ERR("failed to acquire swap chain image");

            // Update uniform data buffer
            m_p_rm->updateUniformData (
                m_p_ic->getDev(), 
                m_p_dc->current_frame, 
                __max_frame_c,
                m_config.p_camera, 
                false,
                {0, 0},
                m_config.cam_type,
                DENG_CAMERA_UNIFORM_NO_CAMERA_MODE_2D | 
                DENG_CAMERA_UNIFORM_PERSPECTIVE_CAMERA_MODE_3D
            );

            VkSemaphore wait_semaphores[] = 
            {m_p_dc->image_available_semaphore_set[m_p_dc->current_frame]};
            VkSemaphore signal_semaphores[] = 
            {m_p_dc->render_finished_semaphore_set[m_p_dc->current_frame]};

            VkSubmitInfo submitinfo{};
            submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            submitinfo.waitSemaphoreCount = 1;
            submitinfo.pWaitSemaphores = wait_semaphores;
            submitinfo.pWaitDstStageMask = wait_stages;
            submitinfo.commandBufferCount = 1;
            submitinfo.pCommandBuffers = &m_p_dc->getComBufs()[image_index];
            submitinfo.signalSemaphoreCount = 1;
            submitinfo.pSignalSemaphores = signal_semaphores;

            vkResetFences (
                m_p_ic->getDev(), 
                1, 
                &m_p_dc->flight_fences[m_p_dc->current_frame]
            );

            if
            (
                vkQueueSubmit (
                    m_p_ic->getQFF().graphics_queue, 
                    1, 
                    &submitinfo, 
                    m_p_dc->flight_fences[m_p_dc->current_frame]
                ) != VK_SUCCESS
            ) VK_FRAME_ERR("failed to submit draw command"); 

            VkPresentInfoKHR present_info{};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = signal_semaphores;
            present_info.pImageIndices = &image_index;
            
            VkSwapchainKHR swapchains[] = {m_p_scc->getSC()};
            present_info.swapchainCount = 1;
            present_info.pSwapchains = swapchains;

            vkQueuePresentKHR (
                m_p_ic->getQFF().present_queue, 
                &present_info
            );

            m_p_dc->current_frame = (m_p_dc->current_frame + 1) % __max_frame_c;
        }


        /*
         * Setup the renderer, create descriptor sets, allocate buffers
         * and record command buffers
         */
        void __vk_Renderer::setup() {
            // Make and allocate memory for main buffer 
            m_p_rm->mkBuffers (
                m_p_ic->getDev(),
                m_p_ic->getGpu(),
                m_p_dc->getComPool(),
                m_p_ic->getQFF().graphics_queue
            );

            // Create descriptor sets for all currently available assets
            LOG("Test uuid: " + std::string(m_p_rm->getDummyTexUUID()));
            m_p_desc_c->mkDS (
                m_p_ic->getDev(), 
                m_p_rm->getBD(),
                m_p_rm->getDummyTexUUID(),
                { 0, (deng_ui32_t) m_assets.size() },
                m_p_ic->getGpuLimits().minUniformBufferOffsetAlignment
            );

            // Start recording command buffers
            m_p_dc->setAssetsData (
                &m_assets, 
                &m_textures
            );

            m_p_dc->setMiscData (
                m_p_desc_c->getPipelines(),
                m_p_rm->getFB()
            );
            
            m_p_dc->allocateMainCmdBuffers (
                m_p_ic->getDev(), 
                m_p_ic->getQFF().graphics_queue, 
                m_p_scc->getRp(),
                m_p_scc->getExt(),
                m_config.background,
                m_p_rm->getBD()
            );
        }


        /*
         * Wait for any queue operation to finish
         * This method needs to be called whenever any 
         * command or data buffers need to be updated
         */
        void __vk_Renderer::idle() {
            vkDeviceWaitIdle(m_p_ic->getDev());
        }
    }
}
