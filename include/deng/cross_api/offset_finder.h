/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: offset_finder.h - cross api asset and gui data offset helper class header
/// author: Karl-Mihkel Ott


#ifndef __OFFSET_FINDER_H
#define __OFFSET_FINDER_H


#ifdef __OFFSET_FINDER_CPP
    #include <string.h>
    #include <vector>
    #include <string>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/common.h>
    #include <common/hashmap.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/registry/registry.h>
    #include <deng/cross_api/gpu_mem.h>
    #include <imgui-layer/imgui_entity.h>
#endif


namespace deng {

    class __OffsetFinder {
    private:
        std::vector<deng_Id> &m_assets;
        Registry &m_reg;
        __ImGuiData *m_p_imgui_data = NULL;
        BufferSectionInfo m_buf_sec_info = {};

    private:
        /// Vertex buffer alignment specific offset finder methods
        void __findGLAssetOffsets(das_Asset &asset);
        void __findVkAssetOffsets(das_Asset &asset);

    protected:
        /// Find offsets for the current asset
        void __findAssetOffsets(das_Asset &asset);


        /// Find offsets for all ImGui entities
        void __findGuiEntitiesOffsets(const deng_bool_t is_opengl);


        /// Calculate all offsets for all available assets and ui elements if possible
        void __calcOffsets(const deng_bool_t is_opengl);


        /// Find the largest memory size required by a single asset 
        const deng_ui64_t __findMaxAssetSize(const dengMath::vec2<deng_ui32_t> &bounds);

    public:
        __OffsetFinder(std::vector<deng_Id> &assets, Registry &reg);


        /// Check if the current capacity is enough for assets and if it isn't resize the capacity and return true
        const deng_bool_t assetCapCheck();


        /// Check if the current capacity is enough for asset indices and if it isn't resize the capacity and return true
        const deng_bool_t indicesCapCheck();


        /// Check if the current capacity is enough for ImGui elements and if it isn't resize the capacity and return true
        const deng_bool_t uiCapCheck();

    // Setters and getters
    public:
        void setUIData(__ImGuiData *p_gui);
        BufferSectionInfo &getSectionInfo();
    };
}

#endif
