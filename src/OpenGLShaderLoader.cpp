// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLShaderLoader.cpp - OpenGL shader loading helper class implementation
// author: Karl-Mihkel Ott

#define OPENGL_SHADER_LOADER_CPP
#include <OpenGLShaderLoader.h>


namespace DENG {
    namespace OpenGL {
        
        std::string ShaderLoader::_ReadShaderSource(const std::string &_file_name) {
            std::string abs_fname = Libdas::Algorithm::RelativePathToAbsolute(_file_name);
            std::ifstream file;
            file.open(abs_fname, std::ios_base::binary);

            // check if file was opened correctly
            if(file.fail()) FILE_ERR(_file_name);

            file.seekg(0, std::ios_base::end);
            size_t len = file.tellg();
            file.seekg(0, std::ios_base::beg);

            std::vector<char> data(len);
            file.read(data.data(), len);

            if(file.bad()) FILE_ERR(_file_name);

            file.close();
            return std::string(data.data(), data.size());
        }


        void ShaderLoader::_CompileShadersToProgram(ShaderModule &_module) {
            // 0: vertex shader
            // 1: geometry shader
            // 2: fragment shader
            std::array<GLint, 3> shaders = {};

            shaders[0] = glCreateShader(GL_VERTEX_SHADER);
            glErrorCheck("glCreateShader");
            shaders[1] = glCreateShader(GL_GEOMETRY_SHADER);
            glErrorCheck("glCreateShader");
            shaders[2] = glCreateShader(GL_FRAGMENT_SHADER);
            glErrorCheck("glCreateShader");

            // Load shader resources if needed
            if(_module.load_shaders_from_file) {
                _module.vertex_shader_src = _ReadShaderSource(_module.vertex_shader_file);
                if(_module.geometry_shader_file != "")
                    _module.geometry_shader_src = _ReadShaderSource(_module.geometry_shader_file);
                _module.fragment_shader_src = _ReadShaderSource(_module.fragment_shader_file);
            }

            GLint vlen = static_cast<GLint>(_module.vertex_shader_src.size());
            GLint glen = static_cast<GLint>(_module.geometry_shader_src.size());
            GLint flen = static_cast<GLint>(_module.fragment_shader_src.size());

            GLchar *varr = const_cast<GLchar*>(_module.vertex_shader_src.c_str());
            GLchar *garr = const_cast<GLchar*>(_module.geometry_shader_src.c_str());
            GLchar *farr = const_cast<GLchar*>(_module.fragment_shader_src.c_str());
            
            // set shader source code
            glShaderSource(shaders[0], 1, &varr, &vlen);
            glErrorCheck("glShaderSource");
            if(_module.geometry_shader_src.size()) {
                glShaderSource(shaders[1], 1, &garr, &glen);
                glErrorCheck("glShaderSource");
            }

            glShaderSource(shaders[2], 1, &farr, &flen);
            glErrorCheck("glShaderSource");

            GLuint program;
            program = glCreateProgram();
            glErrorCheck("glCreateProgram");

            // attempt to compile shaders
            glCompileShader(shaders[0]);
            _CheckCompileStatus(shaders[0], "vertex shader");
            if(_module.geometry_shader_src.size()) {
                glCompileShader(shaders[1]);
                _CheckCompileStatus(shaders[1], "geometry shader");
                glAttachShader(program, shaders[1]);
                glErrorCheck("glAttachShader");
            }
            glCompileShader(shaders[2]);
            _CheckCompileStatus(shaders[2], "fragment shader");

            // create new shader programs
            glErrorCheck("glCreateProgram");
            glAttachShader(program, shaders[0]);
            glErrorCheck("glAttachShader");
            glAttachShader(program, shaders[2]);
            glErrorCheck("glAttachShader");

            // link the newly created program
            glLinkProgram(program);
            _CheckLinkingStatus(program);

            // delete shader objects
            glDeleteShader(shaders[0]);
            glErrorCheck("glDeleteShader");
            if(_module.geometry_shader_src.size()) {
                glDeleteShader(shaders[1]);
                glErrorCheck("glDeleteShader");
            }
            glDeleteShader(shaders[2]);
            glErrorCheck("glDeleteShader");

            m_programs.push_back(program);
        }


        void ShaderLoader::_CheckCompileStatus(uint32_t _shader_id, const std::string &_file_name) {
            int success;
            char log[512] = { 0 };
            glGetShaderiv(_shader_id, GL_COMPILE_STATUS, &success);
            
            if(!success) {
                glGetShaderInfoLog(_shader_id, 512, NULL, log);
                LOG("Shader compilation error in shader: " + _file_name);
                LOG(std::string(log));
                exit(EXIT_FAILURE);
            }
        }


        void ShaderLoader::_CheckLinkingStatus(uint32_t _program_id) {
            int success;
            char log[512];
            glGetProgramiv(_program_id, GL_LINK_STATUS, &success);

            if(!success) {
                glGetProgramInfoLog(_program_id, 512, NULL, log);
                LOG("Failed to link shaders from sources");
                LOG(std::string(log));
                exit(EXIT_FAILURE);
            }
        }


        void ShaderLoader::LoadShaders(std::vector<ShaderModule> &_modules) {
            m_strides.reserve(_modules.size());
            for(ShaderModule &module : _modules) {
                _CompileShadersToProgram(module);
                m_strides.push_back(CalculateStride(module));
            }
        }
    }
}
