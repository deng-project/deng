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

    #include <BaseTypes.h>
    #include <ErrorDefinitions.h>
    #include <Window.h>
    #include <Renderer.h>
    #include <ShaderDefinitions.h>
    #include <OpenGLShaderLoader.h>
    #include <OpenGLBufferLoader.h>
    #include <UniformData.h>
    #include <VertexAttributes.h>
#endif

namespace DENG {

#ifdef _DEBUG
        /// Global OpenGL error checking function
        void lglErrorCheck(const std::string &_func, const std::string &_file, const uint32_t _line);
#endif

#ifndef DEBUG_ONLY

    class OpenGLRenderer : public Renderer {
        private:
            OpenGL::ShaderLoader *mp_shader_loader = nullptr;
            OpenGL::BufferLoader *mp_buffer_loader = nullptr;

        private:
            inline GLenum _TextureIdToUnit(uint32_t _id) {
                return GL_TEXTURE0 + (GLenum) _id;
            }

        public:
            OpenGLRenderer(const Window &_win);
            ~OpenGLRenderer();

            virtual void UpdateUniforms() override;
            virtual void UpdateVertexBuffer(std::vector<uint8_t> &_raw_data, uint32_t _offset = 0) override;
            virtual void UpdateIndexBuffer(std::vector<uint8_t> &_raw_data, uint32_t _offset = 0) override;
            virtual void ClearFrame() override;
            virtual void RenderFrame() override;
    };
#endif
}

#endif

