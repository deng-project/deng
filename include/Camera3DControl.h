// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Camera3DControl.h - Common 3D camera functionality class header
// author: Karl-Mihkel Ott

#ifndef CAMERA_3D_CONTROL_H
#define CAMERA_3D_CONTROL_H


namespace DENG {
    
    class Camera3DControl {
        private:
            float m_fov; // Radians
            Libdas::Point2D<float> m_planes; // near and far planes
            float m_aspect_ratio;

            Libdas::Vector3<float> m_origin = Libdas::Vector3<float>(0.0f, 0.0f, 0.0f); // Origin doesn't accept transformations
            CameraMatrix m_cam_mat;
            Libdas::Matrix4<float> m_proj_mat;

        private:
            inline Libdas::Matrix4<float> _CalculateProjection() {
                return Libdas::Matrix4<float> {
                    { m_aspect_ratio / tanf(m_fov / 2), 0.0f, 0.0f, 0.0f },
                    { 0.0f, 1 / tanf(m_fov / 2), 0.0f, 0.0f },
                    { 0.0f, 0.0f, m_planes.y / (m_planes.x + m_planes.y), 1.0f },
                    { 0.0f, 0.0f, (m_planes.x * m_planes.y) / (m_planes.x + m_planes.y), 0.0f }
                };
            }

        public:
            Camera3DControl(float fov, const Libdas::Point2D<float> &planes, float aspect_ratio) : 
                m_fov(fov), 
                m_planes(planes), 
                m_aspect_ratio(aspect_ratio), 
                m_cam_mat(type), 
                m_proj_mat(_CalculateProjection()) {}


            /// Following methods are for moving the camera position in its coordinate system
            inline void MoveU(float delta, bool ignore_pitch) {
                m_cam_mat.MoveCamera(Libdas::Vector3<float>(delta, 0.0f, 0.0f), false, ignore_pitch, DENG_COORD_AXIS_X);
            }

            inline void MoveV(float delta, bool ignore_pitch) {
                m_cam_mat.MoveCamera(Libdas::Vector3<float>(0.0f, delta, 0.0f), false, ignore_pitch, DENG_COORD_AXIS_Y );
            }

            inline void MoveW(float delta, bool ignore_pitch) {
                m_cam_mat.MoveCamera(Libdas::Vector3<float>{0.0f, 0.0f, delta}, false, ignore_pitch, DENG_COORD_AXIS_Z);
            }


            /// Following methods are for moving the camera position in world coordinate system
            inline void MoveX(float delta, bool ignore_pitch) {
                m_cam_mat.moveCamera(Libdas::Vector3<float>{delta, 0.0f, 0.0f}, true, ignore_pitch, DENG_COORD_AXIS_X);
            }

            inline void MoveY(float delta, bool ignore_pitch) {
                m_cam_mat.MoveCamera(Libdas::Vector3<deng_vec_t>{0.0f, delta, 0.0f}, true, ignore_pitch, DENG_COORD_AXIS_Y );
            }

            inline void MoveZ(float delta, bool ignore_pitch) {
                m_cam_mat.MoveCamera(Libdas::Vector3<deng_vec_t>{0.0f, 0.0f, delta}, true, ignore_pitch, DENG_COORD_AXIS_Z );
            }

            
            /// Following methods are for rotating the camera in its coordinate system
            inline void RotateU(float rot) {
                m_cam_mat.SetCameraRotation(rot, 0);
            }

            inline void RotateV(float rot) {
                m_cam_mat.SetCameraRotation(0, rot);
            }


            /// Following methods are for rotating the camera in origin specific coordinate system
            inline void RotateX(float rot) {
                m_cam_mat.SetOriginRotation(Libdas::Vector3<deng_vec_t>(m_origin.first, m_origin.second, m_origin.third), rot, 0);
            }

            inline void RotateY(float rot) {
                m_cam_mat.SetOriginRotation(Libdas::Vector3<deng_vec_t>(m_origin.first, m_origin.second, m_origin.third), 0, rot);
            }


            inline Libdas::Matrix4<float> &GetCameraMatrix() {
                return m_cam_mat.GetTransformationMatrix();
            }

            inline Libdas::Matrix4<float> &GetProjectionMatrix() {
                return m_proj_mat;
            }
    };
}

#endif
