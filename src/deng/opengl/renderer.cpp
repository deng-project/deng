/// DENG: dynamic engine - powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: renderer.h - OpenGL renderer class implementation
/// author: Karl-Mihkel Ott


#define __GL_RENDERER_CPP
#include <deng/opengl/renderer.h>


namespace deng {
    namespace opengl {

        __gl_Renderer::__gl_Renderer (
            __gl_ConfigVars &cfg,
            deng::__GlobalRegistry &reg,
            std::vector<deng_Id> &assets,
            std::vector<deng_Id> &textures
        ) : m_reg(reg), m_assets(assets), m_textures(textures) {
            m_cfg_vars = cfg;
            
            // Load all OpenGL functions
            int status = neko_LoadGL();
            DENG_ASSERT("Failed to load OpenGL functions", status);

            // Load all shaders into OpenGL
            m_pipelines = std::make_shared<__gl_Pipelines>(glErrorCheck);
            m_buf_manager = std::make_unique<__gl_BufferManager>(m_assets, m_pipelines, m_reg, __gl_Renderer::glErrorCheck);

            // Enable some OpenGL features
            glEnable(GL_PROGRAM_POINT_SIZE);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_STENCIL_TEST);
            __gl_Renderer::glErrorCheck("glEnable");

            glDepthFunc(GL_LESS);
            __gl_Renderer::glErrorCheck("glDepthFunc");

            // Set texture parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            __gl_Renderer::glErrorCheck("glTexParameteri");
        }


        /// OpenGL error checking function
        void __gl_Renderer::glErrorCheck(const std::string &func_name) {
            GLenum error;
            while((error = glGetError()) != GL_NO_ERROR) {
                switch(error) {
                case GL_INVALID_ENUM:
                    std::cerr << func_name << "(): GL_INVALID_ENUM " << std::endl;
                    break;

                case GL_INVALID_VALUE:
                    std::cerr << func_name << "(): GL_INVALID_VALUE " << std::endl;
                    break;

                case GL_INVALID_OPERATION:
                    std::cerr << func_name << "(): GL_INVALID_OPERATION " << std::endl;
                    break;

                case GL_STACK_OVERFLOW:
                    std::cerr << func_name << "(): GL_STACK_OVERFLOW " << std::endl;
                    break;

                case GL_STACK_UNDERFLOW:
                    std::cerr << func_name << "(): GL_STACK_UNDERFLOW " << std::endl;
                    break;

                case GL_OUT_OF_MEMORY:
                    std::cerr << func_name << "(): GL_OUT_OF_MEMORY " << std::endl;
                    break;

                default:
                    break;
                }
                exit(-1);
            }
        }


        void __gl_Renderer::setLighting(std::array<deng_Id, __DENG_MAX_LIGHT_SRC_COUNT> &light_srcs) {
            m_buf_manager->updateUboLighting(light_srcs);
        }


        void __gl_Renderer::prepareAssets(const dengMath::vec2<deng_ui32_t> &bounds) {
            // For each asset between bounds, merge index buffers
            for(deng_ui32_t i = bounds.first; i < bounds.second; i++) {
                RegType &reg_asset = m_reg.retrieve(m_assets[i], DENG_SUPPORTED_REG_TYPE_ASSET, NULL);
                das_MergeIndexBuffers(&reg_asset.asset);
                reg_asset.asset.is_opengl = true;
            }

            // Check if any reallocations should occur
            const deng_bool_t is_realloc = m_buf_manager->reallocCheck();
            if(!is_realloc) m_buf_manager->cpyAssetsToBuffer(false, bounds);
        }


        /// Prepare texture for OpenGL usage
        void __gl_Renderer::prepareTexture(const deng_Id id) {
            RegType &reg_tex = m_reg.retrieve(id, DENG_SUPPORTED_REG_TYPE_TEXTURE, NULL);

            RegType reg_gl_tex = {};
            reg_gl_tex.gl_tex.base_id = id;
            reg_gl_tex.gl_tex.uuid = uuid_Generate();
            reg_tex.tex.gl_id = reg_gl_tex.gl_tex.uuid;

            glGenTextures(1, &reg_gl_tex.gl_tex.gl_id);
            __gl_Renderer::glErrorCheck("glGenTextures");

            glBindTexture(GL_TEXTURE_2D, reg_gl_tex.gl_tex.gl_id);
            __gl_Renderer::glErrorCheck("glBindTexture");

            // Copy image data into texture object
            glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, reg_tex.tex.pixel_data.width, reg_tex.tex.pixel_data.height,
                0, GL_RGBA, GL_UNSIGNED_BYTE, reg_tex.tex.pixel_data.pixel_data);
            __gl_Renderer::glErrorCheck("glTexImage2D");
            glGenerateMipmap(GL_TEXTURE_2D);
            __gl_Renderer::glErrorCheck("glGenerateMipmap");

            // Push the OpenGL texture to registry
            m_reg.push(reg_gl_tex.gl_tex.uuid, DENG_SUPPORTED_REG_TYPE_GL_TEXTURE, reg_gl_tex);
        }


        /// Main frame updating function
        void __gl_Renderer::makeFrame() {
            glViewport(0, 0, m_cfg_vars.p_win->getSize().first, m_cfg_vars.p_win->getSize().second);
            __gl_Renderer::glErrorCheck("glViewport");
            glClearColor(m_cfg_vars.background.first, m_cfg_vars.background.second, m_cfg_vars.background.third,
                m_cfg_vars.background.fourth);
            __gl_Renderer::glErrorCheck("glClearColor");
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            __gl_Renderer::glErrorCheck("glClear");
            

            // For each asset draw it to the screen
            LOG("Asset count " + std::to_string(m_assets.size()));
            for(deng_ui64_t i = 0; i < m_assets.size(); i++) {
                glBindVertexArray(m_buf_manager->getResources().vert_array);
                __gl_Renderer::glErrorCheck("glBindVertexArray");
                RegType &reg_asset = m_reg.retrieve(m_assets[i], DENG_SUPPORTED_REG_TYPE_ASSET, NULL);

                // Draw asset by its elements to the screen
                m_pipelines->setAssetVertAttributes(reg_asset.asset);
                m_buf_manager->updateUboTransform3D(m_cfg_vars.p_cam);

                // Check if texture must be bound
                if((reg_asset.asset.asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED || 
                  reg_asset.asset.asset_mode == DAS_ASSET_MODE_2D_TEXTURE_MAPPED) && !reg_asset.asset.force_unmap) {
                   RegType &reg_tex = m_reg.retrieve(reg_asset.asset.tex_uuid, DENG_SUPPORTED_REG_TYPE_TEXTURE, NULL);
                   RegType &reg_gl_tex = m_reg.retrieve(reg_tex.tex.gl_id, DENG_SUPPORTED_REG_TYPE_GL_TEXTURE, NULL);
                   //glBindTexture(GL_TEXTURE_2D, reg_gl_tex.gl_tex.gl_id);
                }

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buf_manager->getResources().idx_buffer);
                __gl_Renderer::glErrorCheck("glBindBuffer");
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*) reg_asset.asset.offsets.ind_offset);
                __gl_Renderer::glErrorCheck("glDrawElements");
                m_pipelines->disableAssetVertAttributes(reg_asset.asset);
            }
        }
    }
}
