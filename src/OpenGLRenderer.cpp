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

    OpenGLRenderer::OpenGLRenderer(Window &_win, const RendererConfig &_conf) : Renderer(_win, _conf) {
        RenderState *rs = RenderState::GetInstance();
        if(rs->GetPrimary() == RENDERER_TYPE_UNKNOWN)
            rs->SetPrimary(RENDERER_TYPE_OPENGL);

        m_id = rs->RegisterRenderer(RENDERER_TYPE_OPENGL, static_cast<uint64_t>(time(NULL)));

        // Load all OpenGL functions
        int status = deng_LoadGL();
        if(!status) {
            std::cerr << "Unexpected error occured, when loading OpenGL functions" << std::endl;
            std::exit(-1);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glErrorCheck("glTexParameteri");

        // Load all shaders into OpenGL and intialise buffer
        mp_buffer_loader = new OpenGL::BufferLoader();
        mp_shader_loader = new OpenGL::ShaderLoader();

        // load missing texture
        int x, y, size;
        const char *tex = GetMissingTexture(x, y, size);
        PushTextureFromMemory(MISSING_TEXTURE_NAME, tex, x, y, 4);
    }


    OpenGLRenderer::~OpenGLRenderer() {
        // delete texture objects
        for(auto it = m_opengl_textures.begin(); it != m_opengl_textures.end(); it++)
            glDeleteTextures(1, &it->second);

        delete mp_shader_loader;
        delete mp_buffer_loader;

        RenderState *rs = RenderState::GetInstance();
        rs->RemoveRenderer(m_id);
    }


    void OpenGLRenderer::_BindVertexAttributes(const DrawCommand &_cmd, uint32_t _shader_id, uint32_t _base_offset) {
        glBindBuffer(GL_ARRAY_BUFFER, mp_buffer_loader->GetBufferData().vert_buffer);
        glErrorCheck("glBindBuffer");
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mp_buffer_loader->GetBufferData().vert_buffer);
        glBindVertexArray(mp_buffer_loader->GetBufferData().vert_array);

        DENG_ASSERT(m_shaders[_shader_id].attributes.size() == m_shaders[_shader_id].attribute_strides.size());

        for(uint32_t i = 0; i < static_cast<uint32_t>(m_shaders[_shader_id].attributes.size()); i++) {
            // enable vertex attribute array
            glEnableVertexAttribArray(i);
            glErrorCheck("glEnableVertexAttribArray");
            GLsizei stride = static_cast<GLsizei>(m_shaders[_shader_id].attribute_strides[i]);

            switch(m_shaders[_shader_id].attributes[i]) {
                // single element attribute
                case ATTRIBUTE_TYPE_FLOAT:
                    glVertexAttribPointer(i, 1, GL_FLOAT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_DOUBLE:
                    glVertexAttribPointer(i, 1, GL_DOUBLE, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_BYTE:
                    glVertexAttribPointer(i, 1, GL_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_UBYTE:
                    glVertexAttribPointer(i, 1, GL_UNSIGNED_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_SHORT:
                    glVertexAttribPointer(i, 1, GL_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_USHORT:
                    glVertexAttribPointer(i, 1, GL_UNSIGNED_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_UINT:
                    glVertexAttribPointer(i, 1, GL_UNSIGNED_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_INT:
                    glVertexAttribPointer(i, 1, GL_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                // two element attribute
                case ATTRIBUTE_TYPE_VEC2_FLOAT:
                    glVertexAttribPointer(i, 2, GL_FLOAT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC2_DOUBLE:
                    glVertexAttribPointer(i, 2, GL_DOUBLE, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC2_BYTE:
                    glVertexAttribPointer(i, 2, GL_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC2_UBYTE:
                    glVertexAttribPointer(i, 2, GL_UNSIGNED_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC2_SHORT:
                    glVertexAttribPointer(i, 2, GL_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC2_USHORT:
                    glVertexAttribPointer(i, 2, GL_UNSIGNED_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC2_UINT:
                    glVertexAttribPointer(i, 2, GL_UNSIGNED_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC2_INT:
                    glVertexAttribPointer(i, 2, GL_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                // three element attribute
                case ATTRIBUTE_TYPE_VEC3_FLOAT:
                    glVertexAttribPointer(i, 3, GL_FLOAT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC3_DOUBLE:
                    glVertexAttribPointer(i, 3, GL_DOUBLE, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC3_BYTE:
                    glVertexAttribPointer(i, 3, GL_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC3_UBYTE:
                    glVertexAttribPointer(i, 3, GL_UNSIGNED_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC3_SHORT:
                    glVertexAttribPointer(i, 3, GL_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC3_USHORT:
                    glVertexAttribPointer(i, 3, GL_UNSIGNED_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC3_UINT:
                    glVertexAttribPointer(i, 3, GL_UNSIGNED_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC3_INT:
                    glVertexAttribPointer(i, 3, GL_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                // four element attribute
                case ATTRIBUTE_TYPE_VEC4_FLOAT:
                    glVertexAttribPointer(i, 4, GL_FLOAT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC4_DOUBLE:
                    glVertexAttribPointer(i, 4, GL_DOUBLE, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC4_BYTE:
                    glVertexAttribPointer(i, 4, GL_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC4_UBYTE:
                    glVertexAttribPointer(i, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC4_SHORT:
                    glVertexAttribPointer(i, 4, GL_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC4_USHORT:
                    glVertexAttribPointer(i, 4, GL_UNSIGNED_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC4_UINT:
                    glVertexAttribPointer(i, 4, GL_UNSIGNED_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;

                case ATTRIBUTE_TYPE_VEC4_INT:
                    glVertexAttribPointer(i, 4, GL_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_base_offset + _cmd.attribute_offsets[i]));
                    glErrorCheck("glVertexAttribPointer");
                    break;
            }
        }
    }


    void OpenGLRenderer::_UnbindVertexAttributes(uint32_t _shader_id) {
        for (size_t i = 0; i < m_shaders[_shader_id].attributes.size(); i++) {
            glDisableVertexAttribArray(static_cast<GLuint>(i));
            glErrorCheck("glDisableVertexAttribArray");
        }
    }


    void OpenGLRenderer::_SetRenderState(uint32_t _shader_id) {
        // scissor test
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if(m_shaders[_shader_id].enable_scissor) {
            glEnable(GL_SCISSOR_TEST);
            glErrorCheck("glEnable");
        } else {
            glDisable(GL_SCISSOR_TEST);
            glErrorCheck("glDisable");
        }

        // depth test
        if(m_shaders[_shader_id].enable_depth_testing) {
            glEnable(GL_DEPTH_TEST);
            glErrorCheck("glEnable");
            glDepthFunc(GL_LESS);
            glErrorCheck("glDepthFunc");
        } else {
            glDisable(GL_DEPTH_TEST);
            glErrorCheck("glDisable");
        }

        // stencil testing
        if(m_shaders[_shader_id].enable_stencil_testing) {
            glEnable(GL_STENCIL_TEST);
            glErrorCheck("glEnable");
        } else {
            glDisable(GL_STENCIL_TEST);
            glErrorCheck("glDisable");
        }

        // blending
        if(m_shaders[_shader_id].enable_blend) {
            glEnable(GL_BLEND);
            glErrorCheck("glEnable");
            glBlendEquation(GL_FUNC_ADD);
            glErrorCheck("glBlendEquation");
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glErrorCheck("glBlendFuncSeparate");
        } else {
            glDisable(GL_BLEND);
            glErrorCheck("glBlend");
        }

        // culling
        switch(m_shaders[_shader_id].cull_mode) {
            case CULL_MODE_NONE:
                glDisable(GL_CULL_FACE);
                glErrorCheck("glDisable");
                break;

            case CULL_MODE_COUNTER_CLOCKWISE:
                glEnable(GL_CULL_FACE);
                glErrorCheck("glEnable");
                glCullFace(GL_FRONT);
                glErrorCheck("glCullFace");
                glFrontFace(GL_CCW);
                glErrorCheck("glFrontFace");
                break;

            case CULL_MODE_CLOCKWISE:
                glEnable(GL_CULL_FACE);
                glErrorCheck("glEnable");
                glCullFace(GL_FRONT);
                glErrorCheck("glCullFace");
                glFrontFace(GL_CW);
                glErrorCheck("glFrontFace");
                break;
        }
    }


    void OpenGLRenderer::PushTextureFromMemory(const std::string &_name, const char *_raw_data, uint32_t _width, uint32_t _height, uint32_t _bit_depth) {
        // check if the named texture already exists in texture map
        if(m_opengl_textures.find(_name) != m_opengl_textures.end()) {
            std::cerr << "Texture with name '" << _name << "' already exists in texture map" << std::endl;
            DENG_ASSERT(false);
            return;
        }

        // check if there are any texture units available
        GLint max_units;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_units);

        if(static_cast<GLint>(m_opengl_textures.size() + 1) >= max_units) {
            std::cerr << "Current texture exceeds the maximum texture capacity\n"\
                         "Requested " << m_opengl_textures.size() + 1 << ", avalilable" << max_units << std::endl;
            std::exit(1);
        }

        GLuint tex;
        glGenTextures(1, &tex);
        glErrorCheck("glGenTextures");

        glBindTexture(GL_TEXTURE_2D, tex);
        glErrorCheck("glBindTexture");

        switch(_bit_depth) {
            case 3:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(_width), static_cast<GLsizei>(_height), 0, GL_RGB, GL_UNSIGNED_BYTE, _raw_data);
                break;

            case 4:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(_width), static_cast<GLsizei>(_height), 0, GL_RGBA, GL_UNSIGNED_BYTE, _raw_data);
                break;

            default:
                DENG_ASSERT(false);
                break;
        }

        glErrorCheck("glTexImage2D");

        glGenerateMipmap(GL_TEXTURE_2D);
        glErrorCheck("glGenerateMipmap");
        
        m_opengl_textures[_name] = tex;
    }


    void OpenGLRenderer::PushTextureFromFile(const std::string &_name, const std::string &_file_name) {
        Libdas::TextureReader reader(Libdas::Algorithm::RelativePathToAbsolute(_file_name));
        size_t len;
        int x, y;
        const char *buf = reader.GetRawBuffer(x, y, len);
        PushTextureFromMemory(_name, buf, static_cast<uint32_t>(x), static_cast<uint32_t>(x), 4);
    }


    void OpenGLRenderer::RemoveTexture(const std::string &_name) {
        // check if texture does not exist in current hash map
        if(m_opengl_textures.find(_name) == m_opengl_textures.end()) {
            std::cerr << "Requested texture with name '" << _name << "' does not exist for removal" << std::endl;
            DENG_ASSERT(false);
            return;
        }

        glDeleteTextures(1, &m_opengl_textures[_name]);
    }


    std::vector<std::string> OpenGLRenderer::GetTextureNames() {
        std::vector<std::string> names;

        for(auto it = m_opengl_textures.begin(); it != m_opengl_textures.end(); it++)
            names.push_back(it->first);

        return names;
    }


    uint32_t OpenGLRenderer::AlignUniformBufferOffset(uint32_t _req) {
        return AlignData(_req, mp_buffer_loader->GetUniformBufferOffsetAlignment());
    }


    void OpenGLRenderer::LoadShaders(uint32_t _ubo_size) {
        // check if buffer reallocation is required
        if (_ubo_size) {
            mp_buffer_loader->RequestMemory(_ubo_size, GL_UNIFORM_BUFFER);
        }

        mp_shader_loader->LoadShaders(m_shaders);
    }


    void OpenGLRenderer::UpdateUniform(const char *_raw_data, uint32_t _size, uint32_t _offset) {
        mp_buffer_loader->RequestMemory(_offset + _size, GL_UNIFORM_BUFFER);
        void *data = glMapNamedBufferRange(mp_buffer_loader->GetBufferData().ubo_buffer, (GLintptr) _offset, (GLsizeiptr) _size, GL_MAP_WRITE_BIT);
        glErrorCheck("glMapBufferRange");
        std::memcpy(data, _raw_data, static_cast<size_t>(_size));
        glUnmapNamedBuffer(mp_buffer_loader->GetBufferData().ubo_buffer);
        glErrorCheck("glUnmapNamedBuffer");
    }


    void OpenGLRenderer::UpdateVertexDataBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset) {
        mp_buffer_loader->RequestMemory(_offset + _raw_data.second, GL_ARRAY_BUFFER);
        void *data = glMapNamedBufferRange(mp_buffer_loader->GetBufferData().vert_buffer, (GLintptr) _offset, (GLsizeiptr) _raw_data.second, GL_MAP_WRITE_BIT);
        glErrorCheck("glMapNamedBufferRange");
            std::memcpy(data, _raw_data.first, static_cast<size_t>(_raw_data.second));
        glUnmapNamedBuffer(mp_buffer_loader->GetBufferData().vert_buffer);
        glErrorCheck("glUnmapeNamedBuffer");
    }


    void OpenGLRenderer::ClearFrame() {
        glViewport(0, 0, static_cast<GLsizei>(m_window.GetSize().x), static_cast<GLsizei>(m_window.GetSize().y));
        glErrorCheck("glViewport");

        // right now the clear color will be black
        glClearColor(0, 0, 0, 0);
        glErrorCheck("glClearColor");

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glErrorCheck("glClear");
    }


    void OpenGLRenderer::RenderFrame() {
        GLint last_scissor_box[4];
        glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
        glErrorCheck("glGetIntegerv");

        GLint last_viewport[4];
        glGetIntegerv(GL_VIEWPORT, last_viewport);
        glErrorCheck("glGetIntegerv");

        // draw each mesh to the screen
        for(auto mesh_it = m_meshes.begin(); mesh_it < m_meshes.end(); mesh_it++) {
            const uint32_t shader_id = mesh_it->shader_module_id;

            // check if custom viewport was requested
            if(m_shaders[shader_id].enable_custom_viewport) {
                GLint x = static_cast<GLint>(m_shaders[shader_id].viewport.x);
                GLint y = static_cast<GLint>(m_window.GetSize().y) - static_cast<GLint>(m_shaders[shader_id].viewport.y) - static_cast<GLint>(m_shaders[shader_id].viewport.height);
                glViewport(x, y, (GLsizei) m_shaders[shader_id].viewport.width, (GLsizei) m_shaders[shader_id].viewport.height);
            } else {
                glViewport(last_viewport[0], last_viewport[1], last_viewport[2], last_viewport[3]);
            }

            _SetRenderState(shader_id);
            glUseProgram(mp_shader_loader->GetShaderProgramById(shader_id));
            glErrorCheck("glUseProgram");

            for(auto cmd_it = mesh_it->commands.begin(); cmd_it < mesh_it->commands.end(); cmd_it++) {
                _BindVertexAttributes(*cmd_it, shader_id, 0);

                // for each mesh uniform object bind appropriate buffer ranges
                for(auto ubo = mesh_it->ubo_blocks.begin(); ubo != mesh_it->ubo_blocks.end(); ubo++) {
                    const GLuint binding = static_cast<GLuint>(ubo->binding);
                    const GLintptr offset = static_cast<GLintptr>(ubo->offset);
                    const GLsizeiptr size = static_cast<GLsizeiptr>(ubo->size);

                    glBindBufferRange(GL_UNIFORM_BUFFER, binding, mp_buffer_loader->GetBufferData().ubo_buffer, offset, size);
                    glErrorCheck("glBindBufferRange");
                }

                // for each shader uniform object bind appropriate uniform buffer ranges
                for(auto ubo = m_shaders[shader_id].ubo_data_layouts.begin(); ubo != m_shaders[shader_id].ubo_data_layouts.end(); ubo++) {
                    const GLuint binding = static_cast<GLuint>(ubo->block.binding);

                    if(ubo->usage == UNIFORM_USAGE_PER_SHADER && ubo->type == DENG::UNIFORM_DATA_TYPE_BUFFER) {
                        const GLintptr offset = static_cast<GLintptr>(ubo->block.offset);
                        const GLsizeiptr size = static_cast<GLsizeiptr>(ubo->block.size);

                        glBindBufferRange(GL_UNIFORM_BUFFER, binding, mp_buffer_loader->GetBufferData().ubo_buffer, offset, size);
                        glErrorCheck("glBindBufferRange");
                    } else if(ubo->usage == UNIFORM_USAGE_PER_SHADER && m_shaders[shader_id].enable_texture_mapping) {
                        for(auto tex_it = cmd_it->texture_names.begin(); tex_it != cmd_it->texture_names.end(); tex_it++) {
                            const GLuint tex_id = m_opengl_textures[*tex_it];
                            glActiveTexture(GL_TEXTURE0 + binding);
                            glErrorCheck("glActiveTexture");
                            glBindTexture(GL_TEXTURE_2D, tex_id);
                            glErrorCheck("glBindTexture");
                        }
                    }
                }

                // check if scissor technique is required
                if(m_shaders[shader_id].enable_scissor) {
                    glScissor((GLint) cmd_it->scissor.offset.x, (GLint) (m_window.GetSize().y - (cmd_it->scissor.ext.y + cmd_it->scissor.offset.y)), (GLsizei) cmd_it->scissor.ext.x, (GLsizei) cmd_it->scissor.ext.y);
                    glErrorCheck("glScissor");
                } else {
                    glScissor(0, 0, (GLsizei) m_window.GetSize().x, (GLsizei) m_window.GetSize().y);
                    glErrorCheck("glScissor");
                }

                // check if indexed draw was requested
                if(m_shaders[shader_id].enable_indexing) {
                    const size_t ioffset = static_cast<size_t>(cmd_it->indices_offset);
                    switch(m_shaders[shader_id].prim_mode) {
                        case PRIMITIVE_MODE_TRIANGLES:
                            glDrawElements(GL_TRIANGLES, (GLsizei) cmd_it->draw_count, GL_UNSIGNED_INT, reinterpret_cast<const void*>(ioffset));
                            break;

                        case PRIMITIVE_MODE_LINES:
                            glDrawElements(GL_LINES, (GLsizei) cmd_it->draw_count, GL_UNSIGNED_INT, reinterpret_cast<const void*>(ioffset));
                            break;

                        case PRIMITIVE_MODE_POINTS:
                            DENG_ASSERT(false);
                            break;
                    }

                    glErrorCheck("glDrawElements");
                } else {
                    switch(m_shaders[shader_id].prim_mode) {
                        case PRIMITIVE_MODE_TRIANGLES:
                            glDrawArrays(GL_TRIANGLES, 0, (GLsizei) cmd_it->draw_count);
                            break;

                        case PRIMITIVE_MODE_POINTS:
                            glDrawArrays(GL_POINTS, 0, (GLsizei) cmd_it->draw_count);
                            break;

                        case PRIMITIVE_MODE_LINES:
                            glDrawArrays(GL_LINES, 0, (GLsizei) cmd_it->draw_count);
                            break;
                    }
                    glErrorCheck("glDrawArrays");
                }

                _UnbindVertexAttributes(shader_id);
            }
        }

        glScissor(last_scissor_box[0], last_scissor_box[1], last_scissor_box[2], last_scissor_box[3]);
        glErrorCheck("glScissor");

        glViewport(last_viewport[0], last_viewport[1], last_viewport[2], last_viewport[3]);
        glErrorCheck("glViewport");
    }
}
