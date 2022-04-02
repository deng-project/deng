// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Camera3D.h - 3D camera base class header
// author: Karl-Mihkel Ott

#ifndef CAMERA_3D_H
#define CAMERA_3D_H


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
        Libdas::Point3D<float> origin; 
    };

    class DENG_API Camera3D {
        protected:
            typedef std::variant<FirstPersonCameraConfiguration, ThirdPersonCameraConfiguration, EditorCameraConfiguration> Camera3DConfiguration;
            Window &m_window;
            Camera3DConfiguration m_config;
            CameraTransformManager m_cam_transform;

            std::chrono::time_point<std::chrono::system_clock> m_beg_time = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock> m_cur_time = std::chrono::system_clock::now();

            const uint32_t m_ubo_offset;
            std::string m_name;

        public:
            Camera3D(Window &_win, const Camera3DConfiguration &_conf, const std::string &_name, uint32_t _ubo_offset) : 
                m_window(_win), 
                m_config(_conf), 
                m_ubo_offset(_ubo_offset),
                m_name(_name) {}

            virtual void EnableCamera() = 0;
            virtual void DisableCamera() = 0;
            virtual void Update(Renderer &_rend) = 0;

            inline const std::string &GetName() {
                return m_name;
            }

            inline void UpdateConfig(const Camera3DConfiguration &_conf) {
                DENG_ASSERT(m_config.index() == _conf.index());
                m_config = _conf;
            }
    };
}

#endif
