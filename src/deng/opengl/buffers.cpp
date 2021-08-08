/// DENG: dynamic engine - powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: buffers.h - OpenGL buffer manager class implementation
/// author: Karl-Mihkel Ott

#define __GL_BUFFERS_CPP
#include <deng/opengl/buffers.h>


namespace deng {
    namespace opengl {

        __gl_BufferManager::__gl_BufferManager (
            std::vector<deng_Id> &assets, 
            std::shared_ptr<__gl_Pipelines> pipelines, 
            __GlobalRegistry &reg,
            void (*gl_error_check)(const std::string &func_name)
        ) :
            __gl_UniformManager(reg, assets, m_resources), m_assets(assets), m_reg(reg), m_pipelines(pipelines),
            glErrorCheck(gl_error_check)
        {
            // Generate new buffers for vertices, indices and uniform data
            glGenBuffers(1, &m_resources.vert_buffer);
            glErrorCheck("glGenBuffers");
            glBindBuffer(GL_ARRAY_BUFFER, m_resources.vert_buffer);
            glErrorCheck("glBindBuffer");

            glGenBuffers(1, &m_resources.idx_buffer);
            glErrorCheck("glGenBuffers");
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_resources.idx_buffer);
            glErrorCheck("glBindBuffer");

            glGenBuffers(1, &m_resources.ubo_buffer);
            glErrorCheck("glGenBuffers");
            glBindBuffer(GL_UNIFORM_BUFFER, m_resources.ubo_buffer);
            glErrorCheck("glBindBuffer");

            LOG("vert, idx, ubo buffer values: " + std::to_string(m_resources.vert_buffer) + " " + std::to_string(m_resources.idx_buffer) + " " +
                std::to_string(m_resources.ubo_buffer));
            glErrorCheck("glGenBuffers");

            // Bind all buffers accordingly

            glGenVertexArrays(1, &m_resources.vert_array);
            glErrorCheck("glGenVertexArrays");
            glBindVertexArray(m_resources.vert_array);
            glErrorCheck("glBindVertexArray");

            // Find all asset offsets
            __OffsetFinder::getSectionInfo().asset_size = 0;
            for(deng_ui64_t i = 0; i < m_assets.size(); i++) {
                RegType &reg_asset = m_reg.retrieve(m_assets[i], DENG_SUPPORTED_REG_TYPE_ASSET, NULL);
                __OffsetFinder::__findAssetOffsets(reg_asset.asset);
            }

            // Allocate the initial amount of memory for buffers
            allocateBufferMemory();
        }


        /// Allocate ui_cap + asset_cap amount of memory for the buffer
        void __gl_BufferManager::allocateBufferMemory() {
            // Allocate memory for vertex buffer
            glBufferData(GL_ARRAY_BUFFER, __OffsetFinder::getSectionInfo().asset_cap + 
                __OffsetFinder::getSectionInfo().ui_cap, NULL, GL_DYNAMIC_DRAW);
            LOG("initialising asset vertex buffer with capacity of " + std::to_string(__OffsetFinder::getSectionInfo().asset_cap));

            // Allocate memory for indices buffer
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, __OffsetFinder::getSectionInfo().indices_cap, NULL, GL_DYNAMIC_DRAW);
            LOG("initialising indices buffer with capacity of " + std::to_string(__OffsetFinder::getSectionInfo().indices_cap));

            // Allocate memory for uniform buffers
            glBufferData(GL_UNIFORM_BUFFER, __OffsetFinder::getSectionInfo().ui_cap, NULL, GL_DYNAMIC_DRAW);

            glErrorCheck("glBufferData");
        }


        /// Check if buffer reallocation is needed for assets and gui elements
        deng_bool_t __gl_BufferManager::reallocCheck() {
            __OffsetFinder::__calcOffsets(true);
            const deng_bool_t asset_realloc = __OffsetFinder::assetCapCheck();
            const deng_bool_t ui_realloc = __OffsetFinder::uiCapCheck();

            if(asset_realloc || ui_realloc) {
                allocateBufferMemory();
                cpyAssetsToBuffer(true, dengMath::vec2<deng_ui32_t>{ 0, static_cast<deng_ui32_t>(m_assets.size()) });
                if(m_p_imgui_data) cpyUIDataToBuffer(true);
            }

            return asset_realloc || ui_realloc;
        }


        /// Copy all asset data between given bounds to buffer
        /// NOTE: The asset capacity in buffer has to be larger than required asset size (use assetCapCheck() for this)
        void __gl_BufferManager::cpyAssetsToBuffer(deng_bool_t no_offset_calc, const dengMath::vec2<deng_ui32_t> &bounds) {
            if(!no_offset_calc) {
                __OffsetFinder::getSectionInfo().asset_size = 0;
                __OffsetFinder::getSectionInfo().indices_size = 0;
                for(deng_ui64_t i = 0; i < m_assets.size(); i++) {
                    RegType &reg_asset = m_reg.retrieve(m_assets[i], DENG_SUPPORTED_REG_TYPE_ASSET, NULL);
                    __OffsetFinder::__findAssetOffsets(reg_asset.asset);
                }
            }
 
            // For each asset copy its data over to the buffers
            for(deng_ui32_t i = bounds.first; i < bounds.second; i++) {
                RegType &reg_asset = m_reg.retrieve(m_assets[i], DENG_SUPPORTED_REG_TYPE_ASSET, NULL);

                // Check the asset type and bind data accordingly
                switch(reg_asset.asset.asset_mode) {
                case DAS_ASSET_MODE_2D_UNMAPPED:
                    glBufferSubData(GL_ARRAY_BUFFER, reg_asset.asset.offsets.pos_offset, reg_asset.asset.vertices.v2d.mer.n * 
                        sizeof(das_GL2DVertexUnmapped), reg_asset.asset.vertices.v2d.mer.uvert);
                    break;

                case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                    glBufferSubData(GL_ARRAY_BUFFER, reg_asset.asset.offsets.pos_offset, reg_asset.asset.vertices.v2d.mer.n * 
                        sizeof(das_GL2DVertex), reg_asset.asset.vertices.v2d.mer.vert);
                    break;

                case DAS_ASSET_MODE_3D_UNMAPPED:
                    glBufferSubData(GL_ARRAY_BUFFER, reg_asset.asset.offsets.pos_offset, reg_asset.asset.vertices.v3d.mer.n * 
                        sizeof(das_GL3DVertexUnmapped), reg_asset.asset.vertices.v3d.mer.uvert);
                    break;

                case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                    LOG("Copying " + std::to_string(reg_asset.asset.vertices.v3d.mer.n) + " vertices to the buffer");
                    glBufferSubData(GL_ARRAY_BUFFER, reg_asset.asset.offsets.pos_offset, reg_asset.asset.vertices.v3d.mer.n * 
                        sizeof(das_GL3DVertex), reg_asset.asset.vertices.v3d.mer.vert);
                    break;

                default:
                    break;
                }

                // LOG all indices to FILE
                cm_OpenLogger("ind.log");
                for(deng_ui64_t i = 0; i < reg_asset.asset.indices.n; i++) {
                    char buf[128] = { 0 };
                    sprintf(buf, "%u", reg_asset.asset.indices.gl[i]);
                    cm_LogWrite(buf);
                }
                cm_CloseLogger();

                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, reg_asset.asset.offsets.ind_offset, 
                    reg_asset.asset.indices.n * sizeof(deng_idx_t), reg_asset.asset.indices.gl);
                glErrorCheck("glBufferSubData");
            }
        }


        /// Copy ImGui vertex and indices data to buffer
        /// NOTE: The UI element capacity has to be larger than required UI element size (use uiCapCheck() for this)
        void __gl_BufferManager::cpyUIDataToBuffer(const deng_bool_t no_offset_calc) {
            // If no imgui pointer is specified, return
            if(!m_p_imgui_data || !m_p_imgui_data->cmd_data.size()) return;

            // Check if offsets must be calculated
            if(!no_offset_calc) __OffsetFinder::__findGuiEntitiesOffsets(true);

            // For each gui entity copy its data to buffer
            for(deng_ui64_t i = 0; i < m_p_imgui_data->cmd_data.size(); i++) {
                // Vertices
                glBufferSubData(GL_ARRAY_BUFFER, m_p_imgui_data->cmd_data[i].voffset, m_p_imgui_data->cmd_data[i].vert_c * sizeof(ImDrawVert),
                    m_p_imgui_data->cmd_data[i].verts);

                // Indices
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, m_p_imgui_data->cmd_data[i].ioffset, m_p_imgui_data->cmd_data[i].ind_c *
                    sizeof(ImDrawIdx), m_p_imgui_data->cmd_data[i].ind);
                glErrorCheck("glBufferSubData");
            }
        }


        __gl_Resources &__gl_BufferManager::getResources() {
            return m_resources;
        }


        void __gl_BufferManager::setUIDataPtr(__ImGuiData *p_gui) {
            m_p_imgui_data = p_gui;
        }
    }
}
