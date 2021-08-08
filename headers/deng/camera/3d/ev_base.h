/// NEKO: dynamic engine - powerful 2D and 3D game engine
/// licence: Apache, see LICENCE file
/// file: ev_base.h - 3D camera base event class header
/// author: Karl-Mihkel Ott


#ifdef __EV_BASE_CPP
    #include <string>
    #include <vulkan/vulkan.h>
    #include <common/base_types.h>
    #include <common/err_def.h>

    #include <data/assets.h>
    #include <math/deng_math.h>
    #include <deng/window.h>
    
    #include <deng/camera/3d/cam_bindings.h>
#endif


#ifndef __EV_BASE_H
#define __EV_BASE_H


#define BASE_MAX_VC_X   720
#define BASE_MAX_VC_Y   180


namespace deng {

    class __Event3DBase {
    protected:
        Window *m_p_win = NULL;
        dengMath::vec2<deng_px_t> m_mouse_pos = dengMath::vec2<deng_px_t>{0, 0};
        dengMath::vec2<neko_VCPOverflowAction> m_vcp_overflow;
        dengMath::vec2<dengMath::vec2<deng_i64_t>> m_vc_bounds;
        dengMath::vec2<deng_f64_t> m_max_rot;
        Camera3DBindings m_bindings;

    protected:

        /// Camera mouse position update method 
        void __updateCameraMousePos();


        /// Find the current mouse control rotation 
        dengMath::vec2<deng_f64_t> __getMouseRotation();


        /// Check if input input conditions are satified for certain action
        deng_bool_t __checkInputAction(deng_CameraAction action);

    public:
        __Event3DBase (
            const dengMath::vec2<neko_VCPOverflowAction> &vcp_act,
            const dengMath::vec2<dengMath::vec2<deng_px_t>> &vc_bounds,
            const dengMath::vec2<deng_f64_t> &max_rot,
            Window *p_win
        );
    };
}

#endif
