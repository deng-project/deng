/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: rend_init.h - Vulkan renderer initialiser class header file
/// author: Karl-Mihkel Ott


#ifndef __VK_REND_INIT_H
#define __VK_REND_INIT_H


#ifdef __VK_REND_INIT_CPP
    #include <mutex>
    #include <string.h>
    #include <vector>
    #include <queue>
    #include <array>
    #include <memory>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/hashmap.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/cam3d.h>

    #include <deng/vulkan/qm.h>
    #include <deng/vulkan/sd.h>
    #include <deng/vulkan/resources.h>

    #include <deng/lighting/light_srcs.h>
    #include <deng/registry/registry.h>
    #include <imgui-layer/imgui_entity.h>

    #include <deng/cross_api/shader_def.h>
    #include <deng/cross_api/gpu_mem.h>
    #include <deng/cross_api/offset_finder.h>
    
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

    extern deng_ui32_t __max_frame_c;
#endif


namespace deng {
    namespace vulkan {

        /// Vulkan renderer config variables
        struct __vk_ConfigVars {
            VkSampleCountFlagBits msaa_sample_count = VK_SAMPLE_COUNT_1_BIT;
            deng_bool_t enable_vsync = false;
            deng_bool_t enable_validation_layers = false;
            dengMath::vec4<deng_vec_t> background = {0.0f, 0.0f, 0.0f, 1.0f};
            deng::Camera3D *p_cam;
            deng::Window *p_win;
        };


        class __vk_RendererInitialiser {
        private:
            // All renderer initialisers
            std::unique_ptr<__vk_InstanceCreator> m_ic = {};
            std::unique_ptr<__vk_SwapChainCreator> m_scc = {};
            std::unique_ptr<__vk_DescriptorSetsCreator> m_desc_c = {}; 
            std::unique_ptr<__vk_PipelineCreator> m_pl_c = {};
            std::unique_ptr<__vk_ResourceManager> m_rm = {};
            std::unique_ptr<__vk_DrawCaller> m_dc = {};

        protected:
            Registry &m_reg;
            std::vector<deng_Id> &m_assets;
            std::vector<deng_Id> &m_textures;

        public:
            __vk_RendererInitialiser (
                deng::Window &win, 
                const __vk_ConfigVars &conf,
                deng::Registry &reg,
                std::vector<deng_Id> &assets,
                std::vector<deng_Id> &textures
            );

        /// Getter and setter methods
        public:
            __vk_InstanceCreator &getIC();
            __vk_SwapChainCreator &getSCC();
            __vk_DescriptorSetsCreator &getDescC();
            __vk_PipelineCreator &getPipelineC();
            __vk_ResourceManager &getResMan();
            __vk_DrawCaller &getDrawCaller();

            void setUIDataPtr(__ImGuiData *p_gui);
        };
    }
}

#endif
