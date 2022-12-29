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
    #include <unordered_map>
    #include <fstream>
    #include <cstring>
    #include <cmath>
    #include <array>
    #include <algorithm>
    #include <queue>
#ifdef __DEBUG
    #include <iostream>
#endif

#ifdef _WIN32
    #include <Windows.h>
#endif

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Quaternion.h"

    #include "das/Api.h"
    #include "das/Algorithm.h"
    #include "das/DasStructures.h"
    #include "das/TextureReader.h"

    #include "deng/Api.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Window.h"
    #include "deng/Renderer.h"

#define DEBUG_ONLY
    #include "deng/OpenGLRenderer.h"
#undef DEBUG_ONLY
#endif

namespace DENG {

    namespace OpenGL {
        
        class ShaderLoader {
            private:
                std::vector<GLuint> m_programs;
                std::vector<uint32_t> m_program_table;
                std::vector<std::pair<std::string, GLuint>> m_ubo_name_list;

            private:
                std::string _ReadShaderSource(const std::string &_file_name);
                void _CheckCompileStatus(uint32_t _shader_id, const std::string &_shader);
                void _CheckLinkingStatus(uint32_t _program_id);

            public:
                void CompileShaderToProgram(const ShaderModule& _module, uint32_t _id);
                void DeleteShaderProgram(uint32_t _id);
                inline bool IsShaderCompiled(uint32_t _id) {
                    if (_id >= static_cast<uint32_t>(m_program_table.size()) || m_program_table[_id] == UINT32_MAX)
                        return false;
                    else return true;
                }

                inline GLuint &GetShaderProgramById(uint32_t _id) {
                    DENG_ASSERT(static_cast<size_t>(_id) < m_programs.size());
                    return m_programs[_id];
                }
        };

    }
}

#endif
