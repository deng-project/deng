/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: fpp_cam.h - first person camera class header
/// author: Karl-Mihkel Ott


#ifndef __FPP_CAM_H
#define __FPP_CAM_H


#ifdef __FPP_CAM_CPP
    #include <stdlib.h>
    #include <mutex>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/camera/3d/cam_bindings.h>
    #include <deng/camera/3d/cam_base.h>
    #include <deng/camera/3d/ev_base.h>
    #include <deng/camera/3d/fpp_cam_ev.h>
#endif


namespace deng {

        
    /// First person perspective camera class 
    class __FPPCamera : private __FPPCameraEv, public __Camera3DBase {
    private:
        deng_bool_t m_is_pitch_ignore;

    public:
        __FPPCamera (
            const dengMath::vec3<deng_vec_t> &camera_mov_speed_mul, 
            const dengMath::vec2<deng_f64_t> &mouse_sens, 
            deng_vec_t fov, 
            deng_vec_t near_plane, 
            deng_vec_t far_plane, 
            deng_bool_t ignore_pitch_mov,
            Window *p_win
        );


        /// FPPCamera wrapper method for event update 
        void update(); 

        
        /// Set first person camera control bindings
        void setBindings(const Camera3DBindings &bindings);

        
        /// Check if camera movement system should ignore pitch rotation, when translating
        /// movements into camera coordinate system.
        deng_bool_t isPitchIgnore();

        
        /// Get the pointer to camera matrix instance
        dengMath::CameraMatrix *getCamMatPtr();
    };


}

#endif
