/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: cam_base.cpp - common 3D camera functionality class implementation
/// author: Karl-Mihkel Ott

#define __CAM_BASE_CPP
#include <deng/camera/3d/cam_base.h>


namespace deng {

    __Camera3DBase::__Camera3DBase (
        deng_CameraType type,
        deng_vec_t fov,
        const dengMath::vec2<deng_vec_t> &planes,
        deng_vec_t aspect_ratio
    ) : m_cam_mat(type),
        m_proj_mat(fov, planes, aspect_ratio) { m_fov = fov; }


    /// Following methods are for moving the camera position in its coordinate system
    void __Camera3DBase::moveU(deng_vec_t delta, deng_bool_t ignore_pitch) {
        m_cam_mat.moveCamera(dengMath::vec3<deng_vec_t>(delta, 0.0f, 0.0f), false,
            ignore_pitch, DENG_COORD_AXIS_X);
    }


    void __Camera3DBase::moveV(deng_vec_t delta, deng_bool_t ignore_pitch) {
        m_cam_mat.moveCamera(dengMath::vec3<deng_vec_t>(0.0f, delta, 0.0f), false,
            ignore_pitch, DENG_COORD_AXIS_Y );
    }


    void __Camera3DBase::moveW(deng_vec_t delta, deng_bool_t ignore_pitch) {
        m_cam_mat.moveCamera(dengMath::vec3<deng_vec_t>{0.0f, 0.0f, delta},
            false, ignore_pitch, DENG_COORD_AXIS_Z);
    }


    /// Following methods are for moving the camera's position in world coordinate system
    void __Camera3DBase::moveX(deng_vec_t delta, deng_bool_t ignore_pitch) {
        m_cam_mat.moveCamera(dengMath::vec3<deng_vec_t>{delta, 0.0f, 0.0f},
            true, ignore_pitch, DENG_COORD_AXIS_X);
    }


    void __Camera3DBase::moveY(deng_vec_t delta, deng_bool_t ignore_pitch) {
        m_cam_mat.moveCamera(dengMath::vec3<deng_vec_t>{0.0f, delta, 0.0f}, true,
            ignore_pitch, DENG_COORD_AXIS_Y );
    }


    void __Camera3DBase::moveZ(deng_vec_t delta, deng_bool_t ignore_pitch) {
        m_cam_mat.moveCamera(dengMath::vec3<deng_vec_t>{0.0f, 0.0f, delta}, true,
            ignore_pitch, DENG_COORD_AXIS_Z );
    }

    
    /// Following methods are for rotating the camera in its coordinate system
    void __Camera3DBase::rotU(deng_vec_t rot) {
        m_cam_mat.setCameraRotation(rot, 0);
    }


    void __Camera3DBase::rotV(deng_vec_t rot) {
        m_cam_mat.setCameraRotation(0, rot);
    }


    /// Following methods are for rotating the camera in origin specific coordinate system
    void __Camera3DBase::rotX(deng_vec_t rot) {
        m_cam_mat.setOriginRotation(dengMath::vec3<deng_vec_t>(m_origin.first, m_origin.second, m_origin.third), rot, 0);
    }


    void __Camera3DBase::rotY(deng_vec_t rot) { 
        m_cam_mat.setOriginRotation(dengMath::vec3<deng_vec_t>(m_origin.first, m_origin.second, m_origin.third), 0, rot);
    }


    dengMath::mat4<deng_vec_t> __Camera3DBase::getCamMat() { return m_cam_mat.getTransformMat(); }
    dengMath::mat4<deng_vec_t> __Camera3DBase::getProjMat() { return m_proj_mat.getProjectionMatrix(); }
}
