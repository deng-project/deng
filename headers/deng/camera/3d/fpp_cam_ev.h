/// NEKO: dynamic engine - small but powerful 2D and 3D game engine
/// licence: Apache, see LICENCE file
/// file: fpp_cam_ev.h - first person camera event class header
/// author: Karl-Mihkel Ott

#ifndef __FPP_CAM_EV_H
#define __FPP_CAM_EV_H


#ifdef __FPP_CAM_EV_CPP
    #include <string>
    #include <string.h>
    #include <iostream>
    #include <chrono>

    #include <vulkan/vulkan.h>
    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/camera/3d/cam_bindings.h>
    #include <deng/camera/3d/ev_base.h>

    //
#endif


namespace deng {

    /// FPPCamera forward declaration, needed in update method
    class __FPPCamera;


    /// Perspective first person camera event class
    class __FPPCameraEv : protected __Event3DBase {
    private:
        // NEED TO BE REPLACED WITH STD::CHRONO CALLS ! ! !
        //std::chrono::duration<deng_ui64_t, std::milli> m_mov_timer = std::chrono::milliseconds(0); 
        //std::chrono::duration<deng_ui64_t, std::milli> m_input_mode_timer = std::chrono::milliseconds(0);
        std::chrono::system_clock::time_point m_last_mov_beg = std::chrono::system_clock::now();
        std::chrono::system_clock::time_point m_last_input_mode_ch_beg = std::chrono::system_clock::now();
        dengMath::vec4<deng_vec_t> m_move_speed;
        dengMath::vec3<deng_MovementEvent> m_movements;

    private:
        void __findMovements();
        void __checkForInputModeChange(dengMath::CameraMatrix *p_cm);
        void __update(__FPPCamera *p_cam);
        deng_bool_t __keyEvHandler(neko_Key key);

    public:
        __FPPCameraEv (
            Window *p_win,
            const dengMath::vec2<deng_f64_t> &mouse_sens,
            const dengMath::vec3<deng_vec_t> &camera_mov_speed
        );


        /// Find the correct movement speed
        /// Parameters for this method are boolean flags about the axis being opposite or not
        dengMath::vec4<deng_vec_t> getMoveSpeed(deng_bool_t op_x,
            deng_bool_t op_y, deng_bool_t op_z);


        /// Check for input mode changes and move camera if needed
        void updateEv(__FPPCamera *p_cam, deng_bool_t ignore_pitch);


        /// Set the first person perspective camera bindings
        void setBindings(const Camera3DBindings &bindings);

        
        /// Get the pointer to the window instance
        Window *getWinPtr();

        
        /// Set the window pointer
        void setWinPtr(Window *p_win);
    };
}


#ifdef __FPP_CAM_EV_CPP
    #include <deng/camera/3d/cam_base.h>
    #include <deng/camera/3d/fpp_cam.h>
#endif


#endif
