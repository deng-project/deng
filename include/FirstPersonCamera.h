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
#ifdef _DEBUG
    #include <iostream>
#endif

    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Quaternion.h>

    #include <Api.h>
    #include <BaseTypes.h>
    #include <ErrorDefinitions.h>
    #include <Window.h>
    #include <ModelUniforms.h>
    #include <ShaderDefinitions.h>
    #include <Renderer.h>
    #include <CameraTransformManager.h>
    #include <Camera3D.h>
#endif


namespace DENG {

    class DENG_API FirstPersonCamera : public Camera3D {
        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> m_beg_time = std::chrono::high_resolution_clock::now();
            std::chrono::time_point<std::chrono::high_resolution_clock> m_cur_time = std::chrono::high_resolution_clock::now();

            // TR properties
            Libdas::Point3D<float> m_rotation;
            Libdas::Point3D<float> m_camera_pos;

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
