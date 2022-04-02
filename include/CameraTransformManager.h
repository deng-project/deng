// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: CameraTransformManager.h - camera transformation class header
// author: Karl-Mihkel Ott


#ifndef CAMERA_TRANSFORM_MANAGER_H
#define CAMERA_TRANSFORM_MANAGER_H


#ifdef CAMERA_TRANSFORM_MANAGER_CPP
    #include <cstdlib>
    #include <cstring>
    #include <cmath>
    #include <string>
#ifdef _DEBUG
    #include <iostream>
#endif

    #include <BaseTypes.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Points.h>
    #include <libdas/include/Quaternion.h>
#endif


namespace DENG {

    class CameraTransformManager {
        private:
            Libdas::Vector4<float> m_camera_pos = Libdas::Vector4<float>(0.0f, 0.0f, 0.0f, 1.0f);
            Libdas::Vector4<float> m_translation = Libdas::Vector4<float>(0.0f, 0.0f, 0.0f, 0.0f);

            // rotation quaternions relative to the world position
            Libdas::Quaternion m_abs_x_rot = Libdas::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
            Libdas::Quaternion m_abs_y_rot = Libdas::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
            Libdas::Quaternion m_abs_z_rot = Libdas::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

            // rotation quaternions relative to the camera system
            Libdas::Quaternion m_rel_x_rot = Libdas::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
            Libdas::Quaternion m_rel_y_rot = Libdas::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
            Libdas::Quaternion m_rel_z_rot = Libdas::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

            Libdas::Point3D<float> m_rotations = Libdas::Point3D<float>(0.0f, 0.0f, 0.0f);
            // camera viewport sides in world coordinates
            // NOTE: using right handed coordinate system
            Libdas::Matrix4<float> m_normalized_sides = {
                { 1.0f, 0.0f, 0.0f, 0.0f },  // right side
                { 0.0f, 1.0f, 0.0f, 0.0f },  // top side
                { 0.0f, 0.0f, 1.0f, 0.0f },  // front side
                { 0.0f, 0.0f, 0.0f, 1.0f }
            };

        private:
            void _PreventRotationOverflow();

        public:
            CameraTransformManager() = default;

            // NOTE: Relative means relative to camera coordinate set, absolute means relative to world coordinates
            
            void MoveCameraRelatively(const Libdas::Vector3<float> &_delta_mov, bool ignore_pitch_movement = false);
            void MoveCameraAbsolutely(const Libdas::Vector3<float> &_delta_mov);

            void RotateCameraRelatively(const Libdas::Point3D<float> &_delta_rot);
            void RotateCameraAbsolutely(const Libdas::Point3D<float> &_delta_rot);

            // z axis will be pointing to the given point
            void CustomPointRelativeRotation(const Libdas::Point3D<float> &_delta_rot, const Libdas::Vector4<float> &_pos);

            Libdas::Matrix4<float> ConstructViewMatrix();

            inline Libdas::Point3D<float> GetRotations() {
                return m_rotations;
            }
    };
}

#endif
