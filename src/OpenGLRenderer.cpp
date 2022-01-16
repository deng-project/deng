// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLRenderer.cpp - OpenGL renderer class implementation
// author: Karl-Mihkel Ott

#define OPENGL_RENDERER_CPP
#include <OpenGLRenderer.h>

namespace DENG {

#ifdef _DEBUG
    void lglErrorCheck(const std::string &_func, const std::string &_file, const uint32_t _line) {
        GLenum error;
        while((error = glGetError()) != GL_NO_ERROR) {
            switch(error) {
                case GL_INVALID_ENUM:
                    std::cerr << _func << "(); " << _file << ", " << _line << ": GL_INVALID_ENUM " << std::endl;
                    break;

                case GL_INVALID_VALUE:
                    std::cerr << _func << "(); " << _file << ", " << _line << " GL_INVALID_VALUE " << std::endl;
                    break;

                case GL_INVALID_OPERATION:
                    std::cerr << _func << "(); " << _file << ", " << _line << " GL_INVALID_OPERATION " << std::endl;
                    break;

                case GL_STACK_OVERFLOW:
                    std::cerr << _func << "(); " << _file << ", " << _line << " GL_STACK_OVERFLOW " << std::endl;
                    break;

                case GL_STACK_UNDERFLOW:
                    std::cerr << _func << "(); " << _file << ", " << _line << " GL_STACK_UNDERFLOW " << std::endl;
                    break;

                case GL_OUT_OF_MEMORY:
                    std::cerr << _func << "(); " << _file << ", " << _line << " GL_OUT_OF_MEMORY " << std::endl;
                    break;

                default:
                    break;
            }
            std::abort();
        }
    }
#endif

    OpenGLRenderer::OpenGLRenderer(const Window &_win) : Renderer(_win) {
        // Load all OpenGL functions
        int status = deng_LoadGL();
        DENG_ASSERT(status);

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

        // Load all shaders into OpenGL and intialise buffer
        mp_shader_loader = new OpenGL::ShaderLoader();
        mp_buffer_loader = new OpenGL::BufferLoader();


        glDepthFunc(GL_LESS);
        glErrorCheck("glDepthFunc");
    }


    OpenGLRenderer::~OpenGLRenderer() {
        delete mp_shader_loader;
        delete mp_buffer_loader;
    }


    void OpenGLRenderer::_BindVertexAttributes(uint32_t _shader_id) {
        // enable vertex attrib arrays
        for(uint32_t i = 0; i < static_cast<uint32_t>(m_shaders[_shader_id]->attributes.size()); i++) {
            glEnableVertexAttribArray(i);
            glErrorCheck("glEnableVertexAttribArray");
        }

        for(uint32_t i = 0; i < static_cast<uint32_t>(m_shaders[_shader_id]->attributes.size()); i++) {
            switch(m_shaders[_shader_id]->attributes[i]) {
                // single element attribute
                case ATTRIBUTE_TYPE_FLOAT:
                    glVertexAttribPointer(i, 1, GL_FLOAT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_DOUBLE:
                    glVertexAttribPointer(i, 1, GL_DOUBLE, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_SHORT:
                    glVertexAttribPointer(i, 1, GL_SHORT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_UINT:
                    glVertexAttribPointer(i, 1, GL_UNSIGNED_INT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_INT:
                    glVertexAttribPointer(i, 1, GL_INT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                // two element attribute
                case ATTRIBUTE_TYPE_VEC2_FLOAT:
                    glVertexAttribPointer(i, 2, GL_FLOAT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC2_DOUBLE:
                    glVertexAttribPointer(i, 2, GL_DOUBLE, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC2_SHORT:
                    glVertexAttribPointer(i, 2, GL_SHORT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC2_UINT:
                    glVertexAttribPointer(i, 2, GL_UNSIGNED_INT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC2_INT:
                    glVertexAttribPointer(i, 2, GL_INT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                // three element attribute
                case ATTRIBUTE_TYPE_VEC3_FLOAT:
                    glVertexAttribPointer(i, 3, GL_FLOAT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC3_DOUBLE:
                    glVertexAttribPointer(i, 3, GL_DOUBLE, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC3_SHORT:
                    glVertexAttribPointer(i, 3, GL_SHORT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC3_UINT:
                    glVertexAttribPointer(i, 3, GL_UNSIGNED_INT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC3_INT:
                    glVertexAttribPointer(i, 3, GL_INT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                // four element attribute
                case ATTRIBUTE_TYPE_VEC4_FLOAT:
                    glVertexAttribPointer(i, 4, GL_FLOAT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC4_DOUBLE:
                    glVertexAttribPointer(i, 4, GL_DOUBLE, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC4_SHORT:
                    glVertexAttribPointer(i, 4, GL_SHORT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC4_UINT:
                    glVertexAttribPointer(i, 3, GL_UNSIGNED_INT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC4_INT:
                    glVertexAttribPointer(i, 4, GL_INT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;
            }
        }

    }


    void OpenGLRenderer::LoadShaders() {
        mp_shader_loader->LoadShaders(m_shaders);
    }


    void OpenGLRenderer::UpdateUniforms(char *_raw_data, uint32_t _shader_id, uint32_t _ubo_id) {
        // currently uniform buffers hold camera data information
        UniformData ubo;
        ubo.camera = m_camera_components[m_active_camera_component].camera_matrix;
        ubo.view = m_camera_components[m_active_camera_component].camera_view_matrix;

        void *data = glMapBufferRange(GL_UNIFORM_BUFFER, (GLintptr) 0, (GLsizeiptr) sizeof(ubo), GL_MAP_WRITE_BIT);
        glErrorCheck("glMapBufferRange");
        std::memcpy(data, &ubo, sizeof(ubo));
        glUnmapBuffer(GL_UNIFORM_BUFFER);
        glErrorCheck("glUnmapBuffer");
    }


    void OpenGLRenderer::UpdateVertexBuffer(std::pair<char*, uint32_t> _raw_data, uint32_t _offset) {
        mp_buffer_loader->RequestMemory(_offset + _raw_data.second, 0);
        void *data = glMapBufferRange(GL_ARRAY_BUFFER, (GLintptr) _offset, (GLsizeiptr) _raw_data.second, GL_MAP_WRITE_BIT);
        glErrorCheck("glMapBufferRange");
        std::memcpy(data, _raw_data.first, _raw_data.second);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glErrorCheck("glUnmapBuffer");
    }


    void OpenGLRenderer::UpdateIndexBuffer(std::pair<char*, uint32_t> _raw_data, uint32_t _offset) {
        mp_buffer_loader->RequestMemory(0, _offset + _raw_data.second);
        void *data = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, (GLintptr) _offset, (GLsizeiptr) _raw_data.second, GL_MAP_WRITE_BIT);
        glErrorCheck("glMapBufferRange");
        std::memcpy(data, _raw_data.first, _raw_data.second);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        glErrorCheck("glUnmapBuffer");
    }


    void OpenGLRenderer::ClearFrame() {
        glViewport(0, 0, static_cast<GLsizei>(m_window.GetSize().x), static_cast<GLsizei>(m_window.GetSize().y));
        glErrorCheck("glViewport");

        // right now the clear color will be black
        glClearColor(0, 0, 0, 0);
        glErrorCheck("glClearColor");

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glErrorCheck("glClear");

        glBindVertexArray(mp_buffer_loader->GetBufferData().vert_array);
        glErrorCheck("glBindVertexArray");
    }


    void OpenGLRenderer::RenderFrame() {
        // draw each mesh to the screen
        for(uint32_t i = 0; i < static_cast<uint32_t>(m_meshes.size()); i++) {
            glUseProgram(mp_shader_loader->GetShaderProgramById(m_meshes[i].shader_module_id));
            glErrorCheck("glUseProgram");

            _BindVertexAttributes(i);
            
            glDrawElements(GL_TRIANGLES, m_meshes[i].indices_count, GL_UNSIGNED_INT, reinterpret_cast<void*>(m_meshes[i].indices_offset));
            glErrorCheck("glDrawElements");
        }
    }
}