/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: offset_finder.cpp - cross api asset and gui data offset helper class implementation
/// author: Karl-Mihkel Ott


#define __OFFSET_FINDER_CPP
#include <deng/cross_api/offset_finder.h>


namespace deng {

    __OffsetFinder::__OffsetFinder(std::vector<deng_Id> &assets, Registry &reg) : 
        m_assets(assets), m_reg(reg), m_buf_sec_info() {}


    /// Check if the current capacity is enough for assets and if it isn't resize the capacity and return true
    const deng_bool_t __OffsetFinder::assetCapCheck() {
        if(m_buf_sec_info.asset_size < m_buf_sec_info.asset_cap) return false;

        // Scale the current or required capacity by a factor of 1.5 and set it as a new capacity
        m_buf_sec_info.asset_cap = std::max(m_buf_sec_info.asset_cap * 3 /2, m_buf_sec_info.asset_size * 3 / 2);
        m_buf_sec_info.asset_cap += ZERO_MOD_CEIL_REM(m_buf_sec_info.asset_cap, sizeof(deng_idx_t));

        return true;
    }


	/// Check if the current capacity is enough for asset indices and if it isn't resize the capacity and return true
    const deng_bool_t __OffsetFinder::indicesCapCheck() {
        if (m_buf_sec_info.indices_size < m_buf_sec_info.indices_cap) return false;

        // Scale the current or required capacity by a factor of 1.5 and set it as a new capacity
        m_buf_sec_info.indices_cap = std::max(m_buf_sec_info.indices_cap * 3 / 2, m_buf_sec_info.indices_size * 3 / 2);
        m_buf_sec_info.indices_cap += ZERO_MOD_CEIL_REM(m_buf_sec_info.indices_cap, sizeof(deng_idx_t));

        return true;
    }


    /// Check if the current capacity is enough for ImGui elements and if it isn't resize the capacity and return true
    const deng_bool_t __OffsetFinder::uiCapCheck() {
        if(m_buf_sec_info.ui_size < m_buf_sec_info.ui_cap) return false;

        // Scale the current or required capacity by a factor of 1.5 and set it as a new capacity
        m_buf_sec_info.ui_cap = std::max(m_buf_sec_info.ui_cap * 3 / 2, m_buf_sec_info.ui_size * 3 / 2);
        m_buf_sec_info.ui_cap += ZERO_MOD_CEIL_REM(m_buf_sec_info.ui_cap, sizeof(deng_idx_t));

        return true;
    }


    void __OffsetFinder::__findGLAssetOffsets(das_Asset &asset) {
        // Check the current asset type and calculate offsets accordingly
        switch(asset.asset_mode) {
        case DAS_ASSET_MODE_2D_UNMAPPED:
            asset.offsets.pos_offset = m_buf_sec_info.asset_size;
            m_buf_sec_info.asset_size += asset.vertices.v2d.mer.n * sizeof(das_GL2DVertexUnmapped);

            m_buf_sec_info.asset_size += ZERO_MOD_CEIL_REM(m_buf_sec_info.asset_size, sizeof(deng_idx_t));
            asset.offsets.ind_offset = m_buf_sec_info.indices_size;
            m_buf_sec_info.indices_size += asset.indices.n * sizeof(deng_idx_t);
            break;

        case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
            asset.offsets.pos_offset = m_buf_sec_info.asset_size;
            m_buf_sec_info.asset_size += asset.vertices.v2d.mer.n * sizeof(das_GL2DVertex);

            m_buf_sec_info.asset_size += ZERO_MOD_CEIL_REM(m_buf_sec_info.asset_size, sizeof(deng_idx_t));
            asset.offsets.ind_offset = m_buf_sec_info.indices_size;
            m_buf_sec_info.indices_size += asset.indices.n * sizeof(deng_idx_t);
            break;

        case DAS_ASSET_MODE_3D_UNMAPPED:
            asset.offsets.pos_offset = m_buf_sec_info.asset_size;
            m_buf_sec_info.asset_size += asset.vertices.v3d.mer.n * sizeof(das_GL3DVertexUnmapped);

            m_buf_sec_info.asset_size += ZERO_MOD_CEIL_REM(m_buf_sec_info.asset_size, sizeof(deng_idx_t));
            asset.offsets.ind_offset = m_buf_sec_info.indices_size;
            m_buf_sec_info.indices_size += asset.indices.n * sizeof(deng_idx_t);
            break;

        case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
            asset.offsets.pos_offset = m_buf_sec_info.asset_size;
            m_buf_sec_info.asset_size += asset.vertices.v3d.mer.n * sizeof(das_GL3DVertex);

            m_buf_sec_info.asset_size += ZERO_MOD_CEIL_REM(m_buf_sec_info.asset_size, sizeof(deng_idx_t));
            asset.offsets.ind_offset = m_buf_sec_info.indices_size;
            m_buf_sec_info.indices_size += asset.indices.n * sizeof(deng_idx_t);
            break;

        default:
            break;
        }
    }


    void __OffsetFinder::__findVkAssetOffsets(das_Asset &asset) {
        // Check the asset mode for calculating correct offsets
        switch (asset.asset_mode) {
        case DAS_ASSET_MODE_2D_UNMAPPED:
            // Set all vertices' offsets and the vertices that are not available mark as
            // UINT64_MAX
            asset.offsets.pos_offset = m_buf_sec_info.asset_size;
            asset.offsets.tex_offset = UINT64_MAX;
            asset.offsets.nor_offset = UINT64_MAX;

            m_buf_sec_info.asset_size += asset.vertices.v2d.mul.pn * 
                sizeof(das_ObjPosData2D);

            // Ceil the asset size to the nearest multiple of deng_idx_t size
            m_buf_sec_info.asset_size += ZERO_MOD_CEIL_REM(m_buf_sec_info.asset_size, sizeof(deng_idx_t));
            asset.offsets.ind_offset = m_buf_sec_info.asset_size;
            m_buf_sec_info.asset_size += asset.indices.n * sizeof(deng_idx_t);
            break;
        
        case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
            // Set all vertices' offsets and the vertices that are not available mark as
            // UINT64_MAX
            asset.offsets.pos_offset = m_buf_sec_info.asset_size;
            asset.offsets.tex_offset = m_buf_sec_info.asset_size +
                asset.vertices.v2d.mul.pn * sizeof(das_ObjPosData2D);
            asset.offsets.nor_offset = UINT64_MAX;

            m_buf_sec_info.asset_size += asset.vertices.v2d.mul.pn * sizeof(das_ObjPosData2D) + 
                asset.vertices.v2d.mul.tn * sizeof(das_ObjTextureData);

            m_buf_sec_info.asset_size += ZERO_MOD_CEIL_REM(m_buf_sec_info.asset_size, sizeof(deng_idx_t));
            asset.offsets.ind_offset = m_buf_sec_info.asset_size;
            m_buf_sec_info.asset_size += 2 * asset.indices.n * sizeof(deng_idx_t);
            break;

        case DAS_ASSET_MODE_3D_UNMAPPED:
            // Set all vertices' offsets and the vertices that are not available mark as
            // UINT64_MAX
            asset.offsets.pos_offset = m_buf_sec_info.asset_size;
            asset.offsets.tex_offset = UINT64_MAX;
            asset.offsets.nor_offset = m_buf_sec_info.asset_size + 
                asset.vertices.v3d.mul.pn * sizeof(das_ObjPosData);

            m_buf_sec_info.asset_size += asset.vertices.v3d.mul.pn * sizeof(das_ObjPosData) +
            asset.vertices.v3d.mul.nn * sizeof(das_ObjNormalData);

            m_buf_sec_info.asset_size += ZERO_MOD_CEIL_REM(m_buf_sec_info.asset_size, sizeof(deng_idx_t));
            asset.offsets.ind_offset = m_buf_sec_info.asset_size;
            m_buf_sec_info.asset_size += 2 * asset.indices.n * sizeof(deng_idx_t);
            break;

        case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
            asset.offsets.pos_offset = m_buf_sec_info.asset_size;
            asset.offsets.tex_offset = m_buf_sec_info.asset_size +
                asset.vertices.v3d.mul.pn * sizeof(das_ObjPosData);
            asset.offsets.nor_offset = m_buf_sec_info.asset_size +
                asset.vertices.v3d.mul.pn * sizeof(das_ObjPosData) + 
                asset.vertices.v3d.mul.tn * sizeof(das_ObjTextureData);

            m_buf_sec_info.asset_size += asset.vertices.v3d.mul.pn * sizeof(das_ObjPosData) +
                asset.vertices.v3d.mul.tn * sizeof(das_ObjTextureData) + 
                asset.vertices.v3d.mul.nn * sizeof(das_ObjNormalData);

            m_buf_sec_info.asset_size += ZERO_MOD_CEIL_REM(m_buf_sec_info.asset_size, sizeof(deng_idx_t));
            asset.offsets.ind_offset = m_buf_sec_info.asset_size;
            m_buf_sec_info.asset_size += 3 * asset.indices.n * sizeof(deng_idx_t);
            break;
        
        default:
            RUN_ERR("deng::vulkan::__vk_ResourceManager::mkBuffers()", 
                "Invalid asset vertices format for asset " + std::string(asset.uuid));
            break;
        }
    }


    /// Find offsets for the current asset
    void __OffsetFinder::__findAssetOffsets(das_Asset &asset) {
        // Check if opengl is used for the asset rendering and call appropriate offset finder function
        if(asset.is_opengl) __findGLAssetOffsets(asset);
        else __findVkAssetOffsets(asset);

    }

    /// Find offsets for all ImGui entities 
    void __OffsetFinder::__findGuiEntitiesOffsets(const deng_bool_t is_opengl) {
        // For each command data vector element, calculate its offset from the beginning of the ui memory area
        for(size_t i = 0; i < m_p_imgui_data->cmd_data.size(); i++) {
            m_p_imgui_data->cmd_data[i].voffset = m_buf_sec_info.ui_size;
            m_buf_sec_info.ui_size += m_p_imgui_data->cmd_data[i].vert_c * sizeof(ImDrawVert);
            m_buf_sec_info.ui_size += ZERO_MOD_CEIL_REM(m_buf_sec_info.ui_size, sizeof(ImDrawIdx));

            if(is_opengl) {
                m_p_imgui_data->cmd_data[i].ioffset = m_buf_sec_info.indices_size;
                m_buf_sec_info.indices_size += m_p_imgui_data->cmd_data[i].ind_c * sizeof(ImDrawIdx);
            }
            else m_buf_sec_info.ui_size += m_p_imgui_data->cmd_data[i].ind_c * sizeof(ImDrawIdx);
        }
    }


    /// Calculate all offsets for all available assets and ui elements if possible
    void __OffsetFinder::__calcOffsets(const deng_bool_t is_opengl) {
        // For each asset add its size to total asset size and calculate asset offsets
        m_buf_sec_info.asset_size = 0;
        m_buf_sec_info.indices_size = 0;
        for(size_t i = 0; i < m_assets.size(); i++) {
            // Retrieve the asset from registry
            RegData &reg_asset = m_reg.retrieve(m_assets[i], DENG_REGISTRY_TYPE_ASSET, NULL);
            __findAssetOffsets(reg_asset.asset);
        }


        // Calculate the total required memory for ui elements
        if(m_p_imgui_data) {
            m_buf_sec_info.ui_size = 0;
            __findGuiEntitiesOffsets(is_opengl);
        }
    }


    /// Find the largest asset size
    const deng_ui64_t __OffsetFinder::__findMaxAssetSize(const dengMath::vec2<deng_ui32_t> &bounds) {
        VkDeviceSize max_size = 0;
        VkDeviceSize cur_size = 0;

        // For each asset in bounds check the size and find the largest one
        for(size_t i = bounds.first; i < bounds.second; i++) {
            // Retrieve assets from the registry
            RegData &reg_asset = m_reg.retrieve(m_assets[i], DENG_REGISTRY_TYPE_ASSET, NULL);

            // Check how much memory the asset consumes by checking their mode, which
            // they used to allocate memory for the vertex buffer
            switch(reg_asset.asset.asset_mode) {
            case DAS_ASSET_MODE_2D_UNMAPPED:
                cur_size = reg_asset.asset.vertices.v2d.mul.pn * sizeof(das_ObjPosData);
                cur_size += ZERO_MOD_CEIL_REM(cur_size, sizeof(deng_idx_t));
                cur_size += reg_asset.asset.indices.n * sizeof(deng_idx_t);
                break;

            case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                cur_size = reg_asset.asset.vertices.v2d.mul.pn * sizeof(das_ObjPosData) +
                    reg_asset.asset.vertices.v2d.mul.tn * sizeof(das_ObjTextureData);
                cur_size += ZERO_MOD_CEIL_REM(cur_size, sizeof(deng_idx_t));
                cur_size += 2 * reg_asset.asset.indices.n * sizeof(deng_idx_t);
                break;

            case DAS_ASSET_MODE_3D_UNMAPPED:
                cur_size = reg_asset.asset.vertices.v3d.mul.pn * sizeof(das_ObjPosData) +
                    reg_asset.asset.vertices.v3d.mul.nn * sizeof(das_ObjNormalData);
                cur_size += ZERO_MOD_CEIL_REM(cur_size, sizeof(deng_idx_t));
                cur_size += 2 * reg_asset.asset.indices.n * sizeof(deng_idx_t);
                break;

            case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                cur_size = reg_asset.asset.vertices.v3d.mul.pn * sizeof(das_ObjPosData) +
                    reg_asset.asset.vertices.v3d.mul.tn * sizeof(das_ObjTextureData) +
                    reg_asset.asset.vertices.v3d.mul.nn * sizeof(das_ObjNormalData);
                cur_size += ZERO_MOD_CEIL_REM(cur_size, sizeof(deng_idx_t));
                cur_size += 3 * reg_asset.asset.indices.n * sizeof(deng_idx_t);
                break;

            default:
                break;
            }

            // Perform maximum required memory check for the asset
            if(cur_size > max_size)
                max_size = cur_size;
        }

        return max_size;
    }


    
    void __OffsetFinder::setUIData(__ImGuiData *p_gui) { m_p_imgui_data = p_gui; }
    BufferSectionInfo &__OffsetFinder::getSectionInfo() { return m_buf_sec_info; }
}
