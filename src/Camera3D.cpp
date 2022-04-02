// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Camera3D.h - 3D camera base class implementation
// author: Karl-Mihkel Ott

#define CAMERA_3D_CPP
#include <Camera3D.h>

namespace DENG {

    Camera3D::Camera3D(Window &_win, const std::string &_name, uint32_t _ubo_offset, uint32_t _id) : 
        m_window(_win),
        m_camera_id(_id),
        m_ubo_offset(_ubo_offset),
        m_name(_name) 
    {
        m_forward = neko_CreateInputMask(1, NEKO_KEY_W);
        m_back = neko_CreateInputMask(1, NEKO_KEY_S);
        m_up = neko_CreateInputMask(1, NEKO_KEY_SPACE);
        m_down = neko_CreateInputMask(1, NEKO_KEY_LEFT_SHIFT);
        m_right = neko_CreateInputMask(1, NEKO_KEY_D);
        m_left = neko_CreateInputMask(1, NEKO_KEY_A);
    }


    void Camera3D::Update(Renderer &_rend) {
        const float aspect_ratio = static_cast<float>(m_window.GetSize().x) / static_cast<float>(m_window.GetSize().y);
        const float fov = 65 * PI / 180;                            // 65 degrees
        const Libdas::Point2D<float> planes = { -0.1f, -25.0f };    // some random plane values
        
        ModelCameraUbo ubo;
        ubo.projection_matrix = Libdas::Matrix4<float> {
            { aspect_ratio / tanf(fov), 0.0f, 0.0f, 0.0f },
            { 0.0f, 1 / tanf(fov / 2), 0.0f, 0.0f },
            { 0.0f, 0.0f, planes.y / (planes.x + planes.y), 1.0f },
            { 0.0f, 0.0f, (planes.x * planes.y) / (planes.x + planes.y), 0.0f }
        };

        m_cur_time = std::chrono::system_clock::now();
        std::chrono::duration<float, std::milli> delta_time = m_cur_time - m_beg_time;

        // update code
        if(delta_time.count() >= ACTION_DELAY && m_window.IsVirtualCursor()) {
            const float delta_mov = DELTA_MOV * delta_time.count() / ACTION_DELAY;
            Libdas::Point2D<int64_t> delta_mouse = m_window.GetMouseDelta();
            Libdas::Point2D<float> delta_mousef = { static_cast<float>(delta_mouse.x), static_cast<float>(delta_mouse.y) };

            Libdas::Point3D<float> rot = {
                delta_mousef.y * DELTA_ROTATE / MOUSE_ROTATION_DELTA,
                delta_mousef.x * DELTA_ROTATE / MOUSE_ROTATION_DELTA,
                0.0f
            };

            auto current_rotation = m_cam_transform.GetRotations();

            // force limits to rotations on u axis
            if(current_rotation.x + rot.x > PI / 2)
                rot.x = PI / 2 - abs(current_rotation.x);
            else if(current_rotation.x + rot.x < -PI / 2)
                rot.x = -(PI / 2 - abs(current_rotation.x));

            Libdas::Vector3<float> mov = {};

            // w
            if(m_window.IsHidEventActive(m_forward))
                mov.third = delta_mov;
            else if(m_window.IsHidEventActive(m_back))
                mov.third = -delta_mov;

            // v
            if(m_window.IsHidEventActive(m_up))
                mov.second = delta_mov;
            else if(m_window.IsHidEventActive(m_down))
                mov.second = -delta_mov;

            // u
            if(m_window.IsHidEventActive(m_right))
                mov.first = delta_mov;
            else if(m_window.IsHidEventActive(m_left))
                mov.first = -delta_mov;

            m_cam_transform.MoveCameraRelatively(mov, true);
            m_cam_transform.RotateCameraRelatively(rot);
            m_beg_time = std::chrono::system_clock::now();
        }

        ubo.view_matrix = m_cam_transform.ConstructViewMatrix();
        _rend.UpdateUniform(reinterpret_cast<const char*>(&ubo), sizeof(ModelCameraUbo), m_ubo_offset);
    }
}
