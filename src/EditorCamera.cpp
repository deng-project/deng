// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: EditorCamera.cpp - Editor camera class implementation
// author: Karl-Mihkel Ott

#define EDITOR_CAMERA_CPP
#include "deng/EditorCamera.h"

namespace DENG {

    EditorCamera::EditorCamera(Entity* _parent, const std::string& _name, Renderer& _rend) :
        Camera3D(_parent, _name, ENTITY_TYPE_EDITOR_CAMERA, _rend),
        m_translation(0.0f, 0.0f, m_zoom_step) {}


    EditorCamera::~EditorCamera() {
        if (_OnDestroyFunction)
            _OnDestroyFunction(m_script);
    }


    void EditorCamera::_EnforceLimits() {
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
        // construct translation matrix
        m_ubo.pos = TRS::Vector4<float>{
            m_translation.x - m_origin.x,
            m_translation.y - m_origin.y,
            m_translation.z - m_origin.z,
            1.f,
        };

        TRS::Matrix4<float> translation = {
            { 1.0f, 0.0f, 0.0f, m_ubo.pos.first },
            { 0.0f, 1.0f, 0.0f, m_ubo.pos.second },
            { 0.0f, 0.0f, 1.0f, m_ubo.pos.third },
            { 0.0f, 0.0f, 0.0f, 1.0f },
        };

        m_ubo.view_matrix = translation * (m_x_rot * m_y_rot).ExpandToMatrix4();
        m_ubo.view_matrix = m_ubo.view_matrix.Transpose();
    }

    void EditorCamera::Attach() {
        GPUMemoryManager* mem_manager = GPUMemoryManager::GetInstance();
        m_ubo_offset = mem_manager->RequestUniformMemoryLocationP(m_renderer, sizeof(ModelCameraUbo));
        SetAttachedBit(true);

        if (_OnAttachFunction)
            _OnAttachFunction(m_script);
    }


    void EditorCamera::Update() {
        // no transposition needed
        m_ubo.projection_matrix = _CalculateProjection();

        if (_OnUpdateFunction)
            _OnUpdateFunction(m_script);

        _ConstructViewMatrix();
        m_renderer.UpdateUniform(reinterpret_cast<const char*>(&m_ubo), sizeof(ModelCameraUbo), m_ubo_offset);
    }


    // might be omitted in the future
    void EditorCamera::Rotate(TRS::Vector2<int64_t> _mouse_delta) {
        if (m_is_enabled) {
            TRS::Point2D<float> delta_mousef = {
                static_cast<float>(_mouse_delta.first),
                static_cast<float>(_mouse_delta.second)
            };

            TRS::Point2D<float> rot = {
                -delta_mousef.y * m_rotation_step / m_mouse_rotation_step,
                delta_mousef.x * m_rotation_step / m_mouse_rotation_step
            };

            m_rotation += rot;
            _EnforceLimits();
            m_x_rot = TRS::Quaternion(sinf(m_rotation.x / 2.f), 0.0f, 0.0f, cosf(m_rotation.x / 2.f));
            m_y_rot = TRS::Quaternion(0.0f, sinf(m_rotation.y / 2.f), 0.0f, cosf(m_rotation.y / 2.f));
        }
    }
}
