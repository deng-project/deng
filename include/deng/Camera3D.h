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
    #include "deng/Window.h"
    #include "deng/ModelUniforms.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Renderer.h"
    #include "deng/GPUMemoryManager.h"
#endif


namespace DENG {

    struct FirstPersonCameraConfiguration {
        neko_InputBits forward;
        neko_InputBits back;
        neko_InputBits up;
        neko_InputBits down;
        neko_InputBits right;
        neko_InputBits left;

        float delta_mov = 0.02f;
        float action_delay = 10; // ms

        float delta_rotate = PI / 36;
        float mouse_rotation_delta = 50.0f;     // virtual pixels
    };

    struct ThirdPersonCameraConfiguration {
        neko_InputBits forward;
        neko_InputBits back;
        neko_InputBits up;
        neko_InputBits down;
        neko_InputBits right;
        neko_InputBits left;
    };


    struct EditorCameraConfiguration {
        neko_InputBits zoom_in;
        neko_InputBits zoom_out;
        neko_InputBits rotate_toggle;

        float zoom_step = 1.f;                 // coordinate unit
        float action_delay = 10;                // ms
        float delta_rotate = PI / 36;           // radians
        float mouse_rotation_delta = 50.0f;     // virtual pixels
        TRS::Point4D<float> origin = { 0.0f, 0.0f, 0.0f, 0.0f }; 
    };

    class DENG_API Camera3D {
        protected:
            typedef std::variant<FirstPersonCameraConfiguration, ThirdPersonCameraConfiguration, EditorCameraConfiguration> Camera3DConfiguration;
            Renderer &m_renderer;
            Window &m_window;
            Camera3DConfiguration m_config;
            ModelCameraUbo m_ubo;

            // perspective projection matrix attributes
            float m_fov = 65.f * PI / 180.f;    // 65 degrees by default
            TRS::Point2D<float> m_planes = { -0.1f, -25.0f };
            TRS::Point2D<float> m_vc_origin = { 
                static_cast<float>(m_window.GetSize().x / 2.0f),
                static_cast<float>(m_window.GetSize().y / 2.0f)
            };

            uint32_t m_ubo_offset;
            std::string m_name;

        protected:
            inline float _CalculateAspectRatio() {
                return static_cast<float>(m_window.GetSize().x) / static_cast<float>(m_window.GetSize().y);
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
            Camera3D(Renderer &_rend, Window &_win, const Camera3DConfiguration &_conf, const std::string &_name);
            ~Camera3D();

            virtual void EnableCamera() = 0;
            virtual void DisableCamera() = 0;
            virtual void Update() = 0;

            inline ModelCameraUbo &GetCameraUbo() {
                return m_ubo;
            }

            inline void SetVirtualCursorOrigin(TRS::Point2D<float> _origin) {
                m_vc_origin = _origin;
            }

            inline const std::string &GetName() {
                return m_name;
            }

            inline void ChangeFov(float _fov) {
                m_fov = _fov;
            }

            inline uint32_t GetUboOffset() {
                return m_ubo_offset;
            }

            inline void UpdateConfig(const Camera3DConfiguration &_conf) {
                DENG_ASSERT(m_config.index() == _conf.index());
                m_config = _conf;
            }
    };
}

#endif