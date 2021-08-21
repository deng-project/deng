/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: fpp_cam.cpp - first person camera class implementation
/// author: Karl-Mihkel Ott


#define __FPP_CAM_CPP
#include <deng/camera/3d/fpp_cam.h>


namespace deng {

    __FPPCamera::__FPPCamera (
        const dengMath::vec3<deng_vec_t> &camera_mov_sens, 
		const dengMath::vec2<deng_f64_t> &mouse_sens, 
		deng_vec_t fov, 
		deng_vec_t near_plane, 
		deng_vec_t far_plane, 
        deng_bool_t ignore_pitch_mov,
		Window *p_win
	) : __FPPCameraEv (
            p_win,
            mouse_sens, 
            camera_mov_sens
        ),
        __Camera3DBase (
            DENG_CAMERA_TYPE_FPP,
            fov,
            {near_plane, far_plane},
            static_cast<deng_vec_t>(__FPPCameraEv::getWinPtr()->getSize().first) / static_cast<deng_vec_t>(__FPPCameraEv::getWinPtr()->getSize().second)
        ) {
        __FPPCameraEv::setWinPtr(p_win);
    }


    /// __FPPCamera wrapper method for event update 
    void __FPPCamera::update() {
        __FPPCameraEv::updateEv(this, m_is_pitch_ignore);
    }

    
    /// Set first person camera control bindings
    void __FPPCamera::setBindings(const Camera3DBindings &bindings) {
        m_bindings = bindings;
    }

    
    /// Check if camera movement system should ignore pitch rotation, when translating
    /// movements into camera coordinate system.
    deng_bool_t __FPPCamera::isPitchIgnore() {
        return m_is_pitch_ignore;
    }


    /// Get the pointer to camera matrix instance
    dengMath::CameraMatrix *__FPPCamera::getCamMatPtr() {
        return &m_cam_mat;
    }
}
