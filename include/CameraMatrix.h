// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: CameraMatrix.h - camera matrix class header
// author: Karl-Mihkel Ott


#ifndef CAMERA_MATRIX_H
#define CAMERA_MATRIX_H


#ifdef CAMERA_MATRIX_CPP
    #include <type_traits>
    #include <stdlib.h>
    #include <string.h>
    #include <cmath>

    #include <BaseTypes.h>
    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Quaternion.h>
    
    #define CAMERA_RIGHT     Libdas::Vector4<float>(1.0f, 0.0f, 0.0f, 0.0f);
    #define CAMERA_UP        Libdas::Vector4<float>(0.0f, 1.0f, 0.0f, 0.0f);
    #define CAMERA_FORWARD   Libdas::Vector4<float>{0.0f, 0.0f, -1.0f, 0.0f};
#endif


namespace DENG {

    class CameraMatrix {
    private:
        Libdas::Vector4<float> m_camera_pos;
        Libdas::Matrix4<float> m_camera_mat;

        float m_x_rot;
        float m_y_rot;

        Libdas::Vector4<float> m_rs = Libdas::Vector4<float>(1.0f, 0.0f, 0.0f, 0.0f);
        Libdas::Vector4<float> m_ts = Libdas::Vector4<float>(0.0f, 1.0f, 0.0f, 0.0f);
        Libdas::Vector4<float> m_fs = Libdas::Vector4<float>{0.0f, 0.0f, -1.0f, 0.0f};

        // Camera coordinate specific rotation matrices
        Libdas::Quaternion m_x_quaternion;
        Libdas::Quaternion m_y_quaternion;

    public:
        CameraMatrix(CameraType type);

        /// Move camera by one movement step
        void MoveCamera(const Libdas::Vector3<float> &mov_speed, bool is_world, bool ignore_pitch, const CoordinateAxisType &movement_type);


        /// Create transformation matrix for camera system based on previously submitted values
        void CameraTransform(bool is_world_origin);

    // Inlined methods
    public: 
        /// Force set camera position to a new one
        inline void SetCameraPosition(const Libdas::Vector4<float> &camera_pos) {
            m_camera_pos = camera_pos;
        }

        /// Set new rotation for the camera relative to the its coordinate system
        inline void SetCameraRotation(float x_rot, float y_rot) {
            m_x_rot = x_rot;
            m_y_rot = y_rot;

            m_x_quaternion = Libdas::Quaternion(sinf(x_rot / 2), 0, 0, cosf(x_rot / 2));
            m_y_quaternion = Libdas::Quaternion(0, sinf(y_rot / 2), 0, cosf(y_rot / 2));
        }


        /// Set new rotation for the camera relative to its origin point in world coordinates
        inline void SetOriginRotation(Libdas::Point3D<float> point, float x_rot, float y_rot);
        
        /// Get the transformation matrix
        inline Libdas::Matrix4<float> GetTransformMatrix() {
            return m_camera_mat.Transpose();
        }


        /// Get the current position of the camera
        inline Libdas::Vector4<float> GetPosition() {
            return m_camera_pos;
        }

        
        /// Get current camera sides' coordinates in RTF matrix representation
        inline void GetSides() {
            return Libdas::Matrix3<float> { m_rs, m_ts, m_fs };
        }
    };
}

#endif
