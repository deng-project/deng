/// DENG: dynamic engine - small but powerful 2D and 3D game engine
/// licence: Apache, see LICENCE file
/// file: CameraMatrix.h - Camera matrix class implementation
/// author: Karl-Mihkel Ott


#define CAMERA_MATRIX_CPP
#include <CameraMatrix.h>


namespace DENG {
    
    CameraMatrix::CameraMatrix(CameraType cam_type) {
        m_camera_pos = Libdas::Vector4<float>{
            DENG_EDITOR_CAMERA_DEFAULT_POS_X,
            DENG_EDITOR_CAMERA_DEFAULT_POS_Y,
            DENG_EDITOR_CAMERA_DEFAULT_POS_Z,
            1.0f
        };
        m_x_rot = 0.0f;
        m_y_rot = 0.0f;

        // Set the default camera positions and rotations according to the camera type specified
        switch(cam_type) {
            case DENG_CAMERA_TYPE_FPP:
                CameraTransform(false);
                break;

            case DENG_CAMERA_TYPE_EDITOR:
                CameraTransform(true);
                break;
        }
    }


    /// Move camera by one movement step
    void CameraMatrix::MoveCamera(const Libdas::Vector3<float> &mov_speed, bool is_world, bool ignore_pitch, const CoordinateAxisType &movement_type) {
        Libdas::Vector4<float> mov = {0.0f, 0.0f, 0.0f, 0.0f};

        switch(movement_type) {
        case DENG_COORD_AXIS_X:
            mov.first = mov_speed.first;
            if(ignore_pitch && !is_world)
                mov = m_x_quaternion * mov;
            else if(!is_world) mov = m_y_quaternion * m_x_quaternion * mov;
            break;

        case DENG_COORD_AXIS_Y:
            mov.second = mov_speed.second;
            if(ignore_pitch && !is_world)
                mov = m_y_quaternion * mov;
            else if(!is_world) mov = m_y_quaternion * m_x_quaternion * mov;
            break;

        case DENG_COORD_AXIS_Z:
            mov.third = mov_speed.third;
            if(ignore_pitch && !is_world)
                mov = m_y_quaternion * mov;
            else if(!is_world) mov = m_y_quaternion * m_x_quaternion * mov;
            break;

        case DENG_COORD_AXIS_UNDEFINED:
            mov.first = mov_speed.first;
            mov.second = mov_speed.second;
            mov.third = mov_speed.third;

            if(ignore_pitch && !is_world)
                mov = m_y_quaternion * mov;
            else if(!is_world) mov = m_y_quaternion * m_x_quaternion * mov;
            break;
        
        default:
            break;
        }

        m_camera_pos += Libdas::Vector4<float>(mov.first, mov.second, mov.third, 0.0f);
    }


    /// Set new rotation for the camera relative to its origin point in world coordinates
    void CameraMatrix::SetOriginRotation(Libdas::Point3D<float> point, float x_rot, float y_rot) {
        m_x_rot = x_rot;
        m_y_rot = y_rot;

        m_x_quaternion = Libdas::Quaternion(sinf(m_x_rot / 2), 0, 0, cosf(m_x_rot / 2));
        m_y_quaternion = Libdas::Quaternion(0, sinf(m_y_rot / 2), 0, cosf(m_y_rot / 2));

        Libdas::Matrix4<float> tmat = Libdas::Matrix4<float> {
            { 1.0f, 0.0f, 0.0f, m_camera_pos.first - point.x },
            { 0.0f, 1.0f, 0.0f, m_camera_pos.second - point.y },
            { 0.0f, 0.0f, 1.0f, m_camera_pos.third - point.z },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };

        Libdas::Matrix4<float> xmat = m_x_quaternion.ExpandToMatrix4();
        Libdas::Matrix4<float> ymat = m_y_quaternion.ExpandToMatrix4();

        m_x_quaternion = Libdas::Quaternion::MatrixToQuaternion(tmat * m_x_quaternion.ExpandToMatrix4() * tmat.Inverse());
        m_y_quaternion = Libdas::Quaternion::MatrixToQuaternion(tmat * m_y_quaternion.ExpandToMatrix4() * tmat.Inverse());
    }


    /// Create transformation matrix for camera system based
    /// on previously submitted values
    void CameraMatrix::CameraTransform(bool is_world_origin) {
        if(!is_world_origin) {
            m_rs = m_y_quaternion * m_x_quaternion * CAMERA_RIGHT;
            m_fs = m_y_quaternion * m_x_quaternion * CAMERA_FORWARD;
            m_ts = m_y_quaternion * m_x_quaternion * CAMERA_UP;

            //mat4<deng_vec_t> basis;
            Libdas::Vector3<float> translation = { -(m_rs * m_camera_pos), -(m_ts * m_camera_pos), -(m_fs * m_camera_pos) };

            m_camera_mat = Libdas::Matrix4<float> {
                {m_rs.first, m_rs.second, m_rs.third, translation.first},
                {m_ts.first, m_ts.second, m_ts.third, translation.second},
                {m_fs.first, m_fs.second, m_fs.third, translation.third},
                {0.0f, 0.0f, 0.0f, 1.0f}
            };
        }

        else {
            m_rs = m_y_quaternion * m_x_quaternion * CAMERA_RIGHT;
            m_ts = m_y_quaternion * m_x_quaternion * CAMERA_UP;
            m_fs = m_y_quaternion * m_x_quaternion * CAMERA_FORWARD;
            Libdas::Vector4<float> translation = m_y_quaternion * m_x_quaternion * m_camera_pos;

            m_camera_mat = Libdas::Matrix4<float> {
                {m_rs.first, m_rs.second, m_rs.third, translation.first},
                {m_ts.first, m_ts.second, m_ts.third, translation.second},
                {m_fs.first, m_fs.second, m_fs.third, translation.third},
                {0.0f, 0.0f, 0.0f, 1.0f}
            };
        }
    }
}
