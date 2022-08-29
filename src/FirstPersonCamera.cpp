// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: FirstPersonCamera.cpp - first person camera class implementation
// author: Karl-Mihkel Ott

#define FIRST_PERSON_CAMERA_CPP
#include "deng/FirstPersonCamera.h"

namespace DENG {

    FirstPersonCamera::FirstPersonCamera(Entity *_parent, const std::string &_name, Renderer &_rend, FirstPersonCameraConfiguration &_conf) :
        Camera3D(_parent, _name, ENTITY_TYPE_FIRST_PERSON_CAMERA, m_renderer),
        m_config(_conf) {}


    FirstPersonCamera::~FirstPersonCamera() {
        if (_OnDestroyFunction)
            _OnDestroyFunction(m_script);
    }


    void FirstPersonCamera::_EnforceLimits() {
        if (m_rotation.x < -PI / 2.f)
            m_rotation.x = -PI / 2.f;
        else if (m_rotation.x > PI / 2.f)
            m_rotation.x = PI / 2.f;

        if (m_rotation.y < -PI)
            m_rotation.y = 2.f * PI - m_rotation.y;
        else if (m_rotation.y > PI)
            m_rotation.y = -2.f * PI + m_rotation.y;
    }


    void FirstPersonCamera::_ConstructViewMatrix() {
        TRS::Matrix4<float> translation = {
            { 1.0f, 0.0f, 0.0f, m_translation.first },
            { 0.0f, 1.0f, 0.0f, m_translation.second },
            { 0.0f, 0.0f, 1.0f, m_translation.third },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };

        m_ubo.view_matrix = (m_x_rot * m_y_rot).ExpandToMatrix4() * translation;
        m_ubo.view_matrix = m_ubo.view_matrix.Transpose();
    }


    void FirstPersonCamera::Attach() {
        GPUMemoryManager* mem_manager = GPUMemoryManager::GetInstance();
        m_ubo_offset = mem_manager->RequestUniformMemoryLocationP(m_renderer, sizeof(ModelCameraUbo));
        SetAttachedBit(true);

        if (_OnAttachFunction)
            _OnAttachFunction(m_script);
    }


    void FirstPersonCamera::Update() {
        m_ubo.projection_matrix = _CalculateProjection();

        if (_OnUpdateFunction)
            _OnUpdateFunction(m_script);

        m_beg_time = std::chrono::high_resolution_clock::now();
        _ConstructViewMatrix();
        m_renderer.UpdateUniform(reinterpret_cast<const char*>(&m_ubo), sizeof(ModelCameraUbo), m_ubo_offset);
    }


    void FirstPersonCamera::Move(TRS::Vector3<bool> _mov) {
        TRS::Vector4<float> delta_mov;

        const float ts = _GetTimeDeltaMS() / 1000.f;
        TRS::Vector4<float> ymov = { 0.0f, (_mov.second ? m_config.speed * ts : 0.0f), 0.0f, 0.0f};
        
        if (_mov.first)
            delta_mov.first = m_config.speed * ts;
        if (_mov.third)
            delta_mov.third = m_config.speed * ts;

        if (m_is_enabled) {
            const float ts = _GetTimeDeltaMS();
            m_translation += (m_y_rot * delta_mov);
            m_translation += ymov;
        }
    }


    void FirstPersonCamera::Rotate(TRS::Point2D<float> _threshold) {
        if (m_is_enabled) {
            const float ts = _GetTimeDeltaMS();
            m_rotation.x += ts * m_config.max_threshold_rotation.y * _threshold.y / m_config.max_thresholds.y;
            m_rotation.y += ts * m_config.max_threshold_rotation.x * _threshold.x / m_config.max_thresholds.x;

            _EnforceLimits();
            m_x_rot = TRS::Quaternion(sinf(m_rotation.x / 2.f), 0.0f, 0.0f, cosf(m_rotation.y / 2.f));
            m_y_rot = TRS::Quaternion(0.0f, sinf(m_rotation.y / 2.f), 0.0f, cosf(m_rotation.y / 2.f));
        }
    }
}
