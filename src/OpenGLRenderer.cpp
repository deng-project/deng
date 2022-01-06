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


    void OpenGLRenderer::UpdateUniforms() {
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


    void OpenGLRenderer::UpdateVertexBuffer(std::vector<uint8_t> &_raw_data, uint32_t _offset) {
        mp_buffer_loader->RequestMemory(_offset + static_cast<uint32_t>(_raw_data.size()), 0);
        void *data = glMapBufferRange(GL_ARRAY_BUFFER, (GLintptr)_offset, (GLsizeiptr)_raw_data.size(), GL_MAP_WRITE_BIT);
        glErrorCheck("glMapBufferRange");
        std::memcpy(data, _raw_data.data(), _raw_data.size());
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glErrorCheck("glUnmapBuffer");
    }


    void OpenGLRenderer::UpdateIndexBuffer(std::vector<uint8_t> &_raw_data, uint32_t _offset) {
        mp_buffer_loader->RequestMemory(0, _offset + static_cast<uint32_t>(_raw_data.size(), 0));
        void *data = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, (GLintptr) _offset, (GLsizeiptr) _raw_data.size(), GL_MAP_WRITE_BIT);
        glErrorCheck("glMapBufferRange");
        std::memcpy(data, _raw_data.data(), _raw_data.size());
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
        // ANIMATION SYSTEMS ARE NOT YET IMPLEMENTED
        
        // draw each mesh to the screen
        for(uint32_t i = 0; i < static_cast<uint32_t>(m_meshes.size()); i++) {
            // tmp
            glUseProgram(mp_shader_loader->GetShaderProgramById(UM3D_I));
            glErrorCheck("glUseProgram");

            // create and bind vertex attributes
            glEnableVertexAttribArray(0);
            glErrorCheck("glEnableVertexAttribArray");
            glEnableVertexAttribArray(1);
            glErrorCheck("glEnableVertexAttribArray");

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(UnmappedVertexAttribute), reinterpret_cast<const void*>(offsetof(UnmappedVertexAttribute, pos)));
            glErrorCheck("glVertexAttribPointer");
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(UnmappedVertexAttribute), reinterpret_cast<const void*>(offsetof(UnmappedVertexAttribute, norm)));
            glErrorCheck("glVertexAttribPointer");

            
            glDrawElements(GL_TRIANGLES, m_meshes[i].indices_count, GL_UNSIGNED_INT, (void*) m_meshes[i].indices_offset);
            glErrorCheck("glDrawElements");
        }
    }
}
