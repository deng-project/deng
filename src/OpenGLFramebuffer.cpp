// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLFramebuffer.cpp - OpenGL framebuffer class implementation
// author: Karl-Mihkel Ott

#define OPENGL_FRAMEBUFFER_CPP
#include <OpenGLFramebuffer.h>

namespace DENG {
    namespace OpenGL {

        uint32_t Framebuffer::m_color_attachment_counter = 0;


        Framebuffer::Framebuffer(
            const std::string &_fb_name,
            const std::unordered_map<std::string, FramebufferDrawData> &_fb_draws,
            const std::unordered_map<std::string, GLuint> &_misc_textures,
            const BufferData &_bd,
            const GLuint _image,
            const bool _is_default
        ) :
            m_framebuffer_name(_fb_name),
            m_framebuffer_draws(_fb_draws),
            m_textures(_misc_textures),
            m_buffer_data(_bd),
            m_image(_image),
            m_is_default_framebuffer(_is_default)
        {
            DENG_ASSERT(m_framebuffer_draws.find(m_framebuffer_name) != m_framebuffer_draws.end());
            const FramebufferDrawData &draw = m_framebuffer_draws.find(m_framebuffer_name)->second;

            // create framebuffer objects
            if(!m_is_default_framebuffer) {
                // generate and bind framebuffer
                glGenFramebuffers(1, &m_framebuffer);
                glErrorCheck("glGenFramebuffers");
                glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
                glErrorCheck("glBindFramebuffer");

                // bind framebuffer image
                glBindTexture(GL_TEXTURE_2D, m_image);
                glErrorCheck("glBindTexture");

                // generate depth buffer
                glGenRenderbuffers(1, &m_rbo);
                glErrorCheck("glGenRenderbuffers");
                glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
                glErrorCheck("glBindRenderbuffer");
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei) draw.extent.x, (GLsizei) draw.extent.y);
                glErrorCheck("glRenderbufferStorage");
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
                glErrorCheck("glFramebufferRenderbuffer");

                // set color attachment buffer id
                m_color_attachment_id = GL_COLOR_ATTACHMENT0 + m_color_attachment_counter++;
                glFramebufferTexture(GL_FRAMEBUFFER, m_color_attachment_id, m_image, 0);
                glErrorCheck("glFramebufferTexture");

                // set the list of draw buffers
                glDrawBuffers(1, &m_color_attachment_id);
                glErrorCheck("glDrawBuffers");

                DENG_ASSERT(glCheckNamedFramebufferStatus(m_framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
            }
        }


        void Framebuffer::_SetRenderState(const ShaderModule &_module) {
            // scissor test
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            if(_module.enable_scissor) {
                glEnable(GL_SCISSOR_TEST);
                glErrorCheck("glEnable");
            } else {
                glDisable(GL_SCISSOR_TEST);
                glErrorCheck("glDisable");
            }

            // depth test
            if(_module.enable_depth_testing) {
                glEnable(GL_DEPTH_TEST);
                glErrorCheck("glEnable");
                glDepthFunc(GL_LESS);
                glErrorCheck("glDepthFunc");
            } else {
                glDisable(GL_DEPTH_TEST);
                glErrorCheck("glDisable");
            }

            // stencil testing
            if(_module.enable_stencil_testing) {
                glEnable(GL_STENCIL_TEST);
                glErrorCheck("glEnable");
            } else {
                glDisable(GL_STENCIL_TEST);
                glErrorCheck("glDisable");
            }

            // blending
            if(_module.enable_blend) {
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
            switch(_module.cull_mode) {
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


        void Framebuffer::_BindVertexAttributes(const DrawCommand &_cmd, uint32_t _shader_id) {
            const ShaderModule &module = m_framebuffer_draws.find(m_framebuffer_name)->second.shaders[_shader_id];

            glBindBuffer(GL_ARRAY_BUFFER, m_buffer_data.vert_buffer);
            glErrorCheck("glBindBuffer");
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer_data.vert_buffer);
            glBindVertexArray(m_buffer_data.vert_array);

            DENG_ASSERT(module.attributes.size() == module.attribute_strides.size());

            for(uint32_t i = 0; i < static_cast<uint32_t>(module.attributes.size()); i++) {
                // enable vertex attribute array
                glEnableVertexAttribArray(i);
                glErrorCheck("glEnableVertexAttribArray");
                const GLsizei stride = static_cast<GLsizei>(module.attribute_strides[i]);

                switch(module.attributes[i]) {
                    // single element attribute
                    case ATTRIBUTE_TYPE_FLOAT:
                        glVertexAttribPointer(i, 1, GL_FLOAT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_DOUBLE:
                        glVertexAttribPointer(i, 1, GL_DOUBLE, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_BYTE:
                        glVertexAttribPointer(i, 1, GL_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_UBYTE:
                        glVertexAttribPointer(i, 1, GL_UNSIGNED_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_SHORT:
                        glVertexAttribPointer(i, 1, GL_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_USHORT:
                        glVertexAttribPointer(i, 1, GL_UNSIGNED_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_UINT:
                        glVertexAttribPointer(i, 1, GL_UNSIGNED_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_INT:
                        glVertexAttribPointer(i, 1, GL_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    // two element attribute
                    case ATTRIBUTE_TYPE_VEC2_FLOAT:
                        glVertexAttribPointer(i, 2, GL_FLOAT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC2_DOUBLE:
                        glVertexAttribPointer(i, 2, GL_DOUBLE, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC2_BYTE:
                        glVertexAttribPointer(i, 2, GL_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC2_UBYTE:
                        glVertexAttribPointer(i, 2, GL_UNSIGNED_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC2_SHORT:
                        glVertexAttribPointer(i, 2, GL_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC2_USHORT:
                        glVertexAttribPointer(i, 2, GL_UNSIGNED_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC2_UINT:
                        glVertexAttribPointer(i, 2, GL_UNSIGNED_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC2_INT:
                        glVertexAttribPointer(i, 2, GL_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    // three element attribute
                    case ATTRIBUTE_TYPE_VEC3_FLOAT:
                        glVertexAttribPointer(i, 3, GL_FLOAT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC3_DOUBLE:
                        glVertexAttribPointer(i, 3, GL_DOUBLE, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC3_BYTE:
                        glVertexAttribPointer(i, 3, GL_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC3_UBYTE:
                        glVertexAttribPointer(i, 3, GL_UNSIGNED_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC3_SHORT:
                        glVertexAttribPointer(i, 3, GL_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC3_USHORT:
                        glVertexAttribPointer(i, 3, GL_UNSIGNED_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC3_UINT:
                        glVertexAttribPointer(i, 3, GL_UNSIGNED_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC3_INT:
                        glVertexAttribPointer(i, 3, GL_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    // four element attribute
                    case ATTRIBUTE_TYPE_VEC4_FLOAT:
                        glVertexAttribPointer(i, 4, GL_FLOAT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC4_DOUBLE:
                        glVertexAttribPointer(i, 4, GL_DOUBLE, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC4_BYTE:
                        glVertexAttribPointer(i, 4, GL_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC4_UBYTE:
                        glVertexAttribPointer(i, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC4_SHORT:
                        glVertexAttribPointer(i, 4, GL_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC4_USHORT:
                        glVertexAttribPointer(i, 4, GL_UNSIGNED_SHORT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC4_UINT:
                        glVertexAttribPointer(i, 4, GL_UNSIGNED_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;

                    case ATTRIBUTE_TYPE_VEC4_INT:
                        glVertexAttribPointer(i, 4, GL_INT, GL_TRUE, stride, reinterpret_cast<const void*>(_cmd.attribute_offsets[i]));
                        glErrorCheck("glVertexAttribPointer");
                        break;
                }
            }
        }


        void Framebuffer::_UnbindVertexAttributes(const ShaderModule &_module) {
            for (size_t i = 0; i < _module.attributes.size(); i++) {
                glDisableVertexAttribArray(static_cast<GLuint>(i));
                glErrorCheck("glDisableVertexAttribArray");
            }
        }


        void Framebuffer::ClearFrame(const Libdas::Vector4<float> _clear_color) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
            glErrorCheck("glBindFramebuffer");
            glClearColor(_clear_color.first, _clear_color.second, _clear_color.third, _clear_color.fourth);
            glErrorCheck("glClearColor");

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glErrorCheck("glClear");
        }


        void Framebuffer::LoadData() {
            const FramebufferDrawData &draw = m_framebuffer_draws.find(m_framebuffer_name)->second;
            m_shader_loader.LoadShaders(draw.shaders);
        }


        void Framebuffer::Render() {
            const FramebufferDrawData &draw = m_framebuffer_draws.find(m_framebuffer_name)->second;
            glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
            glErrorCheck("glBindFramebuffer");
            glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
            glErrorCheck("glBindRenderbuffer");
            glEnable(GL_DEPTH_TEST);
            glErrorCheck("glEnable");

            GLint last_scissor_box[4];
            glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
            glErrorCheck("glGetIntegerv");

            GLint last_viewport[4];
            glGetIntegerv(GL_VIEWPORT, last_viewport);
            glErrorCheck("glGetIntegerv");

            // draw each mesh to the screen
            for(auto mesh_it = draw.meshes.begin(); mesh_it < draw.meshes.end(); mesh_it++) {
                const uint32_t shader_id = mesh_it->shader_module_id;
                const ShaderModule &module = draw.shaders[shader_id];

                // check if custom viewport was requested
                if(module.enable_custom_viewport) {
                    GLint x = static_cast<GLint>(module.viewport.x);
                    GLint y = static_cast<GLint>(draw.extent.y) - static_cast<GLint>(module.viewport.y) - static_cast<GLint>(module.viewport.height);
                    glViewport(x, y, (GLsizei) module.viewport.width, (GLsizei) module.viewport.height);
                } else {
                    glViewport(0, 0, (GLsizei) draw.extent.x, (GLsizei) draw.extent.y);
                }

                _SetRenderState(draw.shaders[shader_id]);
                glUseProgram(m_shader_loader.GetShaderProgramById(shader_id));
                glErrorCheck("glUseProgram");

                for(auto cmd_it = mesh_it->commands.begin(); cmd_it < mesh_it->commands.end(); cmd_it++) {
                    _BindVertexAttributes(*cmd_it, shader_id);

                    // for each mesh uniform object bind appropriate buffer ranges
                    for(auto ubo = mesh_it->ubo_blocks.begin(); ubo != mesh_it->ubo_blocks.end(); ubo++) {
                        const GLuint binding = static_cast<GLuint>(ubo->binding);
                        const GLintptr offset = static_cast<GLintptr>(ubo->offset);
                        const GLsizeiptr size = static_cast<GLsizeiptr>(ubo->size);

                        glBindBufferRange(GL_UNIFORM_BUFFER, binding, m_buffer_data.ubo_buffer, offset, size);
                        glErrorCheck("glBindBufferRange");
                    }

                    // for each shader uniform object bind appropriate uniform buffer ranges
                    for(auto ubo = module.ubo_data_layouts.begin(); ubo != module.ubo_data_layouts.end(); ubo++) {
                        const GLuint binding = static_cast<GLuint>(ubo->block.binding);

                        if(ubo->usage == UNIFORM_USAGE_PER_SHADER && ubo->type == DENG::UNIFORM_DATA_TYPE_BUFFER) {
                            const GLintptr offset = static_cast<GLintptr>(ubo->block.offset);
                            const GLsizeiptr size = static_cast<GLsizeiptr>(ubo->block.size);

                            glBindBufferRange(GL_UNIFORM_BUFFER, binding, m_buffer_data.ubo_buffer, offset, size);
                            glErrorCheck("glBindBufferRange");
                        } else if(ubo->usage == UNIFORM_USAGE_PER_SHADER && module.enable_texture_mapping) {
                            for(auto tex_it = cmd_it->texture_names.begin(); tex_it != cmd_it->texture_names.end(); tex_it++) {
                                const GLuint tex_id = m_textures.find(*tex_it)->second;
                                glActiveTexture(GL_TEXTURE0 + binding);
                                glErrorCheck("glActiveTexture");
                                glBindTexture(GL_TEXTURE_2D, tex_id);
                                glErrorCheck("glBindTexture");
                            }
                        }
                    }

                    // check if scissor technique is required
                    if(module.enable_scissor) {
                        glScissor((GLint) cmd_it->scissor.offset.x, (GLint) (draw.extent.y - (cmd_it->scissor.ext.y + cmd_it->scissor.offset.y)), (GLsizei) cmd_it->scissor.ext.x, (GLsizei) cmd_it->scissor.ext.y);
                        glErrorCheck("glScissor");
                    } else {
                        glScissor(0, 0, (GLsizei) draw.extent.x, (GLsizei) draw.extent.y);
                        glErrorCheck("glScissor");
                    }

                    // check if indexed draw was requested
                    if(module.enable_indexing) {
                        const size_t ioffset = static_cast<size_t>(cmd_it->indices_offset);
                        switch(module.prim_mode) {
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
                        switch(module.prim_mode) {
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

                    _UnbindVertexAttributes(draw.shaders[shader_id]);
                }
            }

            glScissor(last_scissor_box[0], last_scissor_box[1], last_scissor_box[2], last_scissor_box[3]);
            glErrorCheck("glScissor");

            glViewport(last_viewport[0], last_viewport[1], last_viewport[2], last_viewport[3]);
            glErrorCheck("glViewport");
        }
    }
}
