// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: FirstPersonCamera.h - first person camera class header
// author: Karl-Mihkel Ott


#ifndef FIRST_PERSON_CAMERA_H
#define FIRST_PERSON_CAMERA_H


#ifdef FIRST_PERSON_CAMERA_CPP
    #include <string>
    #include <cmath>
    #include <vector>
    #include <cstring>
    #include <chrono>
    #include <variant>
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
    #include "deng/Entity.h"
    #include "deng/ScriptableEntity.h"
    #include "deng/Camera3D.h"
    #include "deng/GPUMemoryManager.h"
#endif


namespace DENG {

    struct FirstPersonCameraConfiguration {
        float speed = 0.2f;                                                             // movement speed (world units per second)
        TRS::Point2D<float> max_threshold_rotation = { PI / 36.f, PI / 36.f } ;         // how much to rotate, when max threshold is achieved
        TRS::Point2D<float> max_thresholds = { 255.f, 255.f };                          // the smaller the value, the more sensitive the rotation
    };


    class DENG_API FirstPersonCamera : public Camera3D {
        private:
            FirstPersonCameraConfiguration& m_config;
            std::chrono::time_point<std::chrono::high_resolution_clock> m_beg_time = std::chrono::high_resolution_clock::now();
            std::chrono::time_point<std::chrono::high_resolution_clock> m_cur_time = std::chrono::high_resolution_clock::now();

            // TR properties
            TRS::Vector4<float> m_translation = { 0.0f, 0.0f, 0.0f, 0.0f };
            TRS::Point2D<float> m_rotation = { 0.0f, 0.0f };                // always radians

            // cached rotation values after every rotation
            TRS::Quaternion m_x_rot = { 0.0f, 0.0f, 0.0f, 1.0f };
            TRS::Quaternion m_y_rot = { 0.0f, 0.0f, 0.0f, 1.0f };


        private:
            void _EnforceLimits();
            void _ConstructViewMatrix();

            inline float _GetTimeDeltaMS() {
                m_cur_time = std::chrono::high_resolution_clock::now();
                const float ts = (m_cur_time - m_beg_time).count() / 1000.f;
                return ts;
            }

        public:
            FirstPersonCamera(Entity *_parent, const std::string &_name, Renderer &_rend, FirstPersonCameraConfiguration &_conf);
            ~FirstPersonCamera();

            void Attach();
            void Update();

            void Move(TRS::Vector3<bool> _mov);
            void Rotate(TRS::Point2D<float> _threshold);
    };
}

#endif
