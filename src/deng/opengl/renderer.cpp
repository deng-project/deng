/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: renderer.h - OpenGL renderer class implementation
/// author: Karl-Mihkel Ott


#define __GL_RENDERER_CPP
#include <deng/opengl/renderer.h>


namespace deng {
    namespace opengl {

        __gl_Renderer::__gl_Renderer (
            __gl_ConfigVars &cfg,
            deng::Registry &reg,
            std::vector<deng_Id> &assets,
            std::vector<deng_Id> &textures
        ) : m_reg(reg), m_assets(assets), m_textures(textures) {
            m_cfg_vars = cfg;
            
            // Load all OpenGL functions
            int status = deng_LoadGL();
            DENG_ASSERT("Failed to load OpenGL functions", status);

            // Enable some OpenGL features
            glEnable(GL_PROGRAM_POINT_SIZE);
            glErrorCheck("glEnable");
            glEnable(GL_DEPTH_TEST);
            glErrorCheck("glEnable");
            glEnable(GL_STENCIL_TEST);
            glErrorCheck("glEnable");

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glErrorCheck("glTexParameteri");

            // Load all shaders into OpenGL
            m_pipelines = std::make_shared<__gl_Pipelines>(lglErrorCheck);
            m_buf_manager = std::make_unique<__gl_BufferManager>(m_assets, m_pipelines, m_reg, lglErrorCheck);


            glDepthFunc(GL_LESS);
            glErrorCheck("glDepthFunc");
        }

        __gl_Renderer::~__gl_Renderer() {
            LOG("Destroying __gl_Renderer class");
        }


        /// OpenGL error checking function
        void __gl_Renderer::lglErrorCheck(const std::string &func_name, const std::string &file, const deng_ui32_t line ) {
            GLenum error;
            while((error = glGetError()) != GL_NO_ERROR) {
                switch(error) {
                case GL_INVALID_ENUM:
                    std::cerr << func_name << "(); " << file << ", " << line << ": GL_INVALID_ENUM " << std::endl;
                    break;

                case GL_INVALID_VALUE:
                    std::cerr << func_name << "(); " << file << ", " << line << " GL_INVALID_VALUE " << std::endl;
                    break;

                case GL_INVALID_OPERATION:
                    std::cerr << func_name << "(); " << file << ", " << line << " GL_INVALID_OPERATION " << std::endl;
                    break;

                case GL_STACK_OVERFLOW:
                    std::cerr << func_name << "(); " << file << ", " << line << " GL_STACK_OVERFLOW " << std::endl;
                    break;

                case GL_STACK_UNDERFLOW:
                    std::cerr << func_name << "(); " << file << ", " << line << " GL_STACK_UNDERFLOW " << std::endl;
                    break;

                case GL_OUT_OF_MEMORY:
                    std::cerr << func_name << "(); " << file << ", " << line << " GL_OUT_OF_MEMORY " << std::endl;
                    break;

                default:
                    break;
                }
                std::abort();
            }
        }

        const GLenum __gl_Renderer::__textureToUnit(const __gl_Texture &tex) {
            return GL_TEXTURE0 + (GLenum) tex.unit_nr;
        }


        void __gl_Renderer::setLighting(std::array<deng_Id, __DENG_MAX_LIGHT_SRC_COUNT> &light_srcs) {
            m_buf_manager->updateUboLighting(light_srcs);
        }


        void __gl_Renderer::prepareAsset(const deng_Id id) {
            // For each asset between bounds, merge index buffers
            RegData &reg_asset = m_reg.retrieve(id, DENG_REGISTRY_TYPE_ASSET, NULL);
            das_MergeIndexBuffers(&reg_asset.asset);
            reg_asset.asset.is_opengl = true;

            /// BUG: Whenever there is a reallocation occuring for the new asset, the offsets are not calculated, thus
            /// corrupting the vertex data in buffer. 
            /// SOLUTION: This can be solved by creating additional offset calculation boolean flag for the reallocCheck() method
            
            // Check if any reallocations should occur
            const deng_bool_t is_realloc = m_buf_manager->reallocCheck();
            if(!is_realloc) m_buf_manager->cpyAssetToBuffer(id);
        }


        /// Prepare texture for OpenGL usage
        void __gl_Renderer::prepareTexture(const deng_Id id) {
            RegData &reg_tex = m_reg.retrieve(id, DENG_REGISTRY_TYPE_TEXTURE, NULL);

            RegData reg_gl_tex = {};
            reg_gl_tex.gl_tex.base_id = id;
            reg_gl_tex.gl_tex.unit_nr = static_cast<deng_gl_t>(m_reg.getItemsByType(DENG_REGISTRY_TYPE_GL_TEXTURE).size());

            // Check if the unit_nr does not exceed GL_MAX_TEXTURE_IMAGE_UNITS
            GLint max_units;
            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_units);
            DENG_ASSERT("Current texture image index exceeds GL_MAX_TEXTURE_IMAGE_UNITS limit", max_units > (GLint) reg_gl_tex.gl_tex.unit_nr);

            // Console logging
            LOG("Preparing OpenGL texture with unit nr of " + std::to_string(reg_gl_tex.gl_tex.unit_nr));
            LOG("Maximum number of texture units is " + std::to_string(max_units));

            reg_gl_tex.gl_tex.uuid = uuid_Generate();
            reg_tex.tex.gl_id = reg_gl_tex.gl_tex.uuid;

            glBindTexture(GL_TEXTURE_2D, reg_gl_tex.gl_tex.gl_id);
            glErrorCheck("glBindTexture");

            glGenTextures(1, &reg_gl_tex.gl_tex.gl_id);
            glErrorCheck("glGenTextures");

            GLint max_t;
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_t);
            DENG_ASSERT("Texture width for texture " + std::string(reg_tex.tex.src) + " exceeds GL_MAX_TEXTURE_SIZE limit (" + std::to_string(max_t) + ")", max_t > reg_tex.tex.pixel_data.width);
            DENG_ASSERT("Texture height for texture " + std::string(reg_tex.tex.src) + " exceeds GL_MAX_TEXTURE_SIZE limit (" + std::to_string(max_t) + ")", max_t > reg_tex.tex.pixel_data.height);

            // Copy image data into texture object
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(reg_tex.tex.pixel_data.width), static_cast<GLsizei>(reg_tex.tex.pixel_data.height),
                0, GL_BGRA, GL_UNSIGNED_BYTE, reg_tex.tex.pixel_data.pixel_data);
            glErrorCheck("glTexImage2D");
            glGenerateMipmap(GL_TEXTURE_2D);
            glErrorCheck("glGenerateMipmap");

            // Push the OpenGL texture to registry
            m_reg.push(reg_gl_tex.gl_tex.uuid, DENG_REGISTRY_TYPE_GL_TEXTURE, reg_gl_tex);
        }


        /// Overwrite asset vertices and indices data in buffer
        void __gl_Renderer::updateAssetData(const dengMath::vec2<deng_ui32_t> &bounds) {
            const deng_bool_t is_realloc = m_buf_manager->reallocCheck();
            if(!is_realloc) {
                for(deng_ui32_t i = bounds.first; i < bounds.second; i++)
                    m_buf_manager->cpyAssetToBuffer(m_assets[i]);
            }
        }


        /// Overwrite all UI data in buffer
        void __gl_Renderer::updateUIData() {
            const deng_bool_t is_realloc = m_buf_manager->reallocCheck();
            if(!is_realloc) m_buf_manager->cpyUIDataToBuffer(false);
        }


        /// Check if any buffer reallocations are necessary
        void __gl_Renderer::checkForBufferReallocation() {
            m_buf_manager->reallocCheck();
        }


        /// Main frame updating function
        void __gl_Renderer::makeFrame() {
            glViewport(0, 0, m_cfg_vars.p_win->getSize().first, m_cfg_vars.p_win->getSize().second);
            glErrorCheck("glViewport");
            glClearColor(m_cfg_vars.background.first, m_cfg_vars.background.second, m_cfg_vars.background.third,
                m_cfg_vars.background.fourth);

            glErrorCheck("glClearColor");
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glErrorCheck("glClear");
            
			glBindVertexArray(m_buf_manager->getResources().vert_array);
			glErrorCheck("glBindVertexArray");

            // For each asset draw it to the screen
            for(deng_ui64_t i = 0; i < m_assets.size(); i++) {
                RegData &reg_asset = m_reg.retrieve(m_assets[i], DENG_REGISTRY_TYPE_ASSET, NULL);

                // Determine which shader program to use for given draw call
                switch (reg_asset.asset.asset_mode) {
                case DAS_ASSET_MODE_2D_UNMAPPED:
                    glUseProgram(m_pipelines->getShaderProgram(UM2D_I));
                    break;

                case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                    glUseProgram(m_pipelines->getShaderProgram(TM2D_I));
                    break;
                
                case DAS_ASSET_MODE_3D_UNMAPPED:
                    glUseProgram(m_pipelines->getShaderProgram(UM3D_I));
                    break;

                case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                    glUseProgram(m_pipelines->getShaderProgram(TM3D_I));
                    break;

                default:
                    break;
                }
                glErrorCheck("glUseProgram");

                // Draw asset by its elements to the screen
                m_buf_manager->updateUboTransform3D(m_cfg_vars.p_cam);
                m_buf_manager->bindAssetDataBufferRegion(reg_asset.asset);
                m_buf_manager->bindAssetUboBufferRange(reg_asset.asset);

                // Check if texture must be bound
                if((reg_asset.asset.asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED || 
                    reg_asset.asset.asset_mode == DAS_ASSET_MODE_2D_TEXTURE_MAPPED) && !reg_asset.asset.force_unmap) {
                   RegData &reg_tex = m_reg.retrieve(reg_asset.asset.tex_uuid, DENG_REGISTRY_TYPE_TEXTURE, NULL);
                   RegData &reg_gl_tex = m_reg.retrieve(reg_tex.tex.gl_id, DENG_REGISTRY_TYPE_GL_TEXTURE, NULL);

                   // Console logging
                   glActiveTexture(__textureToUnit(reg_gl_tex.gl_tex));
                   glBindTexture(GL_TEXTURE_2D, reg_gl_tex.gl_tex.gl_id);
                   glErrorCheck("glBindTexture");

                }

                glDrawElements(GL_TRIANGLES, reg_asset.asset.indices.n, GL_UNSIGNED_INT, (void*) reg_asset.asset.offsets.ind_offset);
                glErrorCheck("glDrawElements");

                m_buf_manager->unbindAssetDataBufferRegion(reg_asset.asset);
            }
        }

        /// Setter and getter methods
        void __gl_Renderer::setUIDataPtr(__ImGuiData *p_data) {
            m_p_imgui_data = p_data;
            m_buf_manager->setUIDataPtr(p_data);
        }


        const deng_bool_t __gl_Renderer::isInit() {
            return m_is_init;
        }
    }
}
