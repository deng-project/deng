/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: ed_cam.h - editor camera class header
/// author: Karl-Mihkel Ott


#ifndef __ED_CAM_H
#define __ED_CAM_H


#ifdef __ED_CAM_CPP
    #include <stdlib.h>
    #include <string.h>
    #include <mutex>
    #include <cmath>
    #include <string>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/camera/3d/cam_bindings.h>
    #include <deng/camera/3d/cam_base.h>
    #include <deng/camera/3d/ev_base.h>
    #include <deng/camera/3d/ed_cam_ev.h>
#endif

namespace deng {

    /// Main class for editor camera instance creation
    class __EditorCamera : private __EditorCameraEv, public __Camera3DBase {
    private:
        dengMath::vec3<deng_vec_t> m_origin;

    public:
        __EditorCamera(deng_vec_t zoom_step, const dengMath::vec3<deng_vec_t> &origin, const dengMath::vec2<deng_f64_t> &mouse_sens, 
            deng_vec_t FOV, deng_vec_t near_plane, deng_vec_t far_plane, Window *p_ww );
        

        /// Move origin point in world coordinate system by delta_mov
        void moveOrigin(const dengMath::vec3<deng_vec_t> &delta_mov); 

        
        /// Set camera control bindings for editor camera system
        void setBindings(const Camera3DBindings &bindings);


        /// Wrapper method for updating camera events
        void update();
    };
}

#endif
