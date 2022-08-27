// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Camera3D.h - 3D camera base class header
// author: Karl-Mihkel Ott

#ifndef CAMERA_3D_H
#define CAMERA_3D_H


#ifdef CAMERA_3D_CPP
    #include <cstdint>
    #include <cstring>
    #include <cmath>
    #include <vector>
    #include <variant>
    #include <string>
    #include <unordered_map>

#ifdef __DEBUG
    #include <iostream>
#endif

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Quaternion.h"

    #include "deng/Api.h"
    #include "deng/BaseTypes.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/Entity.h"
    #include "deng/ScriptableEntity.h"
    #include "deng/Window.h"
    #include "deng/ModelUniforms.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Renderer.h"
    #include "deng/GPUMemoryManager.h"
    
#endif


namespace DENG {

    class DENG_API Camera3D : public ScriptableEntity {
        protected:
            Renderer &m_renderer;
            ModelCameraUbo m_ubo;

            // perspective projection matrix attributes
            float m_fov = 65.f * PI / 180.f;    // 65 degrees by default
            TRS::Point2D<float> m_planes = { -0.1f, -25.0f };
            uint32_t m_ubo_offset = UINT32_MAX;
            bool m_is_enabled = false;

        protected:
            inline float _CalculateAspectRatio() {
                return static_cast<float>(m_renderer.GetConfig().canvas_size.x) / static_cast<float>(m_renderer.GetConfig().canvas_size.y);
            }

            inline TRS::Matrix4<float> _CalculateProjection() {
                return TRS::Matrix4<float> {
                    { _CalculateAspectRatio() / tanf(m_fov), 0.0f, 0.0f, 0.0f },
                    { 0.0f, 1 / tanf(m_fov / 2), 0.0f, 0.0f },
                    { 0.0f, 0.0f, m_planes.y / (m_planes.x + m_planes.y), 1.0f },
                    { 0.0f, 0.0f, (m_planes.x * m_planes.y) / (m_planes.x + m_planes.y), 0.0f }
                };
            }

        public:
            Camera3D(Entity *_parent, const std::string &_name, EntityType _type, Renderer &_rend);
            ~Camera3D();

            inline void EnableCamera() {
                m_is_enabled = true;
            }

            inline void DisableCamera() {
                m_is_enabled = false;
            }


            inline ModelCameraUbo &GetCameraUbo() {
                return m_ubo;
            }

            inline void ChangeFov(float _fov) {
                m_fov = _fov;
            }

            inline uint32_t GetUboOffset() {
                return m_ubo_offset;
            }
    };
}

#endif
