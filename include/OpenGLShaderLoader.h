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
                const std::array<std::pair<std::string, std::string>, 4> m_shader_files = {
                    std::make_pair(SRC_UNMAPPED_VERT_SHADER_2D, SRC_UNMAPPED_FRAG_SHADER_2D),
                    std::make_pair(SRC_TEXTURE_MAPPED_VERT_SHADER_2D, SRC_TEXTURE_MAPPED_FRAG_SHADER_2D),
                    std::make_pair(SRC_UNMAPPED_VERT_SHADER_3D, SRC_UNMAPPED_FRAG_SHADER_3D),
                    std::make_pair(SRC_TEXTURE_MAPPED_VERT_SHADER_3D, SRC_TEXTURE_MAPPED_FRAG_SHADER_3D)
                };

                std::array<GLuint, 4> m_programs;

            private:
                typedef bool ShaderType;
#define VERTEX      false
#define FRAGMENT    true

                std::vector<char> _ReadShaderSource(uint32_t _id, ShaderType _type);
                void _CompileShadersToProgram(uint32_t _id);
                void _CheckCompileStatus(uint32_t _shader_id, const std::string &_src_file);
                void _CheckLinkingStatus(uint32_t _id);

            public:
                ShaderLoader();

                inline GLuint &GetShaderProgramById(uint32_t _id) {
                    return m_programs[_id % PIPELINE_C];
                }
        };

    }
}

#endif
