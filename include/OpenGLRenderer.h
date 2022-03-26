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
    #include <cmath>

#ifdef _DEBUG
    #include <iostream>
#endif
    #include <string>

    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Points.h>
    #include <libdas/include/Quaternion.h>
    #include <libdas/include/Api.h>
    #include <libdas/include/Algorithm.h>
    #include <libdas/include/DasStructures.h>
    #include <libdas/include/TextureReader.h>

    #include <Api.h>
    #include <BufferAlignment.h>
    #include <BaseTypes.h>
    #include <ErrorDefinitions.h>
    #include <Window.h>
    #include <ShaderDefinitions.h>
    #include <Renderer.h>
#endif

#include <OpenGLShaderLoader.h>
#include <OpenGLBufferLoader.h>

namespace DENG {

#ifdef _DEBUG
        /// Global OpenGL error checking function
        void lglErrorCheck(const std::string &_func, const std::string &_file, const uint32_t _line);
#endif

#ifndef DEBUG_ONLY

    class DENG_API OpenGLRenderer : public Renderer {
        private:
            OpenGL::ShaderLoader *mp_shader_loader = nullptr;
            OpenGL::BufferLoader *mp_buffer_loader = nullptr;
            std::vector<std::vector<uint32_t>> m_ubo_offsets;
            std::vector<GLuint> m_opengl_textures;

        private:
            void _BindVertexAttributes(uint32_t _shader_id, uint32_t _base_offset);
            void _UnbindVertexAttributes(uint32_t _shader_id);
            void _SetRenderState(uint32_t _shader_id);

        public:
            OpenGLRenderer(const Window &_win, const RendererConfig &_conf);
            ~OpenGLRenderer();
            
            virtual uint32_t PushTextureFromFile(const DENG::TextureReference &_tex, const std::string& _file_name) override;
            virtual uint32_t PushTextureFromMemory(const DENG::TextureReference &_tex, const char* _raw_data, uint32_t _width, uint32_t _height, uint32_t _bit_depth) override;

            virtual uint32_t AlignUniformBufferOffset(uint32_t _req) override;
            virtual void ShrinkTextures() override;
            virtual void LoadShaders() override;
            virtual void UpdateUniform(const char *_raw_data, uint32_t _size, uint32_t _offset) override;
            virtual void UpdateCombinedBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset = 0) override;
            virtual void UpdateVertexBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset = 0) override;
            virtual void UpdateIndexBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset = 0) override;
            virtual void ClearFrame() override;
            virtual void RenderFrame() override;
    };
#endif
}

#endif

