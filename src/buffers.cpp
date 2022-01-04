/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: buffers.h - OpenGL buffer manager class implementation
/// author: Karl-Mihkel Ott

#define __GL_BUFFERS_CPP
#include <deng/opengl/buffers.h>


namespace deng {
    namespace opengl {

        __gl_BufferManager::__gl_BufferManager(
            std::vector<deng_Id>& assets,
            std::shared_ptr<__gl_Pipelines> pipelines,
            Registry& reg,
            void (*lgl_error_check)(const std::string& func_name, const std::string& file, const deng_ui32_t line)
        ) : __gl_UniformManager(reg, assets, lgl_error_check, pipelines->getShaderPrograms()), m_assets(assets), m_reg(reg), 
            m_resources(__gl_UniformManager::getResources()),
            m_pipelines(pipelines), lglErrorCheck(lgl_error_check), m_programs(pipelines->getShaderPrograms())
        {
            // Generate and bind new vertex array handles
            glGenVertexArrays(1, &m_resources.vert_array);
            glErrorCheck("glGenVertexArrays");
            glBindVertexArray(m_resources.vert_array);
            glErrorCheck("glBindVertexArray");

            // Generate new buffers
            GLuint buffers[2];
            glGenBuffers(2, buffers);
            glErrorCheck("glGenBuffers");
            m_resources.vert_buffer = buffers[0];
            m_resources.idx_buffer = buffers[1];

            // Bind all buffers handles with their corresponding buffer types
            glBindBuffer(GL_ARRAY_BUFFER, m_resources.vert_buffer);
            glErrorCheck("glBindBuffer");

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_resources.idx_buffer);
            glErrorCheck("glBindBuffer");

            // Find all asset offsets
            __OffsetFinder::getSectionInfo().asset_size = 0;
            for(deng_ui64_t i = 0; i < m_assets.size(); i++) {
                RegData &reg_asset = m_reg.retrieve(m_assets[i], DENG_REGISTRY_TYPE_ASSET, NULL);
                __OffsetFinder::__findAssetOffsets(reg_asset.asset);
            }


            // Set some initial buffer capacity values
            deng::BufferSectionInfo& bsec = __OffsetFinder::getSectionInfo();
            bsec.asset_cap = DENG_DEF_ASSET_CAP;
            bsec.indices_cap = DENG_DEF_ASSET_CAP;
            bsec.ui_cap = DENG_DEF_UI_CAP;

            // Allocate the initial amount of memory for buffers
            allocateBufferMemory();
        }


        __gl_BufferManager::~__gl_BufferManager() {
            LOG("Destroying __gl_BufferManager class");
        }


        /// Allocate ui_cap + asset_cap amount of memory for the buffer
        void __gl_BufferManager::allocateBufferMemory() {
            // Allocate memory for vertex buffer
            glBufferData(GL_ARRAY_BUFFER, __OffsetFinder::getSectionInfo().asset_cap + __OffsetFinder::getSectionInfo().ui_cap, NULL, GL_STATIC_DRAW);
            glErrorCheck("glBufferData");

            // Allocate memory for indices buffer
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, __OffsetFinder::getSectionInfo().indices_cap, NULL, GL_STATIC_DRAW);
            glErrorCheck("glBufferData");
        }


        /// Check if buffer reallocation is needed for assets and gui elements
        deng_bool_t __gl_BufferManager::reallocCheck() {
            __OffsetFinder::__calcOffsets(true);
            const deng_bool_t asset_realloc = __OffsetFinder::assetCapCheck();
            const deng_bool_t indices_realloc = __OffsetFinder::indicesCapCheck();
            const deng_bool_t ui_realloc = __OffsetFinder::uiCapCheck();

            if(asset_realloc || indices_realloc || ui_realloc) {
                allocateBufferMemory();
                // For each asset copy its data to buffers
                for(size_t i = 0; i < m_assets.size(); i++)
                    cpyAssetToBuffer(m_assets[i]);

                if(m_p_imgui_data) cpyUIDataToBuffer(true);
            }

            return asset_realloc || ui_realloc;
        }


        /// Copy all asset data between given bounds to buffer
        /// NOTE: The asset capacity in buffer has to be larger than required asset size (use assetCapCheck() for this)
        void __gl_BufferManager::cpyAssetToBuffer(const deng_Id id) {
            RegData &reg_asset = m_reg.retrieve(id, DENG_REGISTRY_TYPE_ASSET, NULL);
            void *data;

            // Check the asset type and bind data accordingly
            switch(reg_asset.asset.asset_mode) {
            case DAS_ASSET_MODE_2D_UNMAPPED:
                data = glMapBufferRange(GL_ARRAY_BUFFER, reg_asset.asset.offsets.pos_offset, reg_asset.asset.vertices.v2d.mer.n * sizeof(das_GL2DVertexUnmapped), 
                                        GL_MAP_WRITE_BIT);
                glErrorCheck("glMapBufferRange");
                memcpy(data, reg_asset.asset.vertices.v2d.mer.uvert, reg_asset.asset.vertices.v2d.mer.n * sizeof(das_GL2DVertexUnmapped));
                glUnmapBuffer(GL_ARRAY_BUFFER);
                glErrorCheck("glUnmapBuffer");
                break;

            case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                data = glMapBufferRange(GL_ARRAY_BUFFER, reg_asset.asset.offsets.pos_offset, reg_asset.asset.vertices.v2d.mer.n * sizeof(das_GL2DVertex), 
                                        GL_MAP_WRITE_BIT);
                glErrorCheck("glMapBufferRange");
                memcpy(data, reg_asset.asset.vertices.v2d.mer.vert, reg_asset.asset.vertices.v2d.mer.n * sizeof(das_GL2DVertex));
                glUnmapBuffer(GL_ARRAY_BUFFER);
                glErrorCheck("glUnmapBuffer");
                break;

            case DAS_ASSET_MODE_3D_UNMAPPED:
                data = glMapBufferRange(GL_ARRAY_BUFFER, reg_asset.asset.offsets.pos_offset, reg_asset.asset.vertices.v3d.mer.n * sizeof(das_GL3DVertexUnmapped),
                                        GL_MAP_WRITE_BIT);
                glErrorCheck("glMapBufferRange");
                memcpy(data, reg_asset.asset.vertices.v3d.mer.uvert, reg_asset.asset.vertices.v3d.mer.n * sizeof(das_GL3DVertexUnmapped));
                glUnmapBuffer(GL_ARRAY_BUFFER);
                glErrorCheck("glUnmapBuffer");
                break;

            case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                data = glMapBufferRange(GL_ARRAY_BUFFER, reg_asset.asset.offsets.pos_offset, reg_asset.asset.vertices.v3d.mer.n * sizeof(das_GL3DVertex),
                                        GL_MAP_WRITE_BIT);
                glErrorCheck("glMapBufferRange");
                memcpy(data, reg_asset.asset.vertices.v3d.mer.vert, reg_asset.asset.vertices.v3d.mer.n * sizeof(das_GL3DVertex));
                glUnmapBuffer(GL_ARRAY_BUFFER);
                glErrorCheck("glUnmapBuffer");
                break;

            default:
                break;
            }

            data = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, reg_asset.asset.offsets.ind_offset, reg_asset.asset.indices.n * sizeof(deng_idx_t), 
                                    GL_MAP_WRITE_BIT);
            glErrorCheck("glMapBufferRange");
            memcpy(data, reg_asset.asset.indices.gl, reg_asset.asset.indices.n * sizeof(deng_idx_t));
            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
            glErrorCheck("glUnmapBuffer");

            mapUniformBufferArea(reg_asset.asset);
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
                glErrorCheck("glBufferSubData");

                // Indices
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, m_p_imgui_data->cmd_data[i].ioffset, m_p_imgui_data->cmd_data[i].ind_c *
                    sizeof(ImDrawIdx), m_p_imgui_data->cmd_data[i].ind);
                glErrorCheck("glBufferSubData");
            }
        }


        /// Enable and set vertex attribute pointers according to the specified asset
        void __gl_BufferManager::bindAssetDataBufferRegion(const das_Asset &asset) {
            switch(asset.asset_mode) {
            case DAS_ASSET_MODE_2D_UNMAPPED:
                glEnableVertexAttribArray(0);
                glErrorCheck("glEnableVertexAttribArray");

                glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, sizeof(das_ObjPosData2D), (const void*) 0);
                glErrorCheck("glVertexAttribPointer");
                break;

            case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                glEnableVertexAttribArray(0);
                glErrorCheck("glEnableVertexAttribArray");

                glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, sizeof(das_ObjPosData2D), (const void*) offsetof(das_GL2DVertex, pos));
                glErrorCheck("glVertexAttribPointer");

                glEnableVertexAttribArray(1);
                glErrorCheck("glEnableVertexAttribArray");

                glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(das_ObjTextureData), (const void*) offsetof(das_GL2DVertex, tex));
                glErrorCheck("glVertexAttribPointer");
                break;

            case DAS_ASSET_MODE_3D_UNMAPPED:
                glEnableVertexAttribArray(0);
                glErrorCheck("glEnableVertexAttribArray");

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(das_GL3DVertexUnmapped), (const void*) offsetof(das_GL3DVertexUnmapped, pos));
                glErrorCheck("glVertexAttribPointer");

                glEnableVertexAttribArray(1);
                glErrorCheck("glEnableVertexAttribArray");

                glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(das_GL3DVertexUnmapped), (const void*) offsetof(das_GL3DVertexUnmapped, norm));
                glErrorCheck("glVertexAttribPointer");
                break;

            case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                // Enable position vertices
                glEnableVertexAttribArray(0);
                glErrorCheck("glEnableVertexAttribArray");

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(das_GL3DVertex), (const void*) offsetof(das_GL3DVertex, pos));
                glErrorCheck("glVertexAttribPointer");

                glEnableVertexAttribArray(1);
                glErrorCheck("glEnableVertexAttribArray");

                glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(das_GL3DVertex), (const void*) offsetof(das_GL3DVertex, tex));
                glErrorCheck("glVertexAttribPointer");

                glEnableVertexAttribArray(2);
                glErrorCheck("glEnableVertexAttribArray");
                
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(das_GL3DVertex), (const void*) offsetof(das_GL3DVertex, norm));
                glErrorCheck("glVertexAttribPointer");
                break;

            default:
                break;
            }
        }


        /// Disable vertex attributes for specified asset
        void __gl_BufferManager::unbindAssetDataBufferRegion(const das_Asset &asset) {
            glDisableVertexAttribArray(0);
            glErrorCheck("glDisableVertexAttribArray");

            switch(asset.asset_mode) {
            case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
            case DAS_ASSET_MODE_3D_UNMAPPED:
                glDisableVertexAttribArray(1);
                glErrorCheck("glDisableVertexAttribArray");
                break;

            case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                glDisableVertexAttribArray(2);
                glErrorCheck("glDisableVertexAttribArray");
                break;

            default:
                break;
            }

        }


        __gl_Resources __gl_BufferManager::getResources() {
            return m_resources;
        }


        void __gl_BufferManager::setUIDataPtr(__ImGuiData *p_gui) {
            m_p_imgui_data = p_gui;
        }
    }
}
