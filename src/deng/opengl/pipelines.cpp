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
        }


        void __gl_Pipelines::__compileShadersToProgram(const deng_ui32_t index) {
            dengMath::vec2<deng_gl_t> shaders = {};

            // Shader file info console logging for debugging mode
			#ifdef __DEBUG
				switch (index) {
                case UM2D_I:
                    LOG("Compiling shaders for unmapped 2D asset pipeline");
                    break;

                case TM2D_I:
                    LOG("Compiling shaders for texture mapped 2D asset pipeline");
                    break;

                case UM3D_I:
                    LOG("Compiling shaders for unmapped 3D asset pipeline");
                    break;

                case TM3D_I:
                    LOG("Compiling shaders for texture mapped 3D asset pipeline");
                    break;
                default:
                    goto NO_DBG;
                }

				LOG("Shader source files: " + std::string(__shader_src_files[index][0]) + ", " + std::string(__shader_src_files[index][1]));

			NO_DBG:
			#endif

            shaders.first = glCreateShader(GL_VERTEX_SHADER);
            glErrorCheck("glCreateShader");
            shaders.second = glCreateShader(GL_FRAGMENT_SHADER);
            glErrorCheck("glCreateShader");

            // Load shader sources
            char *vert = __loadShaderFromFile(__shader_src_files[index][0]);
            char *frag = __loadShaderFromFile(__shader_src_files[index][1]);

            deng_i32_t len;
            len = strlen(vert);
            glShaderSource(shaders.first, 1, &vert, &len);
            glErrorCheck("glShaderSource");

            len = strlen(frag);
            glShaderSource(shaders.second, 1, &frag, &len);
            glErrorCheck("glShaderSource");
            
            /// Attempt to compile shaders
            glCompileShader(shaders.first);
            __checkCompileStatus(shaders.first, __shader_src_files[index][0]);
            glCompileShader(shaders.second);
            __checkCompileStatus(shaders.second, __shader_src_files[index][1]);

            free(vert);
            free(frag);

            // Create new shader programs for each pipeline type
            m_programs[index] = glCreateProgram();
            glErrorCheck("glCreateProgram");
            glAttachShader(m_programs[index], shaders.first);
            glErrorCheck("glAttachShader");
            glAttachShader(m_programs[index], shaders.second);
            glErrorCheck("glAttachShader");

            glLinkProgram(m_programs[index]);
            glErrorCheck("glLinkProgram");

            __checkLinkingStatus(m_programs[index], static_cast<deng_ui32_t>(index));

            // Delete all shader objects
            glDeleteShader(shaders.first);
            glErrorCheck("glDeleteShader");
            glDeleteShader(shaders.second);
            glErrorCheck("glDeleteShader");
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


        /// Set appropriate asset drawing attributes according to the specified asset mode
        //void setAssetVertAttributes(const das_Asset &asset) {
            //switch(asset.asset_mode) {
            //case DAS_ASSET_MODE_2D_UNMAPPED:
                ////glUseProgram(m_programs[UM2D_I]);
                //glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, sizeof(das_ObjPosData2D), (void*) asset.offsets.pos_offset);
                //glEnableVertexAttribArray(0);
                //break;

            //case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                //glUseProgram(m_programs[TM2D_I]);

                //glEnableVertexAttribArray(0);
                //glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, sizeof(das_ObjPosData2D), (void*) asset.offsets.pos_offset);
                //glEnableVertexAttribArray(1);
                //glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(das_ObjTextureData), (void*) asset.offsets.tex_offset);
                //break;

            //case DAS_ASSET_MODE_3D_UNMAPPED:
                ////glUseProgram(m_programs[UM3D_I]);
                //glEnableVertexAttribArray(0);
                //glEnableVertexAttribArray(1);
                //break;

            //case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                //glUseProgram(m_programs[TM3D_I]);

                //// Enable position vertices
                //glEnableVertexAttribArray(0);
                //glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(das_GL3DVertex), (void*) offsetof(das_GL3DVertex, pos));
                //glEnableVertexAttribArray(1);
                //glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(das_GL3DVertex), (void*) offsetof(das_GL3DVertex, tex));
                //glEnableVertexAttribArray(2);
                //glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(das_GL3DVertex), (void*) offsetof(das_GL3DVertex, norm));
                //break;

            //default:
                //break;
            //}

            //glErrorCheck("glBindVertexArray");
        //}


        const deng_ui32_t __gl_Pipelines::getShaderProgram(const deng_ui32_t pipeline_id) {
            return m_programs[pipeline_id];
        }


        const std::vector<deng_ugl_t> __gl_Pipelines::getShaderPrograms() {
            std::vector<deng_ugl_t> out(m_programs.size());
            out.insert(out.begin(), m_programs.begin(), m_programs.end());
            return out;
        }
    }
}
