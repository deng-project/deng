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
        // delete texture objects
        for(auto it = m_textures.begin(); it != m_textures.end(); it++)
            glDeleteTextures(1, &it->r_identifier);

        delete mp_shader_loader;
        delete mp_buffer_loader;
    }


    void OpenGLRenderer::_BindVertexAttributes(uint32_t _shader_id) {
        for(uint32_t i = 0; i < static_cast<uint32_t>(m_shaders[_shader_id]->attributes.size()); i++) {
            // enable vertex attribute array
            glEnableVertexAttribArray(i);
            glErrorCheck("glEnableVertexAttribArray");

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
                    glVertexAttribPointer(i, 4, GL_UNSIGNED_INT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC4_INT:
                    glVertexAttribPointer(i, 4, GL_INT, GL_TRUE, m_shaders[_shader_id]->strides[i], reinterpret_cast<const void*>(m_shaders[_shader_id]->offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;
            }
        }
    }


    void OpenGLRenderer::_UnbindVertexAttributes(uint32_t _shader_id) {
        for (size_t i = 0; i < m_shaders[_shader_id]->attributes.size(); i++) {
            glDisableVertexAttribArray(i);
            glErrorCheck("glDisableVertexAttribArray");
        }
    }


    uint32_t OpenGLRenderer::PushTextureFromMemory(DENG::TextureReference& _tex, const char *_raw_data, uint32_t _width, uint32_t _height, uint32_t _bit_depth) {
        // check if there are any texture units available
        GLint max_units;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_units);

        if (static_cast<GLint>(m_textures.size() + 1) >= max_units) {
            std::cerr << "Current texture exceeds the maximum texture capacity\n"\
                         "Requested " << m_textures.size() + 1 << ", avalilable" << max_units << std::endl;
            std::exit(1);
        }

        GLuint tex;
        glGenTextures(1, &tex);
        glErrorCheck("glGenTextures");

        glBindTexture(GL_TEXTURE_2D, tex);
        glErrorCheck("glBindTexture");

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(_width), static_cast<GLsizei>(_height), 0, GL_RGBA, GL_UNSIGNED_BYTE, _raw_data);
        glErrorCheck("glTexImage2D");

        glGenerateMipmap(GL_TEXTURE_2D);
        glErrorCheck("glGenerateMipmap");
        
        _tex.r_identifier = tex;
        m_textures.push_back(_tex);
        
        return static_cast<uint32_t>(m_textures.size() - 1);
    }


    uint32_t OpenGLRenderer::PushTextureFromFile(DENG::TextureReference &_tex, const std::string &_file_name) {
        Libdas::TextureReader reader(Libdas::Algorithm::RelativePathToAbsolute(_file_name));
        size_t len;
        int x, y;
        const char *buf = reader.GetRawBuffer(x, y, len);
        return PushTextureFromMemory(_tex, buf, static_cast<uint32_t>(x), static_cast<uint32_t>(x), 4);
    }


    void OpenGLRenderer::LoadShaders() {
        mp_shader_loader->LoadShaders(m_shaders);
    }


    void OpenGLRenderer::UpdateUniform(char *_raw_data, uint32_t _shader_id, uint32_t _ubo_id) {
        uint32_t &offset = m_shaders[_shader_id]->ubo_data_layouts[_ubo_id].offset;
        const uint32_t size = m_shaders[_shader_id]->ubo_data_layouts[_ubo_id].ubo_size;
        const uint32_t aligned_size = AlignData(size, mp_buffer_loader->GetUniformBufferOffsetAlignment());
        
        // check if new offsets need to be pushed back
        if (offset == UINT32_MAX) {
            offset = m_high_ubo_offset;
            m_high_ubo_offset += aligned_size;
            mp_buffer_loader->RequestMemory(offset + aligned_size, GL_UNIFORM_BUFFER);
        }

        void *data = glMapBufferRange(GL_UNIFORM_BUFFER, (GLintptr) offset, (GLsizeiptr) size, GL_MAP_WRITE_BIT);
        glErrorCheck("glMapBufferRange");
        std::memcpy(data, _raw_data, static_cast<size_t>(size));
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
        glClearColor(0, 0.5, 0, 0);
        glErrorCheck("glClearColor");

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glErrorCheck("glClear");

        glBindVertexArray(mp_buffer_loader->GetBufferData().vert_array);
        glErrorCheck("glBindVertexArray");
    }


    void OpenGLRenderer::RenderFrame() {
        // draw each mesh to the screen
        for(uint32_t i = 0; i < static_cast<uint32_t>(m_meshes.size()); i++) {
            uint32_t shader = m_meshes[i].shader_module_id;
            glUseProgram(mp_shader_loader->GetShaderProgramById(shader));
            glErrorCheck("glUseProgram");

            _BindVertexAttributes(i);
            
            // for each shader and its uniform objects bind appropriate uniform buffer ranges
            GLuint ubo_i = 0;
            for (size_t j = 0; j < m_shaders[shader]->ubo_data_layouts.size(); j++) {
                if (m_shaders[shader]->ubo_data_layouts[j].type == DENG::UNIFORM_DATA_TYPE_BUFFER) {
                    const GLuint binding = static_cast<GLuint>(m_shaders[shader]->ubo_data_layouts[j].binding);
                    const GLintptr offset = static_cast<GLintptr>(m_shaders[shader]->ubo_data_layouts[j].offset);
                    const GLsizeiptr size = static_cast<GLsizeiptr>(m_shaders[shader]->ubo_data_layouts[j].ubo_size);

                    glUniformBlockBinding(mp_shader_loader->GetShaderProgramById(i), ubo_i, binding);
                    glErrorCheck("glUniformBlockBinding");
                    glBindBufferRange(GL_UNIFORM_BUFFER, ubo_i, mp_buffer_loader->GetBufferData().ubo_buffer, offset, size);
                    glErrorCheck("glBindBufferRange");
                    ubo_i++;
                }
            }

            // check if texture should be bound
            if (m_meshes[i].texture_id != UINT32_MAX) {
                const uint32_t sampler_id = m_textures[m_meshes[i].texture_id].shader_sampler_id;
                const uint32_t tex_id = m_textures[m_meshes[i].texture_id].r_identifier;
                glActiveTexture(GL_TEXTURE0 + sampler_id);
                glErrorCheck("glActiveTexture");
                glBindTexture(GL_TEXTURE_2D, tex_id);
                glErrorCheck("glBindTexture");
            }
            
            glDrawElements(GL_TRIANGLES, m_meshes[i].indices_count, GL_UNSIGNED_INT, reinterpret_cast<void*>(m_meshes[i].indices_offset));
            glErrorCheck("glDrawElements");

            _UnbindVertexAttributes(i);
        }
    }
}
