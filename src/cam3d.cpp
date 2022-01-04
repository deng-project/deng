/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: cam3d.cpp - class header for handling multiple 3D camera modes
/// author: Karl-Mihkel Ott

#define __CAMERA_CPP
#include <deng/cam3d.h>


namespace deng {

    Camera3D::Camera3D (
        deng_CameraType cam_type,
        deng_vec_t fov,
        const dengMath::vec2<deng_vec_t> &planes,
        const dengMath::vec3<deng_vec_t> &mov_speed,
        const dengMath::vec2<deng_f64_t> &rot_sens,
        Window *win
    ) {
        m_p_win = win;
        m_cam_type = cam_type;

        // Check for camera instance to create
        switch(m_cam_type) {
        case DENG_CAMERA_TYPE_FPP:
            m_fpp_cam = std::make_unique<__FPPCamera>(mov_speed, rot_sens, fov, planes.first, planes.second, m_p_win);
            break;

        case DENG_CAMERA_TYPE_EDITOR: {
            deng_vec_t zoom_step = __DENG_DEFAULT_ZOOM_STEP * (mov_speed.first + mov_speed.second + mov_speed.third) / 3;
            m_ed_cam = std::make_unique<__EditorCamera>(zoom_step, dengMath::vec3<deng_vec_t>(0.0f, 0.0f, 0.0f), rot_sens, fov,
                planes.first, planes.second, m_p_win);
            break;
        }   

        default:
            break;
        }
    }


    /// Set the camera key bindings
    /// NOTE: These bindings are the ones that control camera movement and rotation actions
    void Camera3D::setBindings(const Camera3DBindings &bindings) {
        switch(m_cam_type) {
        case DENG_CAMERA_TYPE_FPP:
            m_fpp_cam->setBindings(bindings);
            break;

        case DENG_CAMERA_TYPE_EDITOR:
            m_ed_cam->setBindings(bindings);
            break;

        default:
            break;
        }
    }


    /// Move camera's origin in world space by delta_mov
    /// NOTE: This method only effects editor camera and third person perspective camera
    /// systems. It does nothing on the first person camera.
    void Camera3D::moveOrigin(const dengMath::vec3<deng_vec_t> &delta_mov) {
        switch(m_cam_type)
        {
        case DENG_CAMERA_TYPE_EDITOR:
            m_ed_cam->moveOrigin(delta_mov);
            break;

        /// Reserved for future use
        default:
            break;
        }
    }


    /// Move camera's position by delta_mov in camera's coordinate system
    void Camera3D::moveCameraPOVC (
        const dengMath::vec3<deng_vec_t> &delta_mov,
        deng_bool_t ignore_pitch
    ) {
        switch(m_cam_type) {
        case DENG_CAMERA_TYPE_FPP:
            if(delta_mov.first)
                m_fpp_cam->moveU(delta_mov.first, ignore_pitch);
            if(delta_mov.second)
                m_fpp_cam->moveV(delta_mov.second, ignore_pitch);
            if(delta_mov.third)
                m_fpp_cam->moveW(delta_mov.third, ignore_pitch);
            break;

        case DENG_CAMERA_TYPE_EDITOR:
            if(delta_mov.first)
                m_ed_cam->moveU(delta_mov.first, ignore_pitch);
            if(delta_mov.second)
                m_ed_cam->moveV(delta_mov.second, ignore_pitch);
            if(delta_mov.third)
                m_ed_cam->moveW(delta_mov.third, ignore_pitch);
            break;

        default:
            break;
        }
    }


    /// Move camera's position in world coordinate system
    void Camera3D::moveCameraPOVW (
        const dengMath::vec3<deng_vec_t> &delta_mov,
        deng_bool_t ignore_pitch
    ) {
        switch(m_cam_type) {
        case DENG_CAMERA_TYPE_FPP:
            if(delta_mov.first)
                m_fpp_cam->moveX(delta_mov.first, ignore_pitch);
            if(delta_mov.second)
                m_fpp_cam->moveY(delta_mov.second, ignore_pitch);
            if(delta_mov.third)
                m_fpp_cam->moveZ(delta_mov.third, ignore_pitch);
            break;

        case DENG_CAMERA_TYPE_EDITOR:
            if(delta_mov.first)
                m_ed_cam->moveX(delta_mov.first, ignore_pitch);
            if(delta_mov.second)
                m_ed_cam->moveY(delta_mov.second, ignore_pitch);
            if(delta_mov.third)
                m_ed_cam->moveZ(delta_mov.third, ignore_pitch);
            break;

        default:
            break;
        }
    }


    /// Rotate camera's position by rot in camera's coordinate system
    void Camera3D::rotCameraPOVC(const dengMath::vec2<deng_vec_t> &rot) {
        switch(m_cam_type) {
        case DENG_CAMERA_TYPE_EDITOR:
            m_ed_cam->rotU(rot.first);
            m_ed_cam->rotV(rot.second);
            break;

        case DENG_CAMERA_TYPE_FPP:
            m_fpp_cam->rotU(rot.first);
            m_fpp_cam->rotV(rot.second);
            break;

        default:
            break;
        }
    }


    /// Rotate camera's position by rot in world's coordinate system
    void Camera3D::rotCameraPOVW(const dengMath::vec2<deng_vec_t> &rot) {
        switch(m_cam_type) {
        case DENG_CAMERA_TYPE_EDITOR:
            m_ed_cam->rotX(rot.first);
            m_ed_cam->rotY(rot.second);
            break;

        case DENG_CAMERA_TYPE_FPP:
            m_fpp_cam->rotX(rot.first);
            m_fpp_cam->rotY(rot.second);
            break;

        default:
            break;
        }
    }


    /// Main camera system update method
    void Camera3D::update() {
        switch(m_cam_type) {
        case DENG_CAMERA_TYPE_EDITOR:
            m_ed_cam->update();
            break;

        case DENG_CAMERA_TYPE_FPP:
            m_fpp_cam->update();
            break;

        default:
            break;
        }
    }


    /// Get the camera type of the current camera instance
    deng_CameraType Camera3D::getType() {
        return m_cam_type;
    }


    /// Get the dot product of camera's view and projection matrix
    dengMath::mat4<deng_vec_t> Camera3D::getCameraMat() {
        switch(m_cam_type) {
        case DENG_CAMERA_TYPE_EDITOR:
            return m_ed_cam->getCamMat() * m_ed_cam->getProjMat();

        case DENG_CAMERA_TYPE_FPP:
            return m_fpp_cam->getCamMat() * m_fpp_cam->getProjMat();

        default:
            return dengMath::mat4<deng_vec_t>();
        }
    }


    /// Get the bare camera view matrix
    dengMath::mat4<deng_vec_t> Camera3D::getViewMat() {
        switch(m_cam_type) {
        case DENG_CAMERA_TYPE_EDITOR:
            return m_ed_cam->getCamMat();

        case DENG_CAMERA_TYPE_FPP:
            return m_fpp_cam->getCamMat();

        default:
            return dengMath::mat4<deng_vec_t>();
        }
    }


    /// Returns true if camera has perspective view
    deng_bool_t Camera3D::isPerspective() { return true; }
}
