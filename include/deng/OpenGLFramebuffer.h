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
    #include <variant>
    #include <cstring>
    #include <cmath>
    #include <array>
    #include <queue>
    #include <stdexcept>
    #include <vulkan/vulkan.h>
#ifdef __DEBUG
    #include <iostream>
#endif

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Quaternion.h"

    #include "das/Api.h"
    #include "das/DasStructures.h"
    #include "das/TextureReader.h"

    #include "deng/Api.h"
    #include "deng/Window.h"
    #include "deng/TextureDatabase.h"
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
                BufferData m_buffer_data;
                bool m_is_default_framebuffer;

                ShaderLoader m_shader_loader;
                TRS::Point2D<uint32_t> m_extent;

                GLuint m_framebuffer = 0;
                GLuint m_rbo = 0;
                GLenum m_color_attachment_id = 0;
                uint32_t m_framebuffer_image_id = 0;

                uint32_t m_missing_2d = 0;
                uint32_t m_missing_3d = 0;

            private:
                void _SetRenderState(const ShaderModule &_module);
                void _BindVertexAttributes(const DrawCommand &_cmd, ShaderModule* _module);
                void _UnbindVertexAttributes(const ShaderModule &_module);

            public:
                Framebuffer(
                    const BufferData& _bd,
                    TRS::Point2D<uint32_t> _extent,
                    uint32_t _missing_2d,
                    uint32_t _missing_3d,
                    const bool _is_default = false
                );
                Framebuffer(Framebuffer &&_fb) noexcept = default;

                void ClearFrame(const TRS::Vector4<float> _clear_color);
                void Draw(const MeshReference& _ref, uint32_t _mesh_id, const std::vector<ShaderModule*>& _modules);
                void ClearShaderResource(uint32_t _id);

                inline uint32_t GetFramebufferImageId() {
                    return m_framebuffer_image_id;
                }
        };
    }
}

#endif
