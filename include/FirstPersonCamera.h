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
            std::chrono::time_point<std::chrono::system_clock> m_beg_time = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock> m_cur_time = std::chrono::system_clock::now();

        public:
            FirstPersonCamera(Renderer &_rend, Window &_win, const Camera3DConfiguration &_conf, const std::string &_name, uint32_t _ubo_offset);

            virtual void EnableCamera() override;
            virtual void DisableCamera() override;
            virtual void Update() override;
    };
}

#endif
