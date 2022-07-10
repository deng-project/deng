// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: FirstPersonCamera.cpp - first person camera class implementation
// author: Karl-Mihkel Ott

#define FIRST_PERSON_CAMERA_CPP
#include <FirstPersonCamera.h>

namespace DENG {

    FirstPersonCamera::FirstPersonCamera(Renderer &_rend, Window &_win, const Camera3DConfiguration &_conf, const std::string &_name) :
        Camera3D(_rend, _win, _conf, _name) 
    { 
        DENG_ASSERT(m_config.index() == 0); 
    }

    void FirstPersonCamera::EnableCamera() {
        m_window.ChangeVCMode(true, m_vc_origin);
        m_window.ChangeCursor(NEKO_CURSOR_MODE_HIDDEN);
    }


    void FirstPersonCamera::DisableCamera() {
        m_window.ChangeVCMode(false, m_vc_origin);
        m_window.ChangeCursor(NEKO_CURSOR_MODE_STANDARD);
    }


    void FirstPersonCamera::Update() {
        m_ubo.projection_matrix = _CalculateProjection();

        m_cur_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> delta_time = m_cur_time - m_beg_time;

        FirstPersonCameraConfiguration &conf = std::get<FirstPersonCameraConfiguration>(m_config);

        // update code
        if(m_window.IsVirtualCursor()) {
            const float delta_mov = conf.delta_mov * delta_time.count() / conf.action_delay;
            Libdas::Point2D<int64_t> delta_mouse = m_window.GetMouseDelta();
            Libdas::Point2D<float> delta_mousef = { static_cast<float>(delta_mouse.x), static_cast<float>(delta_mouse.y) };

            Libdas::Point3D<float> rot = {
                delta_mousef.y * conf.delta_rotate / conf.mouse_rotation_delta,
                delta_mousef.x * conf.delta_rotate / conf.mouse_rotation_delta,
                0.0f
            };

            //auto current_rotation = m_cam_transform.GetRotations();

            // force limits to rotations on u axis
            //if(current_rotation.x + rot.x > PI / 2)
                //rot.x = PI / 2 - abs(current_rotation.x);
            //else if(current_rotation.x + rot.x < -PI / 2)
                //rot.x = -(PI / 2 - abs(current_rotation.x));

            Libdas::Vector4<float> mov = {};

            // w
            if(m_window.IsHidEventActive(conf.forward))
                mov.third = delta_mov;
            else if(m_window.IsHidEventActive(conf.back))
                mov.third = -delta_mov;

            // v
            if(m_window.IsHidEventActive(conf.up))
                mov.second = delta_mov;
            else if(m_window.IsHidEventActive(conf.down))
                mov.second = -delta_mov;

            // u
            if(m_window.IsHidEventActive(conf.right))
                mov.first = delta_mov;
            else if(m_window.IsHidEventActive(conf.left))
                mov.first = -delta_mov;

            m_beg_time = std::chrono::high_resolution_clock::now();
        }

        //m_ubo.view_matrix = m_cam_transform.ConstructViewMatrix();
        m_renderer.UpdateUniform(reinterpret_cast<const char*>(&m_ubo), sizeof(ModelCameraUbo), m_ubo_offset);
    }
}
