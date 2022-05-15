// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: PointViewer.h - Point viewer class header
// author: Karl-Mihkel Ott

#ifndef POINT_VIEWER_H
#define POINT_VIEWER_H

#ifdef POINT_VIEWER_CPP
    #include <string>
    #include <cstring>
#ifdef _DEBUG
    #include <iostream>
#endif
    #include <vector>
    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>

    #include <Api.h>
    #include <ShaderDefinitions.h>
    #include <Window.h>
    #include <Renderer.h>
#endif

namespace DENG {

    class DENG_API PointViewer {
        private:
            Renderer &m_renderer;
            const uint32_t m_main_buffer_offset = 0;
            const uint32_t m_ubo_offset = 0;
            Libdas::Point3D<float> m_point_vertices[4];
            const uint32_t m_point_indices[6] = { 0, 1, 2, 2, 3 };
            float m_point_square_width;
            static uint32_t m_shader_id;
            static uint32_t m_point_nr;

        private:
            static void _GenerateShaderModule(Renderer &_rend, uint32_t _main_offset, uint32_t _ubo_offset);
            void _CalculateVertices();

        public:
            // _buffer_offset and _ubo_offset arguments are optional
            PointViewer(Renderer &_rend, float _width, uint32_t _buffer_offset = 0, uint32_t _ubo_offset = 0);
            void Update(Libdas::Matrix4<float> &_transform, float _width = 0.0f); // _width is not required

            inline uint32_t GetAttachedUboOffset() {
                return m_ubo_offset + static_cast<uint32_t>(sizeof(Libdas::Matrix4<float>));
            }

            inline uint32_t GetAttachedMainBufferOffset() {
                return m_main_buffer_offset + static_cast<uint32_t>(sizeof(Libdas::Vector3<float>[4]) + sizeof(uint32_t[4]));
            }
    };
}

#endif
