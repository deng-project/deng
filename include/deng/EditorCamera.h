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
    #include <cmath>
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
        TRS::Point4D<float> origin = { 0.0f, 0.0f, 0.0f, 0.0f };
    };


    class DENG_API EditorCamera : public Camera3D {
        private:
            EditorCameraConfiguration m_config;
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
            EditorCamera(Entity *_parent, const std::string &_name, Renderer &_rend, EditorCameraConfiguration &_conf);
            ~EditorCamera();

            void Attach();
            void Update();

            void Rotate(TRS::Vector2<int64_t> _mouse_delta);
            inline void ZoomIn() {
                if (m_is_enabled)
                    m_translation.z -= m_config.zoom_step;
            }

            inline void ZoomOut() {
                if (m_is_enabled)
                    m_translation.z += m_config.zoom_step;
            }
    };
}

#endif
