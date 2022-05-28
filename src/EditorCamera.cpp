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
        m_translation = { 0.0f, 0.0f, conf.zoom_step };
    }


    void EditorCamera::_ForceLimits() {
        // no x axis rotation overflow for angles greater / less than PI / 2 | -PI / 2 is allowed
        if(m_rotation.x > PI / 2)
            m_rotation.x = PI / 2;
        else if(m_rotation.x < -PI / 2)
            m_rotation.x = -PI / 2;

        // don't allow y rotations greater / smaller than 2 * PI | -2 * PI 
        if(m_rotation.y > 2 * PI)
            m_rotation.y = m_rotation.y - 2 * PI;
        else if(m_rotation.y < -2 * PI)
            m_rotation.y = m_rotation.y + 2 * PI;
    }


    void EditorCamera::_ConstructViewMatrix() {
        _ForceLimits();

        // construct quaternions representing the rotation of the camera
        Libdas::Quaternion x = { sinf(m_rotation.x / 2), 0.0f, 0.0f, cosf(m_rotation.x / 2) };
        Libdas::Quaternion y = { 0.0f, sinf(m_rotation.y / 2), 0.0f, cosf(m_rotation.y / 2) };
        Libdas::Quaternion z = { 0.0f, 0.0f, sinf(m_rotation.z / 2), cosf(m_rotation.z / 2) };

        // construct translation matrix
        Libdas::Matrix4<float> translation = {
            { 1.0f, 0.0f, 0.0f, m_translation.x - m_origin.x },
            { 0.0f, 1.0f, 0.0f, m_translation.y - m_origin.y },
            { 0.0f, 0.0f, 1.0f, m_translation.z - m_origin.z },
            { 0.0f, 0.0f, 0.0f, 1.0f },
        };

        m_ubo.view_matrix = translation * (x * y * z).ExpandToMatrix4();
        m_ubo.view_matrix = m_ubo.view_matrix.Transpose();
    }


    void EditorCamera::EnableCamera() {
        m_is_enabled = true;
    }


    void EditorCamera::DisableCamera() {
        m_is_enabled = false;
    }


    void EditorCamera::Update() {
        // no transposition needed
        m_ubo.projection_matrix = _CalculateProjection();
        EditorCameraConfiguration &conf = std::get<EditorCameraConfiguration>(m_config);

        if(m_is_enabled) {
            float delta_step = 0; // conf.zoom_step * delta_time.count() / conf.action_delay;
            Libdas::Point2D<int64_t> mouse_delta = m_window.GetMouseDelta();
            Libdas::Point2D<float> delta_mousef = { static_cast<float>(mouse_delta.x), static_cast<float>(mouse_delta.y) };

            // rotation toggle is activated
            if(m_window.IsHidEventActive(conf.rotate_toggle)) {
                std::cout << "Rotate toggle active" << std::endl;
                m_window.ChangeVCMode(true);
                Libdas::Point3D<float> rot = {
                    -delta_mousef.y * conf.delta_rotate / conf.mouse_rotation_delta,
                    delta_mousef.x * conf.delta_rotate / conf.mouse_rotation_delta,
                    0.0f
                };

                m_rotation += rot;
            } else {
                m_window.ChangeVCMode(false);
                if(m_window.IsHidEventActive(conf.zoom_in) && m_translation.z - conf.zoom_step > 0) {
                    std::cout << "Zooming in" << std::endl;
                    delta_step = -conf.zoom_step;
                    m_translation.z += delta_step;
                }
                else if(m_window.IsHidEventActive(conf.zoom_out)) {
                    std::cout << "Zooming out" << std::endl;
                    delta_step = conf.zoom_step;
                    m_translation.z += delta_step;
                }
            }
        }

        _ConstructViewMatrix();
        m_renderer.UpdateUniform(reinterpret_cast<const char*>(&m_ubo), sizeof(ModelCameraUbo), m_ubo_offset);
    }
}
