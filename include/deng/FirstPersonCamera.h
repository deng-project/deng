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
    #include "deng/Camera3D.h"
#endif


namespace DENG {

    class DENG_API FirstPersonCamera : public Camera3D {
        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> m_beg_time = std::chrono::high_resolution_clock::now();
            std::chrono::time_point<std::chrono::high_resolution_clock> m_cur_time = std::chrono::high_resolution_clock::now();

            // TR properties
            TRS::Point3D<float> m_camera_pos;
            TRS::Point3D<float> m_rotation;

        private:
            void _ForceLimits();
            void _ConstructViewMatrix();

        public:
            FirstPersonCamera(Renderer &_rend, Window &_win, const Camera3DConfiguration &_conf, const std::string &_name);

            virtual void EnableCamera() override;
            virtual void DisableCamera() override;
            virtual void Update() override;
    };
}

#endif
