// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Camera3D.h - 3D camera base class header
// author: Karl-Mihkel Ott

#ifndef CAMERA_3D_H
#define CAMERA_3D_H

#ifdef CAMERA_3D_CPP
    #include <string>
    #include <vector>
    #include <cmath>
    #include <cstring>
    #include <chrono>
#ifdef _DEBUG
    #include <iostream>
#endif

    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Quaternion.h>

    #include <Api.h>
    #include <BaseTypes.h>
    #include <Window.h>
    #include <ShaderDefinitions.h>
    #include <Renderer.h>
    #include <HidEventManager.h>
    #include <CameraTransformManager.h>
    #include <ModelUniforms.h>

    #define DELTA_MOV       0.02f
    #define ACTION_DELAY    10

    #define DELTA_ROTATE            PI / 36.0f
    #define MOUSE_ROTATION_DELTA    50.0f       // pixels virtually
#endif


namespace DENG {

    class DENG_API Camera3D {
        private:
            Window &m_window;
            // config values
            neko_InputBits m_forward;
            neko_InputBits m_back;
            neko_InputBits m_up;
            neko_InputBits m_down;
            neko_InputBits m_right;
            neko_InputBits m_left;

            std::chrono::time_point<std::chrono::system_clock> m_beg_time = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock> m_cur_time = std::chrono::system_clock::now();

            CameraTransformManager m_cam_transform;
            uint32_t m_camera_id = 0;
            const uint32_t m_ubo_offset;
            std::string m_name;

        public:
            Camera3D(Window &_win, const std::string &_name, uint32_t _ubo_offset, uint32_t _id);

            inline void EnableCamera() {
                m_window.ChangeVCMode(true);
                m_window.ChangeCursor(NEKO_CURSOR_MODE_HIDDEN);
            }

            inline void DisableCamera() {
                m_window.ChangeVCMode(false);
                m_window.ChangeCursor(NEKO_CURSOR_MODE_STANDARD);
            }

            inline const std::string &GetName() {
                return m_name;
            }

            inline uint32_t GetCameraId() {
                return m_camera_id;
            }

            void Update(Renderer &_rend);
    };
}

#endif
