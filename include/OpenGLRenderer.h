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

#ifdef _DEBUG
    #include <iostream>
#endif
    #include <string>

    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Points.h>

    #include <Api.h>
    #include <BaseTypes.h>
    #include <ErrorDefinitions.h>
    #include <Window.h>
    #include <ShaderDefinitions.h>
    #include <Renderer.h>
    #include <UniformData.h>
    #include <VertexAttributes.h>
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

        private:
            inline GLenum _TextureIdToUnit(uint32_t _id) {
                return GL_TEXTURE0 + (GLenum) _id;
            }

            void _BindVertexAttributes(uint32_t _shader_id);

        public:
            OpenGLRenderer(const Window &_win);
            ~OpenGLRenderer();

            virtual void LoadShaders() override;
            virtual void UpdateUniforms(std::pair<char*, uint32_t> _raw_data, uint32_t _shader_id, uint32_t _ubo_id) override;
            virtual void UpdateVertexBuffer(std::pair<char*, uint32_t> _raw_data, uint32_t _offset = 0) override;
            virtual void UpdateIndexBuffer(std::pair<char*, uint32_t> _raw_data, uint32_t _offset = 0) override;
            virtual void ClearFrame() override;
            virtual void RenderFrame() override;
    };
#endif
}

#endif

