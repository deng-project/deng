// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: EditorCamera.h - Editor camera class header
// author: Karl-Mihkel Ott


#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H


#ifdef EDITOR_CAMERA_CPP
    #include <string>
    #include <cstring>
    #include <vector>
    #include <variant>
    #include <chrono>
    #include <array>
    #include <cmath>
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
    #include "deng/Entity.h"
    #include "deng/ScriptableEntity.h"
    #include "deng/Camera3D.h"
    #include "deng/GPUMemoryManager.h"
#endif

namespace DENG {

    struct EditorCameraConfiguration {
        float zoom_step = 1.f;                  // coordinate unit
        float action_delay = 10;                // ms
        float delta_rotate = PI / 36;           // radians
        float mouse_rotation_delta = 50.0f;     // virtual pixels
    };


    class DENG_API EditorCamera : public Camera3D {
        private:
            float m_zoom_step = 1.f;                // world units
            float m_action_delay = 10.f;            // ms
            float m_rotation_step = PI / 36.f;      // radians
            float m_mouse_rotation_step = 50.f;     // virtual pixels
            TRS::Point3D<float> m_origin = { 0.0f, 0.0f, 0.0f };

            // camera TR properties
            TRS::Point3D<float> m_translation;
            TRS::Point2D<float> m_rotation = { 0.0f, 0.0f };

            // cached quaternion rotations
            TRS::Quaternion m_x_rot = { 0.0f, 0.0f, 0.0f, 1.0f };
            TRS::Quaternion m_y_rot = { 0.0f, 0.0f, 0.0f, 1.0f };

        private:
            friend class Registry;
            void _EnforceLimits();
            void _ConstructViewMatrix();

        public:
            EditorCamera(Entity* _parent, const std::string& _name, Renderer& _rend);
            ~EditorCamera();

            void Attach();
            void Update();

            void Rotate(TRS::Vector2<int64_t> _mouse_delta);
            inline void ZoomIn() {
                if (m_is_enabled)
                    m_translation.z -= m_zoom_step;
            }

            inline void ZoomOut() {
                if (m_is_enabled)
                    m_translation.z += m_zoom_step;
            }

            inline void SetZoomStep(float _step) {
                m_zoom_step = _step;
            }

            inline void SetActionDelay(float _delay) {
                m_action_delay = _delay;
            }

            inline void SetRotationStep(float _delta) {
                m_rotation_step = _delta;
            }

            inline void SetMouseRotationStep(float _step) {
                m_mouse_rotation_step = _step;
            }

            inline void SetOrigin(TRS::Point3D<float> _origin) {
                m_origin = _origin;
            }
    };
}

#endif
