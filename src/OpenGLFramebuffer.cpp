// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLFramebuffer.cpp - OpenGL framebuffer class implementation
// author: Karl-Mihkel Ott

#define OPENGL_FRAMEBUFFER_CPP
#include "deng/OpenGLFramebuffer.h"

namespace DENG {
    namespace OpenGL {

        uint32_t Framebuffer::m_color_attachment_counter = 0;

        Framebuffer::Framebuffer(
            const BufferData &_bd,
            TRS::Point2D<uint32_t> _extent,
            uint32_t _missing_2d,
            uint32_t _missing_3d,
            const bool _is_default
        ) :
            m_buffer_data(_bd),
            m_extent(_extent),
            m_missing_2d(_missing_2d),
            m_missing_3d(_missing_3d),
            m_is_default_framebuffer(_is_default)
        {
            // create framebuffer objects
            if(!m_is_default_framebuffer) {
                // create framebuffer image
                TextureResource res;
                res.width = m_extent.x;
                res.height = m_extent.y;
                res.bit_depth = 4;
                res.load_type = TEXTURE_RESOURCE_LOAD_TYPE_EMBEDDED;
                res.resource_type = TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_2D_IMAGE;
                GLuint img = 0;

                glGenTextures(1, &img);
                glErrorCheck("glGenTextures");

                glBindTexture(GL_TEXTURE_2D, img);
                glErrorCheck("glBindTexture");

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)m_extent.x, (GLsizei)m_extent.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
                glErrorCheck("glTexImage2D");

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                res.vendor = img;

                // push framebuffer image to database
                TextureDatabase* db = TextureDatabase::GetInstance();
                m_framebuffer_image_id = db->AddResource(res);

                // generate and bind framebuffer
                glGenFramebuffers(1, &m_framebuffer);
                glErrorCheck("glGenFramebuffers");
                glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
                glErrorCheck("glBindFramebuffer");

                // bind framebuffer image
                glBindTexture(GL_TEXTURE_2D, m_framebuffer_image_id);
                glErrorCheck("glBindTexture");

                // generate depth buffer
                glGenRenderbuffers(1, &m_rbo);
                glErrorCheck("glGenRenderbuffers");
                glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
                glErrorCheck("glBindRenderbuffer");
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei) m_extent.x, (GLsizei) m_extent.y);
                glErrorCheck("glRenderbufferStorage");
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
                glErrorCheck("glFramebufferRenderbuffer");

                // set color attachment buffer id
                m_color_attachment_id = GL_COLOR_ATTACHMENT0 + m_color_attachment_counter++;
                glFramebufferTexture(GL_FRAMEBUFFER, m_color_attachment_id, m_framebuffer_image_id, 0);
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


        void Framebuffer::_BindVertexAttributes(const DrawCommand &_cmd, ShaderModule* _module) {
            glBindBuffer(GL_ARRAY_BUFFER, m_buffer_data.vert_buffer);
            glErrorCheck("glBindBuffer");
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer_data.vert_buffer);
            glBindVertexArray(m_buffer_data.vert_array);

            for(uint32_t i = 0; i < static_cast<uint32_t>(_module->attributes.size()); i++) {
                // enable vertex attribute array
                glEnableVertexAttribArray(i);
                glErrorCheck("glEnableVertexAttribArray");
                const GLsizei stride = static_cast<GLsizei>(_module->attribute_strides[i]);

                switch(_module->attributes[i]) {
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


        void Framebuffer::ClearFrame(const TRS::Vector4<float> _clear_color) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
            glErrorCheck("glBindFramebuffer");
            glClearColor(_clear_color.first, _clear_color.second, _clear_color.third, _clear_color.fourth);
            glErrorCheck("glClearColor");

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glErrorCheck("glClear");
        }


        void Framebuffer::Draw(const MeshReference &_ref, uint32_t _mesh_id, const std::vector<ShaderModule*>& _modules) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
            glErrorCheck("glBindFramebuffer");
            glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
            glErrorCheck("glBindRenderbuffer");

            GLint last_scissor_box[4];
            glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
            glErrorCheck("glGetIntegerv");

            GLint last_viewport[4];
            glGetIntegerv(GL_VIEWPORT, last_viewport);
            glErrorCheck("glGetIntegerv");

            // compile bound shader if needed
            if (!m_shader_loader.IsShaderCompiled(_ref.shader_module_id))
                m_shader_loader.CompileShaderToProgram(*_modules[_ref.shader_module_id], _ref.shader_module_id);
                
            // check if custom viewport was requested
            if(_modules[_ref.shader_module_id]->enable_custom_viewport) {
                GLint x = static_cast<GLint>(_modules[_ref.shader_module_id]->viewport.x);
                GLint y = static_cast<GLint>(m_extent.y) - static_cast<GLint>(_modules[_ref.shader_module_id]->viewport.y) - static_cast<GLint>(_modules[_ref.shader_module_id]->viewport.height);
                glViewport(x, y, (GLsizei) _modules[_ref.shader_module_id]->viewport.width, (GLsizei) _modules[_ref.shader_module_id]->viewport.height);
            } else {
                glViewport(0, 0, (GLsizei) m_extent.x, (GLsizei) m_extent.y);
            }

            _SetRenderState(*_modules[_ref.shader_module_id]);
            glUseProgram(m_shader_loader.GetShaderProgramById(_ref.shader_module_id));
            glErrorCheck("glUseProgram");

            for(auto cmd_it = _ref.commands.begin(); cmd_it < _ref.commands.end(); cmd_it++) {
                _BindVertexAttributes(*cmd_it, _modules[_ref.shader_module_id]);

                // bind per mesh uniform objects
                uint32_t texture_count = 0;
                TextureDatabase* db = TextureDatabase::GetInstance();
                for(auto ubo_it = _ref.ubo_data_layouts.begin(); ubo_it != _ref.ubo_data_layouts.end(); ubo_it++) {
                    const GLuint binding = static_cast<GLuint>(ubo_it->block.binding);

                    if (ubo_it->usage == UNIFORM_DATA_TYPE_BUFFER) {
                        const GLintptr offset = static_cast<GLintptr>(ubo_it->block.offset);
                        const GLsizeiptr size = static_cast<GLsizeiptr>(ubo_it->block.size);
                        
                        glBindBufferRange(GL_UNIFORM_BUFFER, binding, m_buffer_data.ubo_buffer, offset, size);
                        glErrorCheck("glBindBufferRange");
                    }
                    else if (ubo_it->usage == UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER && _modules[_ref.shader_module_id]->enable_2d_textures) {
                        glActiveTexture(GL_TEXTURE0 + binding);
                        glErrorCheck("glActiveTexture");

                        GLuint handle = std::get<GLuint>(db->GetResource(m_missing_2d).vendor);
                        if (texture_count < static_cast<uint32_t>(cmd_it->texture_ids.size()))
                            handle = std::get<GLuint>(db->GetResource(cmd_it->texture_ids[texture_count++]).vendor);
                        
                        glBindTexture(GL_TEXTURE_2D, handle);
                        glErrorCheck("glBindTexture");
                    }
                    else if (ubo_it->usage == UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER && _modules[_ref.shader_module_id]->enable_3d_textures) {
                        glActiveTexture(GL_TEXTURE0 + binding);
                        glErrorCheck("glActiveTexture");

                        GLuint handle = std::get<GLuint>(db->GetResource(m_missing_3d).vendor);
                        if (texture_count < static_cast<uint32_t>(cmd_it->texture_ids.size()))
                            handle = std::get<GLuint>(db->GetResource(cmd_it->texture_ids[texture_count++]).vendor);

                        glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
                        glErrorCheck("glBindTexture");
                    }
                }

                // for each shader uniform object bind appropriate uniform buffer ranges
                for(auto ubo_it = _modules[_ref.shader_module_id]->ubo_data_layouts.begin(); ubo_it != _modules[_ref.shader_module_id]->ubo_data_layouts.end(); ubo_it++) {
                    const GLuint binding = static_cast<GLuint>(ubo_it->block.binding);
                    if(ubo_it->type == DENG::UNIFORM_DATA_TYPE_BUFFER) {
                        const GLintptr offset = static_cast<GLintptr>(ubo_it->block.offset);
                        const GLsizeiptr size = static_cast<GLsizeiptr>(ubo_it->block.size);

                        glBindBufferRange(GL_UNIFORM_BUFFER, binding, m_buffer_data.ubo_buffer, offset, size);
                        glErrorCheck("glBindBufferRange");
                    }
                }

                // check if scissor technique is required
                if(_modules[_ref.shader_module_id]->enable_scissor) {
                    glScissor((GLint) cmd_it->scissor.offset.x, (GLint) (m_extent.y - (cmd_it->scissor.ext.y + cmd_it->scissor.offset.y)), (GLsizei) cmd_it->scissor.ext.x, (GLsizei) cmd_it->scissor.ext.y);
                    glErrorCheck("glScissor");
                } else {
                    glScissor(0, 0, (GLsizei) m_extent.x, (GLsizei) m_extent.y);
                    glErrorCheck("glScissor");
                }

                // check if indexed draw was requested
                if(_modules[_ref.shader_module_id]->enable_indexing) {
                    const size_t ioffset = static_cast<size_t>(cmd_it->indices_offset);
                    switch(_modules[_ref.shader_module_id]->prim_mode) {
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
                    switch(_modules[_ref.shader_module_id]->prim_mode) {
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

                _UnbindVertexAttributes(*_modules[_ref.shader_module_id]);
            }

            glScissor(last_scissor_box[0], last_scissor_box[1], last_scissor_box[2], last_scissor_box[3]);
            glErrorCheck("glScissor");

            glViewport(last_viewport[0], last_viewport[1], last_viewport[2], last_viewport[3]);
            glErrorCheck("glViewport");
        }


        void Framebuffer::ClearShaderResource(uint32_t _id) {
            if (m_shader_loader.IsShaderCompiled(_id))
                m_shader_loader.DeleteShaderProgram(_id);
        }
    }
}
