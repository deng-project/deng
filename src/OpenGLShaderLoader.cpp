// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLShaderLoader.cpp - OpenGL shader loading helper class implementation
// author: Karl-Mihkel Ott

#define OPENGL_SHADER_LOADER_CPP
#include <OpenGLShaderLoader.h>


namespace DENG {

    namespace OpenGL {
        
        ShaderLoader::ShaderLoader() {
            _CompileShadersToProgram(UM3D_I);
            _CompileShadersToProgram(TM3D_I);
            _CompileShadersToProgram(UM2D_I);
            _CompileShadersToProgram(TM2D_I);
        }


        std::vector<char> ShaderLoader::_ReadShaderSource(uint32_t _id, ShaderType _type) {
            std::ifstream file;
            if(_type == VERTEX) file.open(m_shader_files[_id].first);
            else file.open(m_shader_files[_id].second);

            // check if file was opened correctly
            if(file.fail()) {
                if(_type == VERTEX) FILE_ERR(m_shader_files[_id].first);
                else FILE_ERR(m_shader_files[_id].second);
            }

            file.seekg(0, std::ios_base::end);
            size_t len = file.tellg();
            file.seekg(0, std::ios_base::beg);

            std::vector<char> data(len);
            file.read(data.data(), len);

            if(file.bad()) {
                if(_type == VERTEX) FILE_ERR(m_shader_files[_id].first);
                else FILE_ERR(m_shader_files[_id].second);
            }

            file.close();
            return data;
        }


        void ShaderLoader::_CompileShadersToProgram(uint32_t _id) {
            std::pair<GLint, GLint> shaders = {};

            shaders.first = glCreateShader(GL_VERTEX_SHADER);
            glErrorCheck("glCreateShader");
            shaders.second = glCreateShader(GL_FRAGMENT_SHADER);
            glErrorCheck("glCreateShader");

            // Load shader resources
            std::vector<char> vert = _ReadShaderSource(_id, VERTEX);
            std::vector<char> frag = _ReadShaderSource(_id, FRAGMENT);

            GLint vlen = static_cast<GLint>(vert.size());
            GLint flen = static_cast<GLint>(frag.size());

            GLchar *varr = vert.data();
            GLchar *farr = frag.data();
            
            // set shader source code
            glShaderSource(shaders.first, 1, &varr, &vlen);
            glErrorCheck("glShaderSource");
            glShaderSource(shaders.second, 1, &farr, &flen);
            glErrorCheck("glShaderSource");

            // attempt to compile shaders
            glCompileShader(shaders.first);
            _CheckCompileStatus(shaders.first, m_shader_files[_id].first);
            glCompileShader(shaders.second);
            _CheckCompileStatus(shaders.second, m_shader_files[_id].second);

            // create new shader programs
            m_programs[_id] = glCreateProgram();
            glErrorCheck("glCreateProgram");
            glAttachShader(m_programs[_id], shaders.first);
            glErrorCheck("glAttachShader");
            glAttachShader(m_programs[_id], shaders.second);
            glErrorCheck("glAttachShader");

            // link the newly created program
            glLinkProgram(m_programs[_id]);
            _CheckLinkingStatus(_id);

            // delete shader objects
            glDeleteShader(shaders.first);
            glErrorCheck("glDeleteShader");
            glDeleteShader(shaders.second);
            glErrorCheck("glDeleteShader");
        }


        void ShaderLoader::_CheckCompileStatus(uint32_t _shader_id, const std::string &_file_name) {
            int success;
            char log[512] = { 0 };
            glGetShaderiv(_shader_id, GL_COMPILE_STATUS, &success);
            
            if(!success) {
                glGetShaderInfoLog(_shader_id, 512, NULL, log);
                LOG("Shader compilation error in file: " + _file_name);
                LOG(std::string(log));
                exit(EXIT_FAILURE);
            }
        }


        void ShaderLoader::_CheckLinkingStatus(uint32_t _id) {
            int success;
            char log[512];
            glGetProgramiv(m_programs[_id], GL_LINK_STATUS, &success);

            if(!success) {
                glGetProgramInfoLog(m_programs[_id], 512, NULL, log);
                LOG("Failed to link shaders from sources " + m_shader_files[_id].first + " and " + m_shader_files[_id].second);
                LOG(std::string(log));
                exit(EXIT_FAILURE);
            }
        }

    }
}
