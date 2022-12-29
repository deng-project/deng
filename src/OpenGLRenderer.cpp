// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLRenderer.cpp - OpenGL renderer class implementation
// author: Karl-Mihkel Ott

#define OPENGL_RENDERER_CPP
#include "deng/OpenGLRenderer.h"


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

    namespace OpenGL {
        void Initialise(Window &_win) {
            RenderState *rs = RenderState::GetInstance();
            if(rs->GetPrimary() == RENDERER_TYPE_UNKNOWN)
                rs->SetPrimary(RENDERER_TYPE_OPENGL);

            Window::LoadOpenGLPlatform();
            _win.CreateOpenGLContext();
            _win.MakeOpenGLContextCurrent();
            Window::LoadOpenGLFunctions();
        }
    }


    OpenGLRenderer::OpenGLRenderer(const RendererConfig &_conf) : 
        Renderer(_conf)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glErrorCheck("glTexParameteri");

        // load missing texture
        TextureDatabase* db = TextureDatabase::GetInstance();
        int x, y, size;
        char *data = GetMissingTextureRGBA(x, y, size);
        m_missing_2d = db->AddResource(data, (uint32_t)x, (uint32_t)y, 4);
        _CreateVendorTextureResource(m_missing_2d);

        // push a missing cubemap
        m_missing_3d = db->AddResource((uint32_t)x, (uint32_t)y, 4, data, data, data, data, data, data);
        _CreateVendorTextureResource(m_missing_3d);

        m_framebuffers.push_back(new OpenGL::Framebuffer(
            m_buffer_loader.GetBufferData(), 
            m_conf.canvas_size, 
            m_missing_2d, 
            m_missing_3d, 
            true));
    }


    OpenGLRenderer::~OpenGLRenderer() {
        // delete texture objects
        TextureDatabase* db = TextureDatabase::GetInstance();
        db->DeleteAll();

        while (!db->GetDeletedEventQueue().empty()) {
            auto res = db->GetDeletedEventQueue().front();
            db->GetDeletedEventQueue().pop();
            delete[] res.rgba_data;
            GLuint handle = std::get<GLuint>(res.vendor);
            glDeleteTextures(1, &handle);
        }
    }


    FramebufferIndices OpenGLRenderer::AddFramebuffer(TRS::Point2D<uint32_t> _extent) {
        m_framebuffers.emplace_back(new OpenGL::Framebuffer(
            m_buffer_loader.GetBufferData(),
            _extent,
            m_missing_2d,
            m_missing_3d
        ));

        return FramebufferIndices(m_framebuffers.back()->GetFramebufferImageId(), static_cast<uint32_t>(m_framebuffers.size() - 1));
    }

    
    uint32_t OpenGLRenderer::AlignUniformBufferOffset(uint32_t _req) {
        return AlignData(_req, m_buffer_loader.GetUniformBufferOffsetAlignment());
    }


    void OpenGLRenderer::UpdateUniform(const char *_raw_data, uint32_t _size, uint32_t _offset) {
        m_buffer_loader.RequestMemory(_offset + _size, GL_UNIFORM_BUFFER);

        if(_raw_data) {
            void *data = glMapNamedBufferRange(m_buffer_loader.GetBufferData().ubo_buffer, (GLintptr) _offset, (GLsizeiptr) _size, GL_MAP_WRITE_BIT);
            glErrorCheck("glMapBufferRange");
            std::memcpy(data, _raw_data, static_cast<size_t>(_size));
            glUnmapNamedBuffer(m_buffer_loader.GetBufferData().ubo_buffer);
            glErrorCheck("glUnmapNamedBuffer");
        }
    }


    void OpenGLRenderer::UpdateVertexDataBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset) {
        m_buffer_loader.RequestMemory(_offset + _raw_data.second, GL_ARRAY_BUFFER);
        void *data = glMapNamedBufferRange(m_buffer_loader.GetBufferData().vert_buffer, (GLintptr) _offset, (GLsizeiptr) _raw_data.second, GL_MAP_WRITE_BIT);
        glErrorCheck("glMapNamedBufferRange");
            std::memcpy(data, _raw_data.first, static_cast<size_t>(_raw_data.second));
        glUnmapNamedBuffer(m_buffer_loader.GetBufferData().vert_buffer);
        glErrorCheck("glUnmapeNamedBuffer");
    }


    void OpenGLRenderer::ClearFrame() {
        glViewport(0, 0, static_cast<GLsizei>(m_conf.canvas_size.x), static_cast<GLsizei>(m_conf.canvas_size.y));
        glErrorCheck("glViewport");

        m_framebuffers[0]->ClearFrame(m_conf.clear_color);
        for (size_t i = 1; i < m_framebuffers.size(); i++) {
            if (m_framebuffers[i]) {
                m_framebuffers[i]->ClearFrame(m_conf.clear_color);
            }
        }
    }


    void OpenGLRenderer::RenderFrame() {
        // check if any resources were pushed or removed
        _CheckAndCopyTextures();
        _CheckAndDeleteTextures();
        _CheckAndRemoveShaders();

        // clear frames
        for (size_t i = 0; i < m_framebuffers.size(); i++) {
            if (m_framebuffers[i]) {
                m_framebuffers[i]->ClearFrame(m_conf.clear_color);
            }
        }

        for (size_t i = 0; i < m_meshes.size(); i++) {
            if (m_meshes[i]) {
                for (auto fb_it = m_meshes[i]->framebuffer_ids.begin(); fb_it != m_meshes[i]->framebuffer_ids.end(); fb_it++) {
                    if (!m_framebuffers[*fb_it]) {
                        throw std::runtime_error("Cannot draw to framebuffer " + std::to_string(i) + ". Framebuffer deleted.");
                    }

                    m_framebuffers[*fb_it]->Draw(*m_meshes[i], (uint32_t)i, m_shader_modules);
                }
            }
        }
    }


    void OpenGLRenderer::_CreateVendorTextureResource(uint32_t _id) {
        TextureDatabase* db = TextureDatabase::GetInstance();
        TextureResource& res = db->GetResource(_id);

        GLuint handle = 0;
        glGenTextures(1, &handle);
        glErrorCheck("glGenTextures");

        switch (res.resource_type) {
            case TEXTURE_RESOURCE_2D_IMAGE:
                glBindTexture(GL_TEXTURE_2D, handle);
                glErrorCheck("glBindTexture");

                if (res.bit_depth == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)res.width, (GLsizei)res.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, res.rgba_data);
                    glErrorCheck("glTexImage2D");
                } else if (res.bit_depth == 3) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)res.width, (GLsizei)res.height, 0, GL_RGB, GL_UNSIGNED_BYTE, res.rgba_data);
                    glErrorCheck("glTexImage2D");
                }

                glGenerateMipmap(GL_TEXTURE_2D);
                glErrorCheck("glGenerateMipmap");
                break;

            case TEXTURE_RESOURCE_3D_IMAGE:
                {
                    glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
                    glErrorCheck("glBindTexture");

                    uint32_t offset = 0;
                    for (uint32_t i = 0; i < 6; i++) {
                        if (res.bit_depth == 4) {
                            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, (GLsizei)res.width, (GLsizei)res.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, res.rgba_data + offset);
                            glErrorCheck("glTexImage2D");
                        } else if (res.bit_depth == 3) {
                            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, (GLsizei)res.width, (GLsizei)res.height, 0, GL_RGB, GL_UNSIGNED_BYTE, res.rgba_data + offset);
                            glErrorCheck("glTexImage2D");
                        }

                        offset += res.width * res.height * res.bit_depth;
                    }

                    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
                    glErrorCheck("glGenerateMipmap");
                }
                break;
        }

        res.vendor = handle;
    }


    void OpenGLRenderer::_CheckAndCopyTextures() {
        TextureDatabase* db = TextureDatabase::GetInstance();

        while (!db->GetAddedEventQueue().empty()) {
            uint32_t id = db->GetAddedEventQueue().front();
            db->GetAddedEventQueue().pop();

            TextureResource& res = db->GetResource(id);
            if (!res.rgba_data) {
                throw std::runtime_error("Texture " + std::to_string(id) + " submitted without RGBA data.");
            }

            _CreateVendorTextureResource(id);
            delete[] res.rgba_data;
            res.rgba_data = nullptr;
        }
    }


    void OpenGLRenderer::_CheckAndDeleteTextures() {
        TextureDatabase* db = TextureDatabase::GetInstance();

        while (!db->GetDeletedEventQueue().empty()) {
            TextureResource& res = db->GetDeletedEventQueue().front();
            db->GetDeletedEventQueue().pop();

            GLuint handle = std::get<GLuint>(res.vendor);
            glDeleteTextures(1, &handle);

            delete[] res.rgba_data;
        }
    }


    void OpenGLRenderer::_CheckAndRemoveShaders() {
        while (!m_removed_shader_queue.empty()) {
            uint32_t id = m_removed_shader_queue.front();

            for (size_t i = 0; i < m_framebuffers.size(); i++) {
                if (m_framebuffers[i]) {
                    m_framebuffers[i]->ClearShaderResource(id);
                }
            }
        }
    }
}
