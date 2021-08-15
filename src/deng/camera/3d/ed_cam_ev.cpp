/// NEKO: dynamic engine - small but powerful 2D and 3D game engine
/// licence: Apache, see LICENCE file
/// file: ed_cam_ev.cpp - editor camera event class implementation
/// author: Karl-Mihkel Ott

#define __ED_CAM_EV_CPP
#include <deng/camera/3d/ed_cam_ev.h>


namespace deng {

    __EditorCameraEv::__EditorCameraEv (
        const dengMath::vec2<deng_f64_t> &mouse_sens,
        deng_vec_t __zoom_step,
        const dengMath::vec3<deng_vec_t> &origin,
        Window *p_win
    ) : __Event3DBase (
            {
                NEKO_VCP_OVERFLOW_ACTION_TO_OPPOSITE_POSITION,
                NEKO_VCP_OVERFLOW_ACTION_BLOCK_POSITION
            },
            {
                { static_cast<deng_px_t>(-BASE_MAX_VC_X / mouse_sens.first), static_cast<deng_px_t>(BASE_MAX_VC_X / mouse_sens.first) }, 
                { static_cast<deng_px_t>(-BASE_MAX_VC_Y / mouse_sens.second), static_cast<deng_px_t>(BASE_MAX_VC_Y / mouse_sens.second)} 
            },
            {PI / 2, PI * 2},
            p_win
    ) { 
        m_p_win = p_win;
        m_zoom_step = __zoom_step;
    } 


    /// Check if any camera events have occured
    void __EditorCameraEv::__findEditorEvent() {
        deng_bool_t rot_act = __checkInputAction(DENG_CAMERA_ACTION_CHANGE_MM);
        deng_bool_t out_zoom = __checkInputAction(DENG_CAMERA_ACTION_MOV_W);
        deng_bool_t in_zoom = __checkInputAction(DENG_CAMERA_ACTION_MOV_NW);

        // Check if rotation mode should be enabled
        if(rot_act) m_editor_cam_ev = DENG_EDITOR_CAMERA_EVENT_MOUSE_ROTATE;
        else if(out_zoom) m_editor_cam_ev = DENG_EDITOR_CAMERA_EVENT_Z_MOV_OUT;
        else if(in_zoom) m_editor_cam_ev = DENG_EDITOR_CAMERA_EVENT_Z_MOV_IN;
        else m_editor_cam_ev = DENG_EDITOR_CAMERA_EVENT_NONE;
    }


    /// Zoom in the editor camera 
    void __EditorCameraEv::__zoomIn(dengMath::CameraMatrix *p_vm) {
        p_vm->moveCamera(dengMath::vec3<deng_vec_t>{0.0f, 0.0f, m_zoom_step},
            true, false, DENG_COORD_AXIS_Z);
    }


    /// Move camera position in z direction 
    void __EditorCameraEv::__zoomOut(dengMath::CameraMatrix *p_vm) {
        p_vm->moveCamera(dengMath::vec3<deng_vec_t>{0.0f, 0.0f, -m_zoom_step},
            true, false, DENG_COORD_AXIS_Z);
    }


    /// Check new editor camera events from IO bindings and perform camera updates
    void __EditorCameraEv::updateEv (
        dengMath::vec3<deng_vec_t> origin, 
        dengMath::CameraMatrix *p_vm
    ) {
        // Check if mouse data should be updated
        if(m_is_rot_cur) __Event3DBase::__updateCameraMousePos();

        /// Check if any actions were triggered according to IO bindings
        __findEditorEvent();

        // Check the occured event type and perform action accordingly
        switch(m_editor_cam_ev) {
        case DENG_EDITOR_CAMERA_EVENT_Z_MOV_IN: {
            if(m_is_rot_cur) {
                m_p_win->changeVCMode(false);
                m_is_rot_cur = false;
                m_p_win->changeCursor(DENG_CURSOR_MODE_STANDARD);
            }

            __zoomIn(p_vm);
            p_vm->setOriginRotation(origin, static_cast<deng_vec_t>(m_last_rot.first),
                static_cast<deng_vec_t>(m_last_rot.second));
            break;
        }

        case DENG_EDITOR_CAMERA_EVENT_Z_MOV_OUT: {
            if(m_is_rot_cur) {
                m_p_win->changeVCMode(false);
                m_is_rot_cur = false;
                m_p_win->changeCursor(DENG_CURSOR_MODE_STANDARD);
            }

            __zoomOut(p_vm);
            p_vm->setOriginRotation (
                origin,
                static_cast<deng_vec_t>(m_last_rot.first), 
                static_cast<deng_vec_t>(m_last_rot.second)
            );

            break;
        }

        case DENG_EDITOR_CAMERA_EVENT_MOUSE_ROTATE: {
            if(!m_is_rot_cur) {
                m_p_win->changeVCMode(true);
                m_is_rot_cur = true;
                m_p_win->changeCursor(DENG_CURSOR_MODE_POINTER);
            }

            m_last_rot = __Event3DBase::__getMouseRotation();
            p_vm->setOriginRotation(origin, static_cast<deng_vec_t>(m_last_rot.first), static_cast<deng_vec_t>(m_last_rot.second));
            break;
        }
                                              
        default:
            if(m_is_rot_cur) {
                m_p_win->changeVCMode(false);
                m_is_rot_cur = false;
                m_p_win->changeCursor(DENG_CURSOR_MODE_STANDARD);
            }
            break;
        }

        p_vm->camTransform(true);
    }


    /// Set the editor camera bindings
    void __EditorCameraEv::setBindings(const Camera3DBindings &bindings) {
        m_bindings = bindings;
    }

    
    /// Get the pointer to the window instance
    Window *__EditorCameraEv::getWinPtr() {
        return m_p_win;
    }

    
    /// Set the window pointer
    void __EditorCameraEv::setWinPtr(Window *p_win) {
        m_p_win = p_win;
    }
}
