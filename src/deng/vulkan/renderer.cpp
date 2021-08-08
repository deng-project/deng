/// DENG: dynamic engine - powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: renderer.cpp - Vulkan renderer class implementation
/// author: Karl-Mihkel Ott

#define __VK_RENDERER_CPP
#include <deng/vulkan/renderer.h>


namespace deng {
    namespace vulkan {
        
        /********************************************/
        /*********** __vk_Renderer Class ************/
        /********************************************/

        __vk_Renderer::__vk_Renderer (
            __vk_ConfigVars &cnf,
            deng::__GlobalRegistry &reg,
            std::vector<deng_Id> &assets, 
            std::vector<deng_Id> &textures
        ) : __vk_RendererInitialiser(*cnf.p_win, cnf, reg, assets, textures), 
            __vk_RuntimeUpdater(getIC(), getSCC(), getDrawCaller(), getResMan(), getDescC(), 
                __vk_RendererInitialiser::m_reg, __vk_RendererInitialiser::m_assets, 
                __vk_RendererInitialiser::m_textures),
            m_config(cnf) {}


        __vk_Renderer::~__vk_Renderer() {
            idle();
            __cleanup();
        }


        /// Free all depth and color image data
        void __vk_Renderer::__cleanRendImgResources() {
            // Clean depth image resources
            vkDestroyImageView(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getResMan().getDepImgView(), NULL);

            vkDestroyImage(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getResMan().getDepImg(), NULL);

            vkFreeMemory(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getResMan().getDepImgMem(), NULL);

            // Clean color image resources
            vkDestroyImageView(__vk_RendererInitialiser::getIC().getDev(),
                __vk_RendererInitialiser::getResMan().getColorImgView(), NULL);

            vkDestroyImage(__vk_RendererInitialiser::getIC().getDev(),
                __vk_RendererInitialiser::getResMan().getColorImg(), NULL);

            vkFreeMemory(__vk_RendererInitialiser::getIC().getDev(),
                __vk_RendererInitialiser::getResMan().getColorImgMem(), NULL);
        }


        void __vk_Renderer::__cleanDrawCommands() {
            // Clean framebuffers
            std::vector<VkFramebuffer> fb;
            fb = __vk_RendererInitialiser::getResMan().getFB();
            for(size_t i = 0; i < fb.size(); i++) {
                vkDestroyFramebuffer(__vk_RendererInitialiser::getIC().getDev(), 
                    fb[i], NULL);
            }

            // Free all commandbuffers
            vkFreeCommandBuffers(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getDrawCaller().getComPool(), 
                static_cast<deng_ui32_t>(__vk_RendererInitialiser::getDrawCaller().getComBufs().size()), 
                __vk_RendererInitialiser::getDrawCaller().getComBufs().data());

        }

        
        void __vk_Renderer::__cleanTextures() {
            LOG("Cleanup texture count: " + std::to_string(m_textures.size()));
            for(size_t i = 0; i < m_textures.size(); i++) {
                // Retrieve base and Vulkan textures
                RegType &reg_tex = __vk_RendererInitialiser::m_reg.retrieve(
                    m_textures[i], DENG_SUPPORTED_REG_TYPE_TEXTURE, NULL);

                RegType &reg_vk_tex = __vk_RendererInitialiser::m_reg.retrieve(
                    reg_tex.tex.vk_id, DENG_SUPPORTED_REG_TYPE_VK_TEXTURE, NULL);

                // Check if image has been buffered and if it then has destroy all of its Vulkan related data
                if(reg_vk_tex.vk_tex.is_buffered) {
                    // Destroy texture sampler
                    vkDestroySampler(__vk_RendererInitialiser::getIC().getDev(),
                        reg_vk_tex.vk_tex.sampler, NULL);

                    // Destroy texture's image views
                    vkDestroyImageView(__vk_RendererInitialiser::getIC().getDev(),
                        reg_vk_tex.vk_tex.image_view, NULL);

                    // Destroy texture image 
                    vkDestroyImage(__vk_RendererInitialiser::getIC().getDev(),
                        reg_vk_tex.vk_tex.image, NULL);
                }
            }

            // Free all memory allocated for texture images
            vkFreeMemory(__vk_RendererInitialiser::getIC().getDev(),
                __vk_RendererInitialiser::getResMan().getBD().img_memory, NULL);
        }

        
        void __vk_Renderer::__cleanAssets() {
            LOG("Asset count: " + std::to_string(__vk_RendererInitialiser::m_assets.size()));
            for(size_t i = 0; i < __vk_RendererInitialiser::m_assets.size(); i++) {
                RegType &reg_vk_asset = __vk_RendererInitialiser::m_reg.retrieve (
                    __vk_RendererInitialiser::m_assets[i], DENG_SUPPORTED_REG_TYPE_VK_ASSET, NULL);

                // Check if asset has descriptor sets allocated and if it does then destroy them
                if(reg_vk_asset.vk_asset.is_desc) {
                    // Retrieve the base asset
                    RegType &reg_asset = __vk_RendererInitialiser::m_reg.retrieve (
                        reg_vk_asset.vk_asset.base_id, DENG_SUPPORTED_REG_TYPE_ASSET, NULL);

                    // Free descriptor sets
                    vkFreeDescriptorSets(__vk_RendererInitialiser::getIC().getDev(), __vk_RendererInitialiser::getDescC().getDescPool(assetModeToPipelineType(reg_asset.asset.asset_mode)),
                        static_cast<deng_ui32_t>(reg_vk_asset.vk_asset.desc_c), reg_vk_asset.vk_asset.desc_sets);
                    reg_vk_asset.vk_asset.is_desc = false;
                }
            }
        }

        
        void __vk_Renderer::__cleanPipelines() {
            // Clean pipeline related data
            std::array<__vk_PipelineData, PIPELINE_C> &pd = 
                __vk_RendererInitialiser::getPipelineC().getPipelines();

            // For each pipeline data instance destroy its pipeline and layout
            for(size_t i = 0; i < pd.size(); i++) { 
                // Destroy pipeline
                vkDestroyPipeline(__vk_RendererInitialiser::getIC().getDev(), 
                    pd[i].pipeline, NULL);

                // Destroy pipeline layout
                vkDestroyPipelineLayout(__vk_RendererInitialiser::getIC().getDev(), 
                    *__vk_RendererInitialiser::getPipelineC().getPipeline(i).p_pipeline_layout, 
                    NULL);
            }

            // Destroy renderpass
            vkDestroyRenderPass(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getSCC().getRp(), NULL);
        }


        void __vk_Renderer::__cleanDescPools() {
            // Destroy 2D unmapped asset descriptor pool 
            vkDestroyDescriptorPool(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getDescC().getDescPool(
                    DENG_PIPELINE_TYPE_UNMAPPED_2D), 
                NULL);

            // Destroy 2D texture mapped asset descriptor pool
            vkDestroyDescriptorPool(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getDescC().getDescPool(
                    DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D), 
                NULL);

            // Destroy 3D unmapped asset descriptor pool 
            vkDestroyDescriptorPool(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getDescC().getDescPool(
                    DENG_PIPELINE_TYPE_UNMAPPED_3D), 
                NULL);

            // Destroy 3D texture mapped asset descriptor pool
            vkDestroyDescriptorPool(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getDescC().getDescPool(
                    DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D), 
                NULL);

            // Destroy ImGui descriptor pool
            vkDestroyDescriptorPool(__vk_RendererInitialiser::getIC().getDev(),
                __vk_RendererInitialiser::getDescC().getDescPool(DENG_PIPELINE_TYPE_UI), NULL);
        }

        
        void __vk_Renderer::__cleanDescSetLayouts() {
            // Destroy 2D unmapped descriptor set layout
            vkDestroyDescriptorSetLayout(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getDescC().getLayout(DENG_PIPELINE_TYPE_UNMAPPED_2D), 
                NULL);

            // Destroy 2D texture mapped descriptor set layout
            vkDestroyDescriptorSetLayout(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getDescC().getLayout(
                    DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D), 
                NULL);

            // Destroy 3D unmapped descriptor set layout
            vkDestroyDescriptorSetLayout(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getDescC().getLayout(DENG_PIPELINE_TYPE_UNMAPPED_3D), 
                NULL);

            // Destroy 3D texture mapped descriptor set layout
            vkDestroyDescriptorSetLayout(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getDescC().getLayout(
                    DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D), 
                NULL);

            // Destroy ImGui descriptor set layouts
            vkDestroyDescriptorSetLayout(__vk_RendererInitialiser::getIC().getDev(),
                __vk_RendererInitialiser::getDescC().getLayout(DENG_PIPELINE_TYPE_UI), NULL);
        }


        void __vk_Renderer::__freeBuffers() {
            // Clean ubo buffer data 
            vkDestroyBuffer(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getResMan().getBD().uniform_buffer, NULL);

            vkFreeMemory(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getResMan().getBD().uniform_buffer_mem, NULL);


            // Clean main buffer data
            vkDestroyBuffer(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getResMan().getBD().main_buffer, NULL);

            vkFreeMemory(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getResMan().getBD().main_buffer_memory, NULL);
        }

        
        void __vk_Renderer::__cleanSemaphores() {
            // Clean semaphores and fences
            for(size_t i = 0; i < __max_frame_c; i++) {
                vkDestroySemaphore(__vk_RendererInitialiser::getIC().getDev(), 
                    __vk_RendererInitialiser::getDrawCaller().image_available_semaphore_set[i], NULL);

                vkDestroySemaphore(__vk_RendererInitialiser::getIC().getDev(), 
                    __vk_RendererInitialiser::getDrawCaller().render_finished_semaphore_set[i], NULL);

                vkDestroyFence(__vk_RendererInitialiser::getIC().getDev(), 
                    __vk_RendererInitialiser::getDrawCaller().flight_fences[i], NULL);
            }
        }

        
        void __vk_Renderer::__cleanDevice() {
            vkDestroyCommandPool(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getDrawCaller().getComPool(), NULL);

            // Clean instance and devices
            vkDestroyDevice(__vk_RendererInitialiser::getIC().getDev(), NULL);

            if(m_config.enable_validation_layers) {
                __vk_InstanceCreator::destroyDebugUtils (
                    __vk_RendererInitialiser::getIC().getIns(), 
                    __vk_RendererInitialiser::getIC().getDMEXT()
                );
            }

            vkDestroySurfaceKHR(__vk_RendererInitialiser::getIC().getIns(), 
                __vk_RendererInitialiser::getIC().getSu(), NULL);

            vkDestroyInstance(__vk_RendererInitialiser::getIC().getIns(), NULL);
        }


        /// Free and destroy all active vulkan instances
        void __vk_Renderer::__cleanup() {
            __cleanRendImgResources();
            __cleanDrawCommands();
            __cleanTextures();
            __cleanAssets();
            __cleanPipelines();
            __vk_RendererInitialiser::getSCC().SCCleanup();
            __cleanDescPools();
            __cleanDescSetLayouts();
            __cleanSemaphores();
            __freeBuffers();
            __cleanDevice();
        }


        /// Prepare assets for rendering with Vulkan
        void __vk_Renderer::prepareAsset(deng_Id id) {
            // Retrive base  asset
            RegType &reg_asset = __vk_RendererInitialiser::m_reg.retrieve(id, 
                DENG_SUPPORTED_REG_TYPE_ASSET, NULL);

            // Create new Vulkan specific asset instance
            RegType reg_vk_asset = {};
            reg_vk_asset.vk_asset.base_id = reg_asset.asset.uuid;
            reg_vk_asset.vk_asset.tex_uuid = reg_asset.asset.tex_uuid;
            reg_vk_asset.vk_asset.uuid = uuid_Generate();
            reg_vk_asset.vk_asset.is_desc = false;
            reg_asset.asset.vk_id = reg_vk_asset.vk_asset.uuid;

            // Push the Vulkan asset entry into registry
            __vk_RendererInitialiser::m_reg.push(reg_vk_asset.vk_asset.uuid, DENG_SUPPORTED_REG_TYPE_VK_ASSET, 
                reg_vk_asset);
        }


        /// Prepare textures for binding them with assets
        void __vk_Renderer::prepareTexture(deng_Id id) {
            LOG("Prep id: " + std::string(id));
            // For each texture between bounds, create Vulkan specific texture instance
            RegType reg_vk_tex;
            reg_vk_tex.vk_tex.base_id = id;
            reg_vk_tex.vk_tex.uuid = uuid_Generate();
            reg_vk_tex.vk_tex.is_buffered = false;

            __vk_RendererInitialiser::m_reg.push(reg_vk_tex.vk_tex.uuid, DENG_SUPPORTED_REG_TYPE_VK_TEXTURE, reg_vk_tex);

            // Retrieve base texture and give it Vulkan texture uuid
            RegType &reg_tex = __vk_RendererInitialiser::m_reg.retrieve(reg_vk_tex.vk_asset.base_id, DENG_SUPPORTED_REG_TYPE_TEXTURE, NULL);
            reg_tex.tex.vk_id = reg_vk_tex.vk_tex.uuid;

            LOG("uuid: " + std::string(reg_tex.tex.uuid));
            LOG("vk_id: " + std::string(reg_tex.tex.vk_id));

            // Create vulkan image resources
            __vk_RendererInitialiser::getResMan().mkTexture(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getIC().getGpu(), __vk_RendererInitialiser::getDrawCaller().getComPool(),
                false, id, __vk_RendererInitialiser::getIC().getQFF().graphics_queue);
        }


        /// Update all currently available light sources
        void __vk_Renderer::setLighting(std::array<deng_Id, __DENG_MAX_LIGHT_SRC_COUNT> &light_srcs) {
            // For each frame in flight, update the light sources' uniform buffer data
            for(deng_ui32_t i = 0; i < __max_frame_c; i++) {
                __vk_RendererInitialiser::getResMan().updateUboLighting (
                    __vk_RendererInitialiser::getIC().getDev(), light_srcs, i);
            }
        }


        /// Submit new draw calls and update uniform data
        void __vk_Renderer::makeFrame() {
            vkWaitForFences(__vk_RendererInitialiser::getIC().getDev(), 1, 
                &__vk_RendererInitialiser::getDrawCaller().flight_fences[__vk_RendererInitialiser::getDrawCaller().current_frame], 
                VK_TRUE, UINT64_MAX);

            // Call Vulkan next image acquire method
            deng_ui32_t image_index;
            VkResult result = vkAcquireNextImageKHR(__vk_RendererInitialiser::getIC().getDev(), __vk_RendererInitialiser::getSCC().getSC(), 
                UINT64_MAX, __vk_RendererInitialiser::getDrawCaller().image_available_semaphore_set[__vk_RendererInitialiser::getDrawCaller().current_frame], 
                VK_NULL_HANDLE, &image_index);
            
            if(result == VK_ERROR_OUT_OF_DATE_KHR) {
                VK_FRAME_ERR("image acquiring timed out");
                return;
            }
            
            else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
                VK_FRAME_ERR("failed to acquire a swap chain image");

            // Update uniform data buffer
            __vk_RendererInitialiser::getResMan().updateUboTransform3D (
                __vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getDrawCaller().current_frame, m_config.p_cam);

            VkSemaphore wait_semaphores[] = 
            { __vk_RendererInitialiser::getDrawCaller().image_available_semaphore_set[__vk_RendererInitialiser::getDrawCaller().current_frame] };

            VkSemaphore signal_semaphores[] = 
            { __vk_RendererInitialiser::getDrawCaller().render_finished_semaphore_set[__vk_RendererInitialiser::getDrawCaller().current_frame] };

            VkSubmitInfo submitinfo{};
            submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submitinfo.waitSemaphoreCount = 1;
            submitinfo.pWaitSemaphores = wait_semaphores;
            submitinfo.pWaitDstStageMask = wait_stages;
            submitinfo.commandBufferCount = 1;
            submitinfo.pCommandBuffers = &__vk_RendererInitialiser::getDrawCaller().getComBufs()[image_index];
            submitinfo.signalSemaphoreCount = 1;
            submitinfo.pSignalSemaphores = signal_semaphores;

            vkResetFences(__vk_RendererInitialiser::getIC().getDev(), 1, 
                &__vk_RendererInitialiser::getDrawCaller().flight_fences[__vk_RendererInitialiser::getDrawCaller().current_frame]);

            VkResult res;
            if((res = vkQueueSubmit(__vk_RendererInitialiser::getIC().getQFF().graphics_queue, 1, &submitinfo, 
               __vk_RendererInitialiser::getDrawCaller().flight_fences[__vk_RendererInitialiser::getDrawCaller().current_frame])) != VK_SUCCESS) 
                VK_FRAME_ERR("failed to submit draw command; error code: " + std::to_string(res));

            VkPresentInfoKHR present_info{};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = signal_semaphores;
            present_info.pImageIndices = &image_index;
            
            VkSwapchainKHR swapchains[] = {__vk_RendererInitialiser::getSCC().getSC()};
            present_info.swapchainCount = 1;
            present_info.pSwapchains = swapchains;

            vkQueuePresentKHR(__vk_RendererInitialiser::getIC().getQFF().present_queue, &present_info);

            m_is_idle = false;
            __vk_RendererInitialiser::getDrawCaller().current_frame = (__vk_RendererInitialiser::getDrawCaller().current_frame + 1) % __max_frame_c;
        }


        /// Setup the renderer, create descriptor sets, allocate buffers
        /// and record command buffers
        void __vk_Renderer::setup() {
            // Create graphics pipelines
            __vk_RendererInitialiser::getPipelineC().mkPipelines(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getSCC().getExt(), __vk_RendererInitialiser::getSCC().getRp(), 
                m_config.msaa_sample_count);

            // Check if any buffer reallocations might be needed
            const deng_bool_t is_realloc = __vk_RendererInitialiser::getResMan().reallocCheck(__vk_RendererInitialiser::getIC().getDev(),
                __vk_RendererInitialiser::getIC().getGpu(), __vk_RendererInitialiser::getDrawCaller().getComPool(),
                __vk_RendererInitialiser::getIC().getQFF().graphics_queue, __vk_RendererInitialiser::getDrawCaller().flight_fences);

            // Copy all available assets to the main buffer, if no reallocation occured
            if(!is_realloc) {
                __vk_RendererInitialiser::getResMan().cpyAssetsToBuffer(__vk_RendererInitialiser::getIC().getDev(), 
                    __vk_RendererInitialiser::getIC().getGpu(), __vk_RendererInitialiser::getDrawCaller().getComPool(),
                    __vk_RendererInitialiser::getIC().getQFF().graphics_queue, false, { 0, static_cast<deng_ui32_t>(__vk_RendererInitialiser::m_assets.size()) });

                // Copy all available ui data to the main buffer
                __vk_RendererInitialiser::getResMan().cpyUIDataToBuffer(__vk_RendererInitialiser::getIC().getDev(), 
                    __vk_RendererInitialiser::getIC().getGpu(), __vk_RendererInitialiser::getDrawCaller().getComPool(),
                    __vk_RendererInitialiser::getIC().getQFF().graphics_queue, false);
            }

            // Create descriptor sets for all currently available assets
            __vk_RendererInitialiser::getDescC().mkAssetDS(__vk_RendererInitialiser::getIC().getDev(), __vk_RendererInitialiser::getResMan().getBD(), 
                __vk_RendererInitialiser::getResMan().getMissingTextureUUID(), { 0, static_cast<deng_ui32_t>(__vk_RendererInitialiser::m_assets.size()) },
                __vk_RendererInitialiser::getResMan().getUboChunkSize(), __vk_RendererInitialiser::getIC().getGpuLimits().minUniformBufferOffsetAlignment);

            // Start recording command buffers
            __vk_RendererInitialiser::getDrawCaller().setMiscData(__vk_RendererInitialiser::getPipelineC().getPipelines(),
                __vk_RendererInitialiser::getResMan().getFB());

            // Allocate memory for commandbuffers
            __vk_RendererInitialiser::getDrawCaller().allocateCmdBuffers(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getIC().getQFF().graphics_queue, __vk_RendererInitialiser::getSCC().getRp(),
                __vk_RendererInitialiser::getSCC().getExt(), m_config.background, __vk_RendererInitialiser::getResMan().getBD(),
                __vk_RendererInitialiser::getResMan().getSectionInfo());

            // Record commandbuffers with initial draw commands
            __vk_RendererInitialiser::getDrawCaller().recordCmdBuffers(__vk_RendererInitialiser::getSCC().getRp(),
                __vk_RendererInitialiser::getSCC().getExt(), m_config.background, __vk_RendererInitialiser::getResMan().getBD(),
                __vk_RendererInitialiser::getResMan().getSectionInfo());

            // Set the running flag as true
            m_is_init = true;
        }


        /// Wait for any queue operation to finish
        /// This method needs to be called whenever any 
        /// command or data buffers need to be updated
        void __vk_Renderer::idle() {
            if(m_is_init && !m_is_idle) {
                vkWaitForFences(__vk_RendererInitialiser::getIC().getDev(), static_cast<deng_ui32_t>(__vk_RendererInitialiser::getDrawCaller().flight_fences.size()), 
                    __vk_RendererInitialiser::getDrawCaller().flight_fences.data(), VK_TRUE, UINT64_MAX);

                m_is_idle = true;
            }
        }


        /// Setter and getter methods
        void __vk_Renderer::setUIDataPtr(__ImGuiData *p_gui) {
            __vk_RuntimeUpdater::m_p_gui_data = p_gui;
            __vk_RendererInitialiser::setUIDataPtr(p_gui);

            // Check if the renderer has been initialised and if it has idle it
            //if(m_is_init) idle();

            // Perform main buffer remapping for potentially new vertices to draw
            // Check if any buffer reallocations might be needed
            const deng_bool_t is_realloc = __vk_RendererInitialiser::getResMan().reallocCheck(__vk_RendererInitialiser::getIC().getDev(),
                __vk_RendererInitialiser::getIC().getGpu(), __vk_RendererInitialiser::getDrawCaller().getComPool(),
                __vk_RendererInitialiser::getIC().getQFF().graphics_queue, __vk_RendererInitialiser::getDrawCaller().flight_fences);

            if(!is_realloc) {
                // Copy all available assets to the main buffer
                __vk_RendererInitialiser::getResMan().cpyAssetsToBuffer(__vk_RendererInitialiser::getIC().getDev(), 
                    __vk_RendererInitialiser::getIC().getGpu(), __vk_RendererInitialiser::getDrawCaller().getComPool(),
                    __vk_RendererInitialiser::getIC().getQFF().graphics_queue, false, { 0, static_cast<deng_ui32_t>(__vk_RendererInitialiser::m_assets.size()) });

                // Copy all available ui data to the main buffer
                __vk_RendererInitialiser::getResMan().cpyUIDataToBuffer(__vk_RendererInitialiser::getIC().getDev(), 
                    __vk_RendererInitialiser::getIC().getGpu(), __vk_RendererInitialiser::getDrawCaller().getComPool(),
                    __vk_RendererInitialiser::getIC().getQFF().graphics_queue, false);
            }

            // Create UI element descriptor sets
            __vk_RendererInitialiser::getDescC().mkUIDS(__vk_RendererInitialiser::getIC().getDev(), 
                __vk_RendererInitialiser::getResMan().getBD(), p_gui->tex_id);
        }


        const deng_bool_t __vk_Renderer::isInit() {
            return m_is_init;
        }


        const __vk_BufferData &__vk_Renderer::getBufferData() {
            return __vk_RendererInitialiser::getResMan().getBD();
        }
    }
}
