// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GridGenerator.h - Grid generator class header
// author: Karl-Mihkel Ott

#ifndef GRID_GENERATOR_H
#define GRID_GENERATOR_H

#ifdef GRID_GENERATOR_CPP
    #include <string>
    #include <cstring>
    #include <cmath>
    #include <vector>
#ifdef _DEBUG
    #include <iostream>
#endif
    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>

    #include <Api.h>
    #include <ShaderDefinitions.h>
    #include <Window.h>
    #include <Renderer.h>
    #include <ModelUniforms.h>
    #include <GPUMemoryManager.h>
#endif

namespace DENG {

    class DENG_API GridGenerator {
        private:
            float m_width;
            float m_height;
            float m_margin_x;
            float m_margin_y;
            uint32_t m_shader_id = UINT32_MAX;
            uint32_t m_mesh_id = UINT32_MAX;
            uint32_t m_vert_count = 0;
            uint32_t m_camera_ubo_offset = 0;
            uint32_t m_ubo_offset = 0;
            uint32_t m_main_offset = 0;
            Libdas::Vector4<float> m_color = { 1.0f, 1.0f, 1.0f, 1.0f };

        private:
            void _GenerateVertices(Renderer &_rend);

        public:
            GridGenerator(float _width, float _height, float _margin_x, float _margin_y, uint32_t _camera_offset);
            void Attach(Renderer &_rend);
            void Update(Renderer &_rend, const Libdas::Vector4<float> _color);

            inline Libdas::Vector4<float> GetColor() {
                return m_color;
            }

            inline uint32_t GetShaderId() {
                return m_shader_id;
            }
    };
}

#endif
