/*    ____         ________     __      _        ______  
 *   |  _ \_      |  ______|   |  \    | |     _/  ____\
 *   | | \_ \_    |  |         |   \   | |    /  _/   
 *   | |   \  |   |  |_____    | |\ \  | |   |  |   _____ 
 *   | |    | |   |  ______|   | | \ \ | |   |  |  |___  | 
 *   | |  _/ _|   |  |         | |  \ \| |    \  \    / /
 *   | |_/ _/     |  |_____    | |   \   |     \_ \__/ /
 *   |____/       |________|   |_|    \__|       \____/
 *               
 * __________________________________________________________
 * __________________________________________________________
 *                       Project DENG 
 *
 *
 * Copyright (C) 2020 - 2021
 * This Software is licensed under Apache License as described 
 * in the LICENSE.md file, which you should have recieved with 
 * this distribution.
 * 
 * You may reproduce and distribute copies of the
 * Work or Derivative Works thereof in any medium, with or without
 * modifications, and in Source or Object form, provided that You
 * meet the following conditions:
 *
 * (a) You must give any other recipients of the Work or
 *     Derivative Works a copy of this License; and
 *
 * (b) You must cause any modified files to carry prominent notices
 *     stating that You changed the files; and
 *
 * (c) You must retain, in the Source form of any Derivative Works
 *     that You distribute, all copyright, patent, trademark, and
 *     attribution notices from the Source form of the Work,
 *     excluding those notices that do not pertain to any part of
 *     the Derivative Works; and
 *
 * (d) If the Work includes a "NOTICE" text file as part of its
 *     distribution, then any Derivative Works that You distribute must
 *     include a readable copy of the attribution notices contained
 *     within such NOTICE file, excluding those notices that do not
 *     pertain to any part of the Derivative Works, in at least one
 *     of the following places: within a NOTICE text file distributed
 *     as part of the Derivative Works; within the Source form or
 *     documentation, if provided along with the Derivative Works; or,
 *     within a display generated by the Derivative Works, if and
 *     wherever such third-party notices normally appear. The contents
 *     of the NOTICE file are for informational purposes only and
 *     do not modify the License. You may add Your own attribution
 *     notices within Derivative Works that You distribute, alongside
 *     or as an addendum to the NOTICE text from the Work, provided
 *     that such additional attribution notices cannot be construed
 *     as modifying the License.
 *
 * You may add Your own copyright statement to Your modifications and
 * may provide additional or different license terms and conditions
 * for use, reproduction, or distribution of Your modifications, or
 * for any such Derivative Works as a whole, provided Your use,
 * reproduction, and distribution of the Work otherwise complies with
 * the conditions stated in this License.
 */ 


#define __CAM_BASE_CPP
#include <deng/camera/cam_base.h>

// Shared data
extern deng::__SharedMouseData __ext_md;

namespace deng {
    
    /*********************************/
    /*********************************/
    /********* __CameraBase **********/
    /*********************************/
    /*********************************/

    __CameraBase::__CameraBase (
        deng_CameraType type,
        deng_vec_t fov,
        const dengMath::vec2<deng_vec_t> &planes,
        deng_vec_t aspect_ratio
    ) : m_cam_mat(type),
        m_proj_mat(fov, planes, aspect_ratio) { m_fov = fov; }


    /* 
     * Following methods are for moving the camera position in its coordinate system
     */
    void __CameraBase::moveU(deng_vec_t delta, deng_bool_t ignore_pitch) {
        m_cam_mat.moveCamera (
            (dengMath::vec3<deng_vec_t>) {delta, 0.0f, 0.0f},
            false,
            ignore_pitch,
            DENG_COORD_AXIS_X
        );
    }


    void __CameraBase::moveV(deng_vec_t delta, deng_bool_t ignore_pitch) {
        m_cam_mat.moveCamera (
            (dengMath::vec3<deng_vec_t>) {0.0f, delta, 0.0f},
            false,
            ignore_pitch,
            DENG_COORD_AXIS_Y 
        );
    }


    void __CameraBase::moveW(deng_vec_t delta, deng_bool_t ignore_pitch) {
        m_cam_mat.moveCamera (
            (dengMath::vec3<deng_vec_t>) {0.0f, 0.0f, delta},
            false,
            ignore_pitch,
            DENG_COORD_AXIS_Z 
        );
    }


    /* 
     * Following methods are for moving the camera's position in world coordinate system
     */
    void __CameraBase::moveX(deng_vec_t delta, deng_bool_t ignore_pitch) {
        m_cam_mat.moveCamera (
            (dengMath::vec3<deng_vec_t>) {delta, 0.0f, 0.0f},
            true,
            ignore_pitch,
            DENG_COORD_AXIS_X
        );
    }


    void __CameraBase::moveY(deng_vec_t delta, deng_bool_t ignore_pitch) {
        m_cam_mat.moveCamera (
            (dengMath::vec3<deng_vec_t>) {0.0f, delta, 0.0f},
            true,
            ignore_pitch,
            DENG_COORD_AXIS_Y 
        );
    }


    void __CameraBase::moveZ(deng_vec_t delta, deng_bool_t ignore_pitch) {
        m_cam_mat.moveCamera (
            (dengMath::vec3<deng_vec_t>) {0.0f, 0.0f, delta},
            true,
            ignore_pitch,
            DENG_COORD_AXIS_Z 
        );
    }

    
    /*
     * Following methods are for rotating the camera in its coordinate system
     */
    void __CameraBase::rotU(deng_vec_t rot) {
        m_cam_mat.setCameraRotation(rot, 0);
    }


    void __CameraBase::rotV(deng_vec_t rot) {
        m_cam_mat.setCameraRotation(0, rot);
    }


    /*
     * Following methods are for rotating the camera in origin specific coordinate system
     */
    void __CameraBase::rotX(deng_vec_t rot) {
        m_cam_mat.setOriginRotation (
            {m_origin.first, m_origin.second, m_origin.third},
            rot,
            0
        );
    }


    void __CameraBase::rotY(deng_vec_t rot) {
        m_cam_mat.setOriginRotation (
            {m_origin.first, m_origin.second, m_origin.third},
            0,
            rot
        );
    }


    dengMath::mat4<deng_vec_t> __CameraBase::getCamMat() { return m_cam_mat.getTransformMat(); }
    dengMath::mat4<deng_vec_t> __CameraBase::getProjMat() { return m_proj_mat.getProjectionMatrix(); }


    /**********************************/
    /*********** Event base ***********/
    /**********************************/

    __EventBase::__EventBase (
		const dengMath::vec2<deng_VCPOverflowAction> &vcp_act,
		const dengMath::vec2<dengMath::vec2<deng_f64_t>> &vc_bounds,
		const dengMath::vec2<deng_f64_t> &max_rot,
        Window *p_win
    ) {
        m_vcp_overflow = vcp_act;
        m_vc_bounds = vc_bounds;
        m_max_rot = max_rot;
        m_p_win = p_win;

        deng_LimitVirtualPos (
            vc_bounds.first.second, 
            vc_bounds.first.first, 
            vc_bounds.second.second,
            vc_bounds.second.first
        );

        deng_SetOverflowAction(
            vcp_act.first,
            vcp_act.second
        );
    }


    /* 
     * Camera mouse update method 
     * This method updates mouse cursor position in Window instance as well as
     * externally available MouseInputInfo
     */
    void __EventBase::__updateMouseEvData() {
        std::lock_guard<std::mutex> lck(__ext_md.mut);
        
        __ext_md.is_mouse_input = !m_p_win->isVCP();
        m_mouse_pos = m_p_win->getMPos(true);
        __ext_md.mouse_coords = m_mouse_pos;
    }


    /* 
     * Find the current mouse control rotation 
     */
    dengMath::vec2<deng_f64_t> __EventBase::__getMouseRotation() {
        dengMath::vec2<deng_f64_t> out_rot;
        out_rot.first = m_mouse_pos.second / m_vc_bounds.second.second * m_max_rot.first;
        out_rot.second = m_mouse_pos.first / m_vc_bounds.first.second * m_max_rot.second; 
        return out_rot;
    }

        
    /*
     * Check if input input conditions are satified for certain action
     */
    deng_bool_t __EventBase::__checkInputAction(deng_CameraAction action) {
        deng_InputEv *evs = NULL;
        switch(action)
        {
        case DENG_CAMERA_ACTION_MOV_U:
            evs = deng_UnmaskInput(m_bindings.mov_u);
            break;
        case DENG_CAMERA_ACTION_MOV_NU:
            evs = deng_UnmaskInput(m_bindings.mov_nu);
            break;
        case DENG_CAMERA_ACTION_MOV_V:
            evs = deng_UnmaskInput(m_bindings.mov_v);
            break;
        case DENG_CAMERA_ACTION_MOV_NV:
            evs = deng_UnmaskInput(m_bindings.mov_nv);
            break;
        case DENG_CAMERA_ACTION_MOV_W:
            evs = deng_UnmaskInput(m_bindings.mov_w);
            break;
        case DENG_CAMERA_ACTION_MOV_NW:
            evs = deng_UnmaskInput(m_bindings.mov_nw);
            break;

        case DENG_CAMERA_ACTION_MOV_X:
            evs = deng_UnmaskInput(m_bindings.mov_x);
            break;
        case DENG_CAMERA_ACTION_MOV_NX:
            evs = deng_UnmaskInput(m_bindings.mov_nx);
            break;
        case DENG_CAMERA_ACTION_MOV_Y:
            evs = deng_UnmaskInput(m_bindings.mov_y);
            break;
        case DENG_CAMERA_ACTION_MOV_NY:
            evs = deng_UnmaskInput(m_bindings.mov_ny);
            break;
        case DENG_CAMERA_ACTION_MOV_Z:
            evs = deng_UnmaskInput(m_bindings.mov_z);
            break;
        case DENG_CAMERA_ACTION_MOV_NZ:
            evs = deng_UnmaskInput(m_bindings.mov_nz);
            break;

        case DENG_CAMERA_ACTION_ROT_U:
            evs = deng_UnmaskInput(m_bindings.rot_u);
            break;
        case DENG_CAMERA_ACTION_ROT_NU:
            evs = deng_UnmaskInput(m_bindings.rot_nu);
            break;
        case DENG_CAMERA_ACTION_ROT_V:
            evs = deng_UnmaskInput(m_bindings.rot_v);
            break;
        case DENG_CAMERA_ACTION_ROT_NV:
            evs = deng_UnmaskInput(m_bindings.rot_nv);
            break;
        case DENG_CAMERA_ACTION_ROT_W:
            evs = deng_UnmaskInput(m_bindings.rot_w);
            break;
        case DENG_CAMERA_ACTION_ROT_NW:
            evs = deng_UnmaskInput(m_bindings.rot_nw);
            break;

        case DENG_CAMERA_ACTION_ROT_X:
            evs = deng_UnmaskInput(m_bindings.rot_x);
            break;
        case DENG_CAMERA_ACTION_ROT_NX:
            evs = deng_UnmaskInput(m_bindings.rot_nx);
            break;
        case DENG_CAMERA_ACTION_ROT_Y:
            evs = deng_UnmaskInput(m_bindings.rot_y);
            break;
        case DENG_CAMERA_ACTION_ROT_NY:
            evs = deng_UnmaskInput(m_bindings.rot_ny);
            break;
        case DENG_CAMERA_ACTION_ROT_Z:
            evs = deng_UnmaskInput(m_bindings.rot_z);
            break;
        case DENG_CAMERA_ACTION_ROT_NZ:
            evs = deng_UnmaskInput(m_bindings.rot_nz);
            break;

        case DENG_CAMERA_ACTION_CHANGE_MM:
            evs = deng_UnmaskInput(m_bindings.ch_vcp);
            break;

        default:
            break;
        }

        // Check if input button or key is active
        deng_bool_t out = true;
        for(int i = 0; i < MAX_KEY_COMBO; i++) {
            if(evs[i].key == DENG_KEY_UNKNOWN) continue;
            else if(evs[i].key <= DENG_KEY_LAST && evs[i].key >= DENG_KEY_FIRST) {
                if (
                    !__deng_FindKeyStatus (
                        evs[i].key, 
                        DENG_MOUSE_BTN_UNKNOWN, 
                        DENG_INPUT_TYPE_KB,
                        DENG_INPUT_EVENT_TYPE_ACTIVE
                    )
                ) {
                    out = false; 
                    break;
                }
            }

            else if(evs[i].btn <= DENG_MOUSE_BTN_LAST && evs[i].btn >= DENG_MOUSE_BTN_FIRST) {
                if ( 
                    !__deng_FindKeyStatus (
                        DENG_KEY_UNKNOWN,
                        evs[i].btn,
                        DENG_INPUT_TYPE_MOUSE,
                        DENG_INPUT_EVENT_TYPE_ACTIVE
                    )
                ) {
                    out = false;
                    break;
                }
            }

            else if(evs[i].md_mov <= DENG_MOUSE_DELTA_LAST && evs[i].md_mov >= DENG_MOUSE_DELTA_LAST) {
                dengMath::vec2<deng_px_t> delta = m_p_win->getMDelta();
                switch(evs[i].md_mov)
                {
                case DENG_MOUSE_DELTA_X:
                    if(delta.first <= 0)
                        out = false;
                    break;

                case DENG_MOUSE_DELTA_NX:
                    if(delta.first >= 0)
                        out = false;
                    break;

                case DENG_MOUSE_DELTA_Y:
                    if(delta.second <= 0)
                        out = false;
                    break;

                case DENG_MOUSE_DELTA_NY:
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
