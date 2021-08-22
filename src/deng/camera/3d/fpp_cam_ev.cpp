/// NEKO: dynamic engine - small but powerful 2D and 3D game engine
/// licence: Apache, see LICENCE file
/// file: fpp_cam_ev.h - first person camera event class header
/// author: Karl-Mihkel Ott

#define __FPP_CAM_EV_CPP
#include <deng/camera/3d/fpp_cam_ev.h>


namespace deng {
    
    __FPPCameraEv::__FPPCameraEv (
        Window *p_win,
        const dengMath::vec2<deng_f64_t> &mouse_sens,
        const dengMath::vec3<deng_vec_t> &camera_mov_sens
    ) : __Event3DBase (
            {
                NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION, 
                NEKO_VCP_OVERFLOW_ACTION_BLOCK_POSITION
            },
            {
                { static_cast<deng_px_t>(-BASE_MAX_VC_X / mouse_sens.first), static_cast<deng_px_t>(BASE_MAX_VC_X / mouse_sens.first) }, 
			    { static_cast<deng_px_t>(-BASE_MAX_VC_Y / mouse_sens.second), static_cast<deng_px_t>(BASE_MAX_VC_Y / mouse_sens.second) }
            },
            { PI / 2, PI * 2 },
            p_win
    ) {
        m_p_win = p_win;
        m_move_speed.first = static_cast<deng_vec_t>(DENG_CAMERA_BASE_SPEED_X * camera_mov_sens.first);
        m_move_speed.second = static_cast<deng_vec_t>(DENG_CAMERA_BASE_SPEED_Y * camera_mov_sens.second);
        m_move_speed.third = static_cast<deng_vec_t>(DENG_CAMERA_BASE_SPEED_Z * camera_mov_sens.third);
        m_move_speed.fourth = 0.0f;
    }


    /// Find the current movement type and direction 
    void __FPPCameraEv::__findMovements() {
        deng_bool_t mov_nw = __checkInputAction(DENG_CAMERA_ACTION_MOV_NW);
        deng_bool_t mov_w = __checkInputAction(DENG_CAMERA_ACTION_MOV_W);
        deng_bool_t mov_nv = __checkInputAction(DENG_CAMERA_ACTION_MOV_NV);
        deng_bool_t mov_v = __checkInputAction(DENG_CAMERA_ACTION_MOV_V);
        deng_bool_t mov_nu = __checkInputAction(DENG_CAMERA_ACTION_MOV_NU);
        deng_bool_t mov_u = __checkInputAction(DENG_CAMERA_ACTION_MOV_U);

        // We are assuming right handed coordinate system
        if(mov_nw && !mov_w) m_movements.third = DENG_MOVEMENT_FORWARD;
        else if(!mov_nw && mov_w) m_movements.third = DENG_MOVEMENT_BACKWARD;
        else m_movements.third = DENG_MOVEMENT_NONE;

        if(mov_nu && !mov_u) m_movements.first = DENG_MOVEMENT_LEFTWARD;
        else if(!mov_nu && mov_u) m_movements.first = DENG_MOVEMENT_RIGHTWARD;
        else m_movements.first = DENG_MOVEMENT_NONE;

        if(!mov_nv && mov_v) m_movements.second = DENG_MOVEMENT_UPWARD;
        else if(mov_nv && !mov_v) m_movements.second = DENG_MOVEMENT_DOWNWARD;
        else m_movements.second = DENG_MOVEMENT_NONE;
    }

    
    /// Check if input FPP camera mouse input mode has changed 
    void __FPPCameraEv::__checkForInputModeChange(dengMath::CameraMatrix *p_vm) {
        __Event3DBase::__updateCameraMousePos();
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::chrono::duration<deng_ui64_t, std::milli> im_duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_input_mode_ch_beg);

        // Check if input mode should be changed ([ESC] key)
        if(im_duration.count() > NEKO_INPUT_EV_COUNT && __checkInputAction(DENG_CAMERA_ACTION_CHANGE_MM)) {
            m_p_win->toggleVCMode();
            if(m_p_win->isVCP())
                m_p_win->changeCursor(DENG_CURSOR_MODE_HIDDEN);
            else m_p_win->changeCursor(DENG_CURSOR_MODE_STANDARD);
            m_last_input_mode_ch_beg = std::chrono::system_clock::now();
        }


        // Check if virtual mouse cursor mode is enabled and 
        // if true then update camera rotation and key events
        if(m_p_win->isVCP()) {
            __findMovements();
            dengMath::vec2<deng_f64_t> rot = __Event3DBase::__getMouseRotation();
            p_vm->setCameraRotation (
                (deng_vec_t) rot.first, 
                (deng_vec_t) rot.second
            );
            p_vm->camTransform(false);
        }

        else {
            m_movements.first = DENG_MOVEMENT_NONE;
            m_movements.second = DENG_MOVEMENT_NONE;
            m_movements.third = DENG_MOVEMENT_NONE;
        }
    }


    /// Check for input mode changes and move camera if needed
    void __FPPCameraEv::updateEv(__FPPCamera *p_cam) {
        __checkForInputModeChange(p_cam->getCamMatPtr());
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::chrono::duration<deng_ui64_t, std::milli> lmov_duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_mov_beg);
        if(lmov_duration.count() > DENG_MOVEMENT_INTERVAL) {
            switch (m_movements.first) {
            case DENG_MOVEMENT_LEFTWARD:
                p_cam->moveU(-m_move_speed.first, true);
                break;

            case DENG_MOVEMENT_RIGHTWARD:
                p_cam->moveU(m_move_speed.first, true);
                break;

            case DENG_MOVEMENT_NONE: break;

            default:
                break;
            }

            switch (m_movements.second) {
            case DENG_MOVEMENT_UPWARD:
                p_cam->moveV(-m_move_speed.second, true);
                break;

            case DENG_MOVEMENT_DOWNWARD:
                p_cam->moveV(m_move_speed.second, true);
                break;

            case DENG_MOVEMENT_NONE: 
                break;

            default:
                break;
            }

            switch (m_movements.third) {
            case DENG_MOVEMENT_FORWARD:
                p_cam->moveW(-m_move_speed.first, true);
                break;

            case DENG_MOVEMENT_BACKWARD:
                p_cam->moveW(m_move_speed.first, true);
                break;

            default:
                break;
            }

            m_last_mov_beg = std::chrono::system_clock::now();
        }
    }


    /// Get the pointer to the window instance
    Window *__FPPCameraEv::getWinPtr() {
        return m_p_win;
    }

    
    /// Set the window pointer
    void __FPPCameraEv::setWinPtr(Window *p_win) {
        m_p_win = p_win;
    }


    /// Find the correct movement speed
    /// Parameters for this method are boolean flags about the axis being opposite or not
    dengMath::vec4<deng_vec_t> __FPPCameraEv::getMoveSpeed (
        deng_bool_t op_x,
        deng_bool_t op_y,
        deng_bool_t op_z
    ) { 
        dengMath::vec4<deng_vec_t> move_speed;

        if(op_x)
            move_speed.first = -m_move_speed.first;
        else move_speed.first = m_move_speed.first;

        if(op_y)
            move_speed.second = m_move_speed.second;
        else move_speed.second = -m_move_speed.second;

        if(op_z)
            move_speed.third = -m_move_speed.third;
        else move_speed.third = m_move_speed.third;

        return move_speed; 
    } 
}
