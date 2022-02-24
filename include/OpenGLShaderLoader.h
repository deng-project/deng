// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLShaderLoader.h - OpenGL shader loading helper class header
// author: Karl-Mihkel Ott

#ifndef OPENGL_SHADER_LOADER_H
#define OPENGL_SHADER_LOADER_H

#ifdef OPENGL_SHADER_LOADER_CPP
    #include <string>
    #include <array>
    #include <vector>
    #include <fstream>
    #include <glad/glad.h>

    #include <libdas/include/Api.h>
    #include <libdas/include/Algorithm.h>
    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>

#define DEBUG_ONLY
    #include <OpenGLRenderer.h>
#undef DEBUG_ONLY
#endif

namespace DENG {

    namespace OpenGL {
        
        class ShaderLoader {
            private:
                std::vector<GLuint> m_programs;
                std::vector<size_t> m_strides;
                std::vector<std::pair<std::string, GLuint>> m_ubo_name_list;

            private:
                std::string _ReadShaderSource(const std::string &_file_name);
                void _CompileShadersToProgram(ShaderModule &_module);
                void _CheckCompileStatus(uint32_t _shader_id, const std::string &_shader);
                void _CheckLinkingStatus(uint32_t _program_id);
                void _CalculateStride(const ShaderModule &_module);

            public:
                void LoadShaders(std::vector<ShaderModule> &_modules);

                inline GLuint &GetShaderProgramById(uint32_t _id) {
                    DENG_ASSERT(static_cast<size_t>(_id) < m_programs.size());
                    return m_programs[_id];
                }

                inline size_t GetVertexStrideById(uint32_t _id) {
                    DENG_ASSERT(static_cast<size_t>(_id) < m_programs.size());
                    return m_strides[_id];
                }
        };

    }
}

#endif
