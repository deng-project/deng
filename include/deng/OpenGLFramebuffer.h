// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLFramebuffer.h - OpenGL framebuffer class header
// author: Karl-Mihkel Ott

#ifndef OPENGL_FRAMEBUFFER_H
#define OPENGL_FRAMEBUFFER_H

#ifdef OPENGL_FRAMEBUFFER_CPP
    #include <string>   
    #include <unordered_map>
    #include <vector>
    #include <cstring>
    #include <cmath>
#ifdef __DEBUG
    #include <iostream>
#endif

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"

    #include "deng/Api.h"
    #include "deng/Window.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Renderer.h"
    #include "deng/OpenGLTextureData.h"
    #include "deng/OpenGLShaderLoader.h"
    #include "deng/OpenGLBufferLoader.h"

#define DEBUG_ONLY
    #include "deng/OpenGLRenderer.h"
#undef DEBUG_ONLY
#endif

namespace DENG {
    namespace OpenGL {

        class Framebuffer {
            private:
                static uint32_t m_color_attachment_counter;
                const std::string m_framebuffer_name;
                std::unordered_map<std::string, FramebufferDrawData> &m_framebuffer_draws;
                const std::unordered_map<std::string, TextureData> &m_textures;
                const BufferData m_buffer_data;
                const GLuint m_image;
                const bool m_is_default_framebuffer;

                ShaderLoader m_shader_loader;

                GLuint m_framebuffer = 0;
                GLuint m_rbo = 0;
                GLenum m_color_attachment_id = 0;

            private:
                void _SetRenderState(const ShaderModule &_module);
                void _BindVertexAttributes(const DrawCommand &_cmd, uint32_t _shader_id);
                void _UnbindVertexAttributes(const ShaderModule &_module);

            public:
                Framebuffer(
                    const std::string &_fb_name,
                    std::unordered_map<std::string, FramebufferDrawData> &_fb_draws,
                    const std::unordered_map<std::string, TextureData> &_misc_textures,
                    const BufferData &_bd,
                    const GLuint _image,
                    const bool _is_default = false
                );
                Framebuffer(Framebuffer &&_fb) noexcept = default;

                void ClearFrame(const TRS::Vector4<float> _clear_color);
                void LoadData();
                void Render();
        };
    }
}

#endif
