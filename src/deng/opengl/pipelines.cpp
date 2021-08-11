/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: pipelines.cpp - OpenGL pipeline handler class implementation
/// author: Karl-Mihkel Ott


#define __GL_PIPELINES_CPP
#include <deng/opengl/pipelines.h>


namespace deng {
    namespace opengl {

        __gl_Pipelines::__gl_Pipelines(void (*lgl_error_check)(const std::string& func_name, const std::string& file, const deng_ui32_t line)) :
            lglErrorCheck(lgl_error_check)
        {
            // For each pipeline type create a set of shader objects
            __compileShadersToProgram(UM3D_I);
            __compileShadersToProgram(TM3D_I);
            __compileShadersToProgram(UM2D_I);
            __compileShadersToProgram(TM2D_I);
            __prepareUniformBindings();
        }


        void __gl_Pipelines::__compileShadersToProgram(const deng_ui32_t index) {
            dengMath::vec2<deng_gl_t> shaders = {};

            shaders.first = glCreateShader(GL_VERTEX_SHADER);
            shaders.second = glCreateShader(GL_FRAGMENT_SHADER);
            glErrorCheck("glCreateShader", __FILE__, __LINE__);

            // Load shader sources
            char *vert = __loadShaderFromFile(__shader_src_files[index][0]);
            char *frag = __loadShaderFromFile(__shader_src_files[index][1]);

            deng_i32_t len;
            len = strlen(vert);
            glShaderSource(shaders.first, 1, &vert, &len);
            glErrorCheck("glShaderSource", __FILE__, __LINE__);
            len = strlen(frag);
            glShaderSource(shaders.second, 1, &frag, &len);
            glErrorCheck("glShaderSource", __FILE__, __LINE__);
            
            /// Attempt to compile shaders
            glCompileShader(shaders.first);
            __checkCompileStatus(shaders.first, __shader_src_files[index][0]);
            glCompileShader(shaders.second);
            __checkCompileStatus(shaders.second, __shader_src_files[index][1]);

            free(vert);
            free(frag);

            // Create new shader programs for each pipeline type
            m_programs[index] = glCreateProgram();
            glErrorCheck("glCreateProgram", __FILE__, __LINE__);
            glAttachShader(m_programs[index], shaders.first);
            glErrorCheck("glAttachShader", __FILE__, __LINE__);
            glAttachShader(m_programs[index], shaders.second);
            glErrorCheck("glAttachShader", __FILE__, __LINE__);

            glLinkProgram(m_programs[index]);
            glErrorCheck("glLinkProgram", __FILE__, __LINE__);

            __checkLinkingStatus(m_programs[index], static_cast<deng_ui32_t>(index));

            // Delete all shader objects
            glDeleteShader(shaders.first);
            glErrorCheck("glDeleteShader", __FILE__, __LINE__);
            glDeleteShader(shaders.second);
            glErrorCheck("glDeleteShader", __FILE__, __LINE__);
        }


        /// Prepare bindings for uniform data
        void __gl_Pipelines::__prepareUniformBindings() {
            // 2D unmapped assets
            deng_ui32_t index;
            index = glGetUniformBlockIndex(m_programs[UM2D_I], "UniformData");
            glErrorCheck("glGetUniformBlockIndex", __FILE__, __LINE__);
            glUniformBlockBinding(m_programs[UM2D_I], index, 0);
            glErrorCheck("glUniformBlockBinding", __FILE__, __LINE__);

            index = glGetUniformBlockIndex(m_programs[UM2D_I], "ColorData");
            glErrorCheck("glGetUniformBlockIndex", __FILE__, __LINE__);
            glUniformBlockBinding(m_programs[UM2D_I], index, 1);
            glErrorCheck("glUniformBlockBinding", __FILE__, __LINE__);

            // 2D texture mapped assets
            index = glGetUniformBlockIndex(m_programs[TM2D_I], "UniformData");
            glErrorCheck("glGetUniformBlockIndex", __FILE__, __LINE__);
            glUniformBlockBinding(m_programs[TM2D_I], index, 0);
            glErrorCheck("glUniformBlockBinding", __FILE__, __LINE__);

            index = glGetUniformBlockIndex(m_programs[TM2D_I], "ColorData");
            glErrorCheck("glGetUniformBlockIndex", __FILE__, __LINE__);
            glUniformBlockBinding(m_programs[TM2D_I], index, 1);
            glErrorCheck("glUniformBlockBinding", __FILE__, __LINE__);

            // 3D unmapped assets
            index = glGetUniformBlockIndex(m_programs[UM3D_I], "UboTransform");
            glErrorCheck("glGetUniformBlockIndex", __FILE__, __LINE__);
            glUniformBlockBinding(m_programs[UM3D_I], index, 0);
            glErrorCheck("glUniformBlockBinding", __FILE__, __LINE__);

            index = glGetUniformBlockIndex(m_programs[UM3D_I], "AssetData");
            glErrorCheck("glGetUniformBlockIndex", __FILE__, __LINE__);
            glUniformBlockBinding(m_programs[UM3D_I], index, 1);
            glErrorCheck("glUniformBlockBinding", __FILE__, __LINE__);

            index = glGetUniformBlockIndex(m_programs[UM3D_I], "LightData");
            glErrorCheck("glGetUniformBlockIndex", __FILE__, __LINE__);
            glUniformBlockBinding(m_programs[UM3D_I], index, 2);
            glErrorCheck("glUniformBlockBinding", __FILE__, __LINE__);

            // 3D texture mapped assets
            index = glGetUniformBlockIndex(m_programs[TM3D_I], "UboTransform");
            glErrorCheck("glGetUniformBlockIndex", __FILE__, __LINE__);
            glUniformBlockBinding(m_programs[TM3D_I], index, 0);
            glErrorCheck("glUniformBlockBinding", __FILE__, __LINE__);

            index = glGetUniformBlockIndex(m_programs[TM3D_I], "AssetData");
            glErrorCheck("glGetUniformBlockIndex", __FILE__, __LINE__);
            glUniformBlockBinding(m_programs[TM3D_I], index, 1);
            glErrorCheck("glUniformBlockBinding", __FILE__, __LINE__);

            index = glGetUniformBlockIndex(m_programs[TM3D_I], "LightData");
            glErrorCheck("glGetUniformBlockIndex", __FILE__, __LINE__);
            glUniformBlockBinding(m_programs[TM3D_I], index, 2);
            glErrorCheck("glUniformBlockBinding", __FILE__, __LINE__);
        }


        /// Load shader data from file to a buffer
        char *__gl_Pipelines::__loadShaderFromFile(const char *file_name) {
            FILE *file = fopen(file_name, "rb");
            DENG_ASSERT((std::string("Failed to open file stream for file name: ") + std::string(file_name)).c_str(), file);
        
            // Find the file size
            fseek(file, 0, SEEK_END);
            size_t buf_size = ftell(file);
            fseek(file, 0, SEEK_SET);

            // Allocate memory for shader buffer
            char *buf = (char*) calloc(buf_size + 1, sizeof(char));
            size_t res = fread(buf, sizeof(char), buf_size, file);
            DENG_ASSERT((std::string("Failed to read from file ") + std::string(file_name)).c_str(), res);

            return buf;
        }


        /// Check if the shader compilation was successful
        void __gl_Pipelines::__checkCompileStatus(const deng_ui32_t shader_id, const char *file_name) {
            int success;
            char log[512] = { 0 };
            glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
            
            if(!success) {
                glGetShaderInfoLog(shader_id, 512, NULL, log);
                LOG("Shader compilation error in file: " + std::string(file_name));
                LOG(std::string(log));
                exit(EXIT_FAILURE);
            }
        }


        /// Check if shader program linking was successful
        void __gl_Pipelines::__checkLinkingStatus(const deng_ui32_t program_id, const deng_ui32_t program_index) {
            int success;
            char log[512];
            glGetProgramiv(program_id, GL_LINK_STATUS, &success);

            if(!success) {
                glGetProgramInfoLog(program_id, 512, NULL, log);
                LOG("Failed to link " + std::string(__shader_program_names[program_index]));
                LOG(std::string(log));
                exit(EXIT_FAILURE);
            }
        }


        const deng_ui32_t __gl_Pipelines::getShaderProgram(const deng_ui32_t pipeline_id) {
            return m_programs[pipeline_id];
        }


        /// Set appropriate asset drawing attributes according to the specified asset mode
        void __gl_Pipelines::setAssetVertAttributes(const das_Asset &asset) {
            switch(asset.asset_mode) {
            case DAS_ASSET_MODE_2D_UNMAPPED:
                //glUseProgram(m_programs[UM2D_I]);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, sizeof(das_ObjPosData2D), (void*) asset.offsets.pos_offset);
                glEnableVertexAttribArray(0);
                break;

            case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                glUseProgram(m_programs[TM2D_I]);

                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, sizeof(das_ObjPosData2D), (void*) asset.offsets.pos_offset);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(das_ObjTextureData), (void*) asset.offsets.tex_offset);
                break;

            case DAS_ASSET_MODE_3D_UNMAPPED:
                //glUseProgram(m_programs[UM3D_I]);
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);
                break;

            case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                glUseProgram(m_programs[TM3D_I]);

                // Enable position vertices
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(das_GL3DVertex), (void*) offsetof(das_GL3DVertex, pos));
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(das_GL3DVertex), (void*) offsetof(das_GL3DVertex, tex));
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(das_GL3DVertex), (void*) offsetof(das_GL3DVertex, norm));
                break;

            default:
                break;
            }

            glErrorCheck("glBindVertexArray", __FILE__, __LINE__);
        }


        /// Disable vertex attributes for given asset
        void __gl_Pipelines::disableAssetVertAttributes(const das_Asset &asset) {
            glDisableVertexAttribArray(0);

            switch(asset.asset_mode) {
            case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
            case DAS_ASSET_MODE_3D_UNMAPPED:
                glDisableVertexAttribArray(1);
                break;

            case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                glDisableVertexAttribArray(2);
                break;

            default:
                break;
            }
        }
    }
}