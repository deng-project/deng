// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: CameraTransformManager.cpp - camera transformation class implementation
// author: Karl-Mihkel Ott

/***** Deprecated ******/
#define CAMERA_TRANSFORM_MANAGER_CPP
#include <CameraTransformManager.h>


namespace DENG {

    void CameraTransformManager::_PreventRotationOverflow() {
        // x axis
        if(m_rotations.x > 2 * PI)
            m_rotations.x = m_rotations.x - 2 * PI;
        else if(m_rotations.x < -2 * PI) m_rotations.x += 2 * PI;

        // y axis
        if(m_rotations.y > 2 * PI)
            m_rotations.y = m_rotations.y - 2 * PI;
        else if(m_rotations.y < -2 * PI) m_rotations.y += 2 * PI;

        // z axis
        if(m_rotations.z > 2 * PI)
            m_rotations.z = m_rotations.z - 2 * PI;
        else if(m_rotations.z < -2 * PI) m_rotations.z += 2 * PI;
    }


    void CameraTransformManager::MoveCamera(const Libdas::Vector3<float> &_delta_mov, bool ignore_pitch_movement) {
        Libdas::Vector4<float> mov;
        if(!ignore_pitch_movement)
            mov = m_rel_z_rot * m_rel_y_rot * m_rel_x_rot * Libdas::Vector4<float>{ _delta_mov.first, _delta_mov.second, _delta_mov.third, 1.0f };
        else mov = m_rel_z_rot * m_rel_y_rot * Libdas::Vector4<float>(_delta_mov.first,  _delta_mov.second, _delta_mov.third, 1.0f);

        m_camera_pos += mov;
    }


    void CameraTransformManager::MoveCameraAbsolutely(const Libdas::Vector3<float> &_delta_mov) {
        m_camera_pos.first += _delta_mov.first;
        m_camera_pos.second += _delta_mov.second;
        m_camera_pos.third += _delta_mov.third;
        m_translation = m_camera_pos;
    }


    void CameraTransformManager::RotateCameraRelatively(const Libdas::Point3D<float> &_delta_rot) {
        m_rotations += _delta_rot;
        _PreventRotationOverflow();

        m_rel_x_rot = { sinf(m_rotations.x / 2), 0, 0, cosf(m_rotations.x / 2) };
        m_rel_y_rot = { 0, sinf(m_rotations.y / 2), 0, cosf(m_rotations.y / 2) };
        m_rel_z_rot = { 0, 0, sinf(m_rotations.z / 2), cosf(m_rotations.z / 2) };
    }


    void CameraTransformManager::RotateCameraAbsolutely(const Libdas::Point3D<float> &_delta_rot) {
        m_rotations += _delta_rot;
        _PreventRotationOverflow();

        m_abs_x_rot = { sinf(m_rotations.x / 2), 0, 0, cosf(m_rotations.x / 2) };
        m_abs_y_rot = { 0, sinf(m_rotations.y / 2), 0, cosf(m_rotations.y / 2) };
        m_abs_z_rot = { 0, 0, sinf(m_rotations.z / 2), cosf(m_rotations.z / 2) };
    }


    void CameraTransformManager::CustomPointRelativeRotation(const Libdas::Point3D<float> &_delta_rot, const Libdas::Vector4<float> &_point) {
        m_rotations += _delta_rot;
        _PreventRotationOverflow();

        m_rel_x_rot = { sinf(m_rotations.x / 2), 0, 0, cosf(m_rotations.x / 2) };
        m_rel_y_rot = { 0, sinf(m_rotations.y / 2), 0, cosf(m_rotations.y / 2) };
        m_rel_z_rot = { 0, 0, sinf(m_rotations.z / 2), cosf(m_rotations.z / 2) };
    }


    Libdas::Matrix4<float> CameraTransformManager::ConstructViewMatrix() {
        Libdas::Matrix4<float> view = m_normalized_sides;
        view.row1.fourth = -(m_translation.first - m_translation_offset.first);
        view.row2.fourth = -(m_translation.second - m_translation_offset.second);
        view.row3.fourth = -(m_translation.third - m_translation_offset.third);

        view = view * (m_abs_z_rot * m_abs_y_rot * m_abs_x_rot).ExpandToMatrix4();

        return view.Transpose();
    }
}
