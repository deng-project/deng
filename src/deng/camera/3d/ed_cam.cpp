/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: ed_cam.cpp - editor camera class implementation
/// author: Karl-Mihkel Ott


#define __ED_CAM_CPP
#include <deng/camera/3d/ed_cam.h>


namespace deng {

    __EditorCamera::__EditorCamera (
        deng_vec_t zoom_step,
        const dengMath::vec3<deng_vec_t> &origin,
        const dengMath::vec2<deng_f64_t> &mouse_sens,
        deng_vec_t fov,
        deng_vec_t near_plane,
        deng_vec_t far_plane,
        Window *p_ww
    ) : __EditorCameraEv (
            mouse_sens, 
            zoom_step, 
            origin, 
            p_ww
        ), 
        __Camera3DBase (
            DENG_CAMERA_TYPE_EDITOR,
            fov,
            {near_plane, far_plane},
            (deng_vec_t) __EditorCameraEv::getWinPtr()->getSize().first / (deng_vec_t) __EditorCameraEv::getWinPtr()->getSize().second
        ) {
        m_origin = origin;
        setWinPtr(p_ww);
        getWinPtr()->changeVCMode(false);
    }


    /// Move origin point in world coordinate system by delta_mov
    void __EditorCamera::moveOrigin(const dengMath::vec3<deng_vec_t> &delta_mov) {
        m_origin += delta_mov;
    }


    /// Set camera control bindings for editor camera system
    void __EditorCamera::setBindings(const Camera3DBindings &bindings) {
        m_bindings = bindings;
    }


    /// Wrapper method for updating camera events
    void __EditorCamera::update() {
        __EditorCameraEv::updateEv(m_origin, &m_cam_mat);
    }
}
