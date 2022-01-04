/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: VulkanRenderer.h - Vulkan renderer class header
/// author: Karl-Mihkel Ott


#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#ifdef VULKAN_RENDERER_CPP
    #include <string.h>
    #include <vector>
    #include <memory>
    #include <chrono>
    #include <string>
    #include <queue>
    #include <array>
    #include <thread>
    
    #include <vulkan/vulkan.h>
    #include <base_types.h>
    #include <hashmap.h>
    #include <common/uuid.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/cam3d.h>

    #include <deng/lighting/light_srcs.h>
    #include <deng/registry/registry.h>
    #include <imgui-layer/imgui_entity.h>
    #include <deng/cross_api/gpu_mem.h>
    #include <deng/cross_api/shader_def.h>
    #include <deng/cross_api/offset_finder.h>

    extern deng_ui32_t __max_frame_c;
#endif


/// Include all vulkan headers
#include <deng/vulkan/qm.h>
#include <deng/vulkan/sd.h>
#include <deng/vulkan/resources.h>
#include <deng/vulkan/rend_infos.h>
#include <deng/vulkan/ic.h>
#include <deng/vulkan/scc.h>
#include <deng/vulkan/pipeline_data.h>
#include <deng/vulkan/pipelines.h>
#include <deng/vulkan/desc_set_layout.h>
#include <deng/vulkan/desc_pool.h>
#include <deng/vulkan/desc_sets.h>
#include <deng/vulkan/dc.h>
#include <deng/vulkan/tm.h>
#include <deng/vulkan/ubm.h>
#include <deng/vulkan/bm.h>
#include <deng/vulkan/rm.h>

#include <deng/vulkan/runtime_updater.h>
#include <deng/vulkan/rend_init.h>


namespace DENG {
    namespace Vulkan {

        /// Main renderer class for Vulkan API
        class Renderer : private RendererInitialiser,
                         public RuntimeUpdater 
        {
        private:
            void *m_udata;
            __vk_ConfigVars m_config;
            dengMath::vec2<deng_i32_t> m_prev_size;
            deng_bool_t m_is_init = false;
            deng_bool_t m_is_idle = true;

        private:

            /// Free and destroy all active Vulkan API specific instances
            void __cleanup();


            /// Additional cleanup methods
            void __cleanRendImgResources();
            void __cleanDrawCommands();
            void __cleanTextures();
            void __cleanAssets();
            void __cleanPipelines();
            void __cleanDescPools();
            void __cleanDescSetLayouts();
            void __freeBuffers();
            void __cleanSemaphores();
            void __cleanDevice();
            void __swapChainRecreationCheck();

            void __makeFrame();

        public:
            __vk_Renderer(__vk_ConfigVars &cnf, deng::Registry &reg,
                std::vector<deng_Id> &assets, std::vector<deng_Id> &textures);


            ~__vk_Renderer();

            /// Prepare assets for rendering
            void prepareAsset(deng_Id id);


            /// Prepare textures for binding them with assets
            void prepareTexture(deng_Id id);

            
            /// Update all currently available light sources
            void setLighting(std::array<deng_Id, __DENG_MAX_LIGHT_SRC_COUNT> &light_srcs);

            
            /// Submit new draw call
            void makeFrame();


            /// Setup the renderer, create descriptor sets, allocate buffers
            /// and record command buffers
            void setup();


            /// Wait for any queue operation to finish
            /// This method needs to be called whenever any 
            /// command or data buffers need to be updated
            void idle();

        // Setter and getter methods
        public:
            void setUIDataPtr(__ImGuiData *p_data);
            const deng_bool_t isInit();
            const __vk_BufferData &getBufferData();
        };
    }
}

#endif
