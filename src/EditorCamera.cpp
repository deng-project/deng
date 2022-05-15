// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: EditorCamera.cpp - Editor camera class implementation
// author: Karl-Mihkel Ott

#define EDITOR_CAMERA_CPP
#include <EditorCamera.h>

namespace DENG {

    EditorCamera::EditorCamera(Renderer &_rend, Window &_win, const Camera3DConfiguration &_conf, const std::string &_name) :
        Camera3D(_rend, _win, _conf, _name) 
    { 
        DENG_ASSERT(m_config.index() == 2); 

        EditorCameraConfiguration &conf = std::get<EditorCameraConfiguration>(m_config);
        m_cam_transform.MoveCameraAbsolutely({ 0.0f, 0.0f, -conf.zoom_step });
    }


    void EditorCamera::EnableCamera() {
        m_is_enabled = true;
    }


    void EditorCamera::DisableCamera() {
        m_is_enabled = false;
    }


    void EditorCamera::Update() {
        m_ubo.projection_matrix = _CalculateProjection();
        
        EditorCameraConfiguration &conf = std::get<EditorCameraConfiguration>(m_config);

        if(m_is_enabled) {
            float delta_step = 0; // conf.zoom_step * delta_time.count() / conf.action_delay;
            Libdas::Point2D<int64_t> mouse_delta = m_window.GetMouseDelta();
            Libdas::Point2D<float> delta_mousef = { static_cast<float>(mouse_delta.x), static_cast<float>(mouse_delta.y) };

            // rotation toggle is activated
            if(m_window.IsHidEventActive(conf.rotate_toggle)) {
                m_window.ChangeVCMode(true);
                Libdas::Point3D<float> rot = {
                    delta_mousef.y * conf.delta_rotate / conf.mouse_rotation_delta,
                    delta_mousef.x * conf.delta_rotate / conf.mouse_rotation_delta,
                    0.0f
                };

                // check for x rotation overflow
                auto current_rotation = m_cam_transform.GetRotations();
                // force limits to rotations on u axis
                if(current_rotation.x + rot.x > PI / 2)
                    rot.x = PI / 2 - abs(current_rotation.x);
                else if(current_rotation.x + rot.x < -PI / 2)
                    rot.x = -(PI / 2 - abs(current_rotation.x));

                m_cam_transform.CustomPointRelativeRotation(rot, m_center_point);

            } else {
                m_window.ChangeVCMode(false);
                if(m_window.IsHidEventActive(conf.zoom_in) && m_cam_transform.GetPosition().third + conf.zoom_step < 0) {
                    delta_step = conf.zoom_step;
                    m_cam_transform.MoveCameraAbsolutely({ 0.0f, 0.0f, delta_step });
                }
                else if(m_window.IsHidEventActive(conf.zoom_out)) {
                    delta_step = -conf.zoom_step;
                    m_cam_transform.MoveCameraAbsolutely({ 0.0f, 0.0f, delta_step });
                }
            }
        }

        m_ubo.view_matrix = m_cam_transform.ConstructViewMatrix();
        m_renderer.UpdateUniform(reinterpret_cast<const char*>(&m_ubo), sizeof(ModelCameraUbo), m_ubo_offset);
    }
}
