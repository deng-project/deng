/// NEKO: dynamic engine - small but powerful 2D and 3D game engine
/// licence: Apache, see LICENCE file
/// file: ev_base.cpp - 3D camera base event class implementation
/// author: Karl-Mihkel Ott

#define __EV_BASE_CPP
#include <deng/camera/3d/ev_base.h>


namespace deng {

    __Event3DBase::__Event3DBase (
		const dengMath::vec2<neko_VCPOverflowAction> &vcp_act,
		const dengMath::vec2<dengMath::vec2<deng_i64_t>> &vc_bounds,
		const dengMath::vec2<deng_f64_t> &max_rot,
        Window *p_win
    ) {
        m_vcp_overflow = vcp_act;
        m_vc_bounds = vc_bounds;
        m_max_rot = max_rot;
        m_p_win = p_win;

        neko_LimitVirtualPos(vc_bounds.first.second, vc_bounds.first.first, vc_bounds.second.second, vc_bounds.second.first);
        neko_SetOverflowAction(vcp_act.first, vcp_act.second);
    }


    /// Camera mouse update method 
    void __Event3DBase::__updateCameraMousePos() {
        m_mouse_pos = m_p_win->getMPos();
    }


    /// Find the current mouse rotation 
    dengMath::vec2<deng_f64_t> __Event3DBase::__getMouseRotation() {
        dengMath::vec2<deng_f64_t> out_rot;
        out_rot.first = static_cast<deng_f64_t>(m_mouse_pos.second) / static_cast<deng_f64_t>(m_vc_bounds.second.second) * m_max_rot.first;
        out_rot.second = static_cast<deng_f64_t>(m_mouse_pos.first) / static_cast<deng_f64_t>(m_vc_bounds.first.second) * m_max_rot.second; 
        return out_rot;
    }

        
    /// Check if input conditions are satisfied for certain action
    deng_bool_t __Event3DBase::__checkInputAction(deng_CameraAction action) {
        neko_InputEv *evs = NULL;

        // Check the current requested action type and unmask its bindings into array
        switch(action) {
        case DENG_CAMERA_ACTION_MOV_U:
            evs = neko_UnmaskInput(m_bindings.mov_u);
            break;
        case DENG_CAMERA_ACTION_MOV_NU:
            evs = neko_UnmaskInput(m_bindings.mov_nu);
            break;
        case DENG_CAMERA_ACTION_MOV_V:
            evs = neko_UnmaskInput(m_bindings.mov_v);
            break;
        case DENG_CAMERA_ACTION_MOV_NV:
            evs = neko_UnmaskInput(m_bindings.mov_nv);
            break;
        case DENG_CAMERA_ACTION_MOV_W:
            evs = neko_UnmaskInput(m_bindings.mov_w);
            break;
        case DENG_CAMERA_ACTION_MOV_NW:
            evs = neko_UnmaskInput(m_bindings.mov_nw);
            break;

        case DENG_CAMERA_ACTION_MOV_X:
            evs = neko_UnmaskInput(m_bindings.mov_x);
            break;
        case DENG_CAMERA_ACTION_MOV_NX:
            evs = neko_UnmaskInput(m_bindings.mov_nx);
            break;
        case DENG_CAMERA_ACTION_MOV_Y:
            evs = neko_UnmaskInput(m_bindings.mov_y);
            break;
        case DENG_CAMERA_ACTION_MOV_NY:
            evs = neko_UnmaskInput(m_bindings.mov_ny);
            break;
        case DENG_CAMERA_ACTION_MOV_Z:
            evs = neko_UnmaskInput(m_bindings.mov_z);
            break;
        case DENG_CAMERA_ACTION_MOV_NZ:
            evs = neko_UnmaskInput(m_bindings.mov_nz);
            break;

        case DENG_CAMERA_ACTION_ROT_U:
            evs = neko_UnmaskInput(m_bindings.rot_u);
            break;
        case DENG_CAMERA_ACTION_ROT_NU:
            evs = neko_UnmaskInput(m_bindings.rot_nu);
            break;
        case DENG_CAMERA_ACTION_ROT_V:
            evs = neko_UnmaskInput(m_bindings.rot_v);
            break;
        case DENG_CAMERA_ACTION_ROT_NV:
            evs = neko_UnmaskInput(m_bindings.rot_nv);
            break;
        case DENG_CAMERA_ACTION_ROT_W:
            evs = neko_UnmaskInput(m_bindings.rot_w);
            break;
        case DENG_CAMERA_ACTION_ROT_NW:
            evs = neko_UnmaskInput(m_bindings.rot_nw);
            break;

        case DENG_CAMERA_ACTION_ROT_X:
            evs = neko_UnmaskInput(m_bindings.rot_x);
            break;
        case DENG_CAMERA_ACTION_ROT_NX:
            evs = neko_UnmaskInput(m_bindings.rot_nx);
            break;
        case DENG_CAMERA_ACTION_ROT_Y:
            evs = neko_UnmaskInput(m_bindings.rot_y);
            break;
        case DENG_CAMERA_ACTION_ROT_NY:
            evs = neko_UnmaskInput(m_bindings.rot_ny);
            break;
        case DENG_CAMERA_ACTION_ROT_Z:
            evs = neko_UnmaskInput(m_bindings.rot_z);
            break;
        case DENG_CAMERA_ACTION_ROT_NZ:
            evs = neko_UnmaskInput(m_bindings.rot_nz);
            break;

        case DENG_CAMERA_ACTION_CHANGE_MM:
            evs = neko_UnmaskInput(m_bindings.ch_vcp);
            break;
        default:
            break;
        }

        // For each input id in input bits, perform array input id check 
        deng_bool_t out = true;
        for(deng_i32_t i = 0; i < MAX_KEY_COMBO; i++) {
            // In case no binding was specified skip the iteration
            if(evs[i].key == NEKO_KEY_UNKNOWN) continue;

            // Check if the key status is active for certain event to occur 
            else if(evs[i].key <= NEKO_KEY_LAST && evs[i].key >= NEKO_KEY_FIRST) {
                if(!neko_FindKeyStatus(evs[i].key, NEKO_INPUT_EVENT_TYPE_ACTIVE)) {
                    out = false; 
                    break;
                }
            }

            else if(evs[i].btn <= NEKO_MOUSE_BTN_LAST && evs[i].btn >= NEKO_MOUSE_BTN_FIRST) {
                if(!neko_FindKeyStatus(evs[i].btn, NEKO_INPUT_EVENT_TYPE_ACTIVE)) {
                    out = false;
                    break;
                }
            }

            // Check if new mouse position exceeds the limit and set the event flag as false if needed
            else if(evs[i].md_mov <= NEKO_MOUSE_DELTA_LAST && evs[i].md_mov >= NEKO_MOUSE_DELTA_LAST) {
                dengMath::vec2<deng_px_t> delta = m_p_win->getMDelta();
                switch(evs[i].md_mov) {
                case NEKO_MOUSE_DELTA_X:
                    if(delta.first <= 0)
                        out = false;
                    break;

                case NEKO_MOUSE_DELTA_NX:
                    if(delta.first >= 0)
                        out = false;
                    break;

                case NEKO_MOUSE_DELTA_Y:
                    if(delta.second <= 0)
                        out = false;
                    break;

                case NEKO_MOUSE_DELTA_NY:
                    if(delta.second >= 0)
                        out = false;
                    break;

                default:
                    break;
                }

                if(!out) break;
            }
        }
        return out;
    }
}
