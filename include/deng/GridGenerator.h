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
    #include <array>
    #include <unordered_map>
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
    #include "deng/BaseTypes.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Window.h"
    #include "deng/Renderer.h"
    #include "deng/ModelUniforms.h"
    #include "deng/GPUMemoryManager.h"
    #include "deng/Entity.h"
    #include "deng/ScriptableEntity.h"
    #include "deng/Registry.h"
    #include "deng/Camera3D.h"
#endif

namespace DENG {

    class DENG_API GridGenerator : public Entity {
        private:
            float m_width;
            float m_height;
            float m_margin_x;
            float m_margin_y;
            uint32_t m_shader_id = UINT32_MAX;
            uint32_t m_mesh_id = UINT32_MAX;
            uint32_t m_vert_count = 0;
            const uint32_t m_camera_id;
            uint32_t m_camera_ubo_offset = 0;
            uint32_t m_ubo_offset = 0;
            uint32_t m_main_offset = 0;
            Renderer& m_renderer;
            TRS::Vector4<float> m_color = { 0.0f, 1.0f, 0.0f, 1.0f };
            const std::string m_framebuffer_id;

        private:
            void _GenerateVertices(Renderer &_rend);

        public:
            GridGenerator(
                Entity *_ent,
                const std::string &_name,
                Renderer &_rend,
                float _width,
                float _height,
                float _margin_x,
                float _margin_y,
                uint32_t _camera_id,
                const std::string &_framebuffer_id = MAIN_FRAMEBUFFER_NAME
            );
            GridGenerator(const GridGenerator& _ge) = delete;
            GridGenerator(GridGenerator&& _ge) noexcept;
            ~GridGenerator();

            inline void Show(bool _show) {
                m_renderer.GetMesh(m_mesh_id, m_framebuffer_id).enable = _show;
            }

            void Attach();
            void Update();

            inline void SetColor(const TRS::Vector4<float> &_color) {
                m_color = _color;
            }

            inline TRS::Vector4<float> GetColor() {
                return m_color;
            }

            inline uint32_t GetShaderId() {
                return m_shader_id;
            }
    };
}

#endif
