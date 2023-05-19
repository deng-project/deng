// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLRenderer.cpp - OpenGL renderer class implementation
// author: Karl-Mihkel Ott

#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H


#ifdef OPENGL_RENDERER_CPP
    #include <vector>
    #include <array>
    #include <cstring>
    #include <fstream>
    #include <memory>
    #include <queue>
    #include <cmath>
    #include <variant>
    #include <unordered_map>
    #include <algorithm>
#ifdef _DEBUG
    #include <iostream>
#endif

#ifdef _WIN32
    #include <Windows.h>
#endif
    #include <string>
    #include <vulkan/vulkan.h>

    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Points.h"
    #include "trs/Quaternion.h"

    #include "das/Api.h"
    #include "das/Algorithm.h"
    #include "das/DasStructures.h"
    #include "das/TextureReader.h"

    #include "deng/Api.h"
    #include "deng/BufferAlignment.h"
    #include "deng/BaseTypes.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/Window.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Missing.h"
    #include "deng/IRenderer.h"
    #include "deng/RenderState.h"
    #include "deng/GPUMemoryManager.h"
    #include "deng/TextureDatabase.h"
#endif

#ifndef DEBUG_ONLY
    #include "deng/OpenGLTextureData.h"
    #include "deng/OpenGLShaderLoader.h"
    #include "deng/OpenGLBufferLoader.h"
    #include "deng/OpenGLFramebuffer.h"
#endif

namespace DENG {

#ifdef _DEBUG
        /// Global OpenGL error checking function
        void lglErrorCheck(const std::string &_func, const std::string &_file, const uint32_t _line);
#endif

#ifndef DEBUG_ONLY

#ifndef NO_NEKOWIN
    namespace OpenGL {
        void DENG_API Initialise(Window &_win);
    }
#endif

    class DENG_API OpenGLRenderer : public IRenderer {
        private:
            OpenGL::BufferLoader m_buffer_loader;
            std::vector<std::vector<uint32_t>> m_ubo_offsets;

            // framebuffers
            std::vector<OpenGL::Framebuffer*> m_framebuffers;

        private:
            void _CreateVendorTextureResource(uint32_t _id);
            void _CheckAndCopyTextures();
            void _CheckAndDeleteTextures();
            void _CheckAndRemoveShaders();

        public:
            OpenGLRenderer(const RendererConfig &_conf);
            ~OpenGLRenderer();
            
            virtual FramebufferIndices AddFramebuffer(TRS::Point2D<uint32_t> _extent) override;
            virtual uint32_t AlignUniformBufferOffset(uint32_t _req) override;
            virtual void UpdateUniform(const char *_raw_data, uint32_t _size, uint32_t _offset) override;
            virtual void UpdateVertexDataBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset = 0) override;
            virtual void ClearFrame() override;
            virtual void RenderFrame() override;
    };
#endif
}

#endif

