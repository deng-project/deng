/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: runtime_updater.h - Vulkan runtime buffer data update handler class header
/// author: Karl-Mihkel Ott


#ifndef __VK_RUNTIME_UPDATER_H
#define __VK_RUNTIME_UPDATER_H

#ifdef __VK_RUNTIME_UPDATE_CPP
    #include <memory>
    #include <vector>
    #include <array>
    #include <chrono>
    #include <queue>
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
    #include <deng/cross_api/cross_api.h>

    #include <deng/vulkan/rend_infos.h>
    #include <deng/vulkan/ic.h>
    #include <deng/vulkan/scc.h>
    #include <deng/vulkan/desc_set_layout.h>
    #include <deng/vulkan/desc_pool.h>
    #include <deng/vulkan/desc_sets.h>
    #include <deng/vulkan/pipeline_data.h>
    #include <deng/vulkan/pipelines.h>
    #include <deng/vulkan/dc.h>
    #include <deng/vulkan/tm.h>
    #include <deng/vulkan/ubm.h>
    #include <deng/vulkan/bm.h>
    #include <deng/vulkan/rm.h>
#endif


namespace deng {
    namespace vulkan {

        class __vk_RuntimeUpdater {
        private:
            __vk_InstanceCreator &m_ic;
            __vk_SwapChainCreator &m_scc;
            __vk_DrawCaller &m_dc;
            __vk_ResourceManager &m_rm;
            __vk_DescriptorSetsCreator &m_desc_c;
            Registry &m_reg;
            std::vector<deng_Id> &m_assets;
            std::vector<deng_Id> &m_tex;

        protected:
            __ImGuiData *m_p_gui_data = NULL;

            // Flag which triggers commandbuffer level switching
            deng_bool_t m_switch_flag = false;

        public:
            __vk_RuntimeUpdater (
                __vk_InstanceCreator &ic,
                __vk_SwapChainCreator &scc,
                __vk_DrawCaller &dc,
                __vk_ResourceManager &rm,
                __vk_DescriptorSetsCreator &desc_c,
                Registry &reg,
                std::vector<deng_Id> &assets, 
                std::vector<deng_Id> &tex
            );

            /// This method updates the vertices buffer that is allocated by given assets
            void updateAssetData(const dengMath::vec2<deng_ui32_t> &bounds);


            /// Update ImGui vertices and indices
            void updateUIData(const dengMath::vec4<deng_vec_t> &background);


            /// Rerecord existing commandbuffers 
            void updateCmdBuffers(const dengMath::vec4<deng_vec_t> &background);

            
            /// Reallocate or allocate new descriptor sets for assets between given bounds
            /// NOTE: Vulkan renderer must be idled
            void updateAssetDS(const dengMath::vec2<deng_ui32_t> &bounds);


            /// Reallocate main buffer and copy all asset data to it if needed
            /// NOTE: Vulkan renderer must be idled
            void checkForReallocation();


            /// Update texture texel data, however it does not perform any texture size adjustments. For that
            /// consider creating a new texture
            void updateTexelData(__vk_Texture &tex);
        };
    }
}

#endif
