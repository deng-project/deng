/// NEKO: dynamic engine - small but powerful 2D and 3D game engine
/// licence: Apache, see LICENCE file
/// file: cam_bindings.h - header file for 3D camera bindings structure
/// author: Karl-Mihkel Ott

#ifndef __CAM_BINDINGS_H
#define __CAM_BINDINGS_H

namespace deng {

    /// NOTE: This structure uses input bits instead of regular input event keys
    struct Camera3DBindings {
        // Movements according to camera's coordinate system
        neko_InputBits mov_u = NEKO_KEY_UNKNOWN;
        neko_InputBits mov_nu = NEKO_KEY_UNKNOWN;
        neko_InputBits mov_v = NEKO_KEY_UNKNOWN;
        neko_InputBits mov_nv = NEKO_KEY_UNKNOWN;
        neko_InputBits mov_w = NEKO_KEY_UNKNOWN;
        neko_InputBits mov_nw = NEKO_KEY_UNKNOWN;
        
        // Movements according to world's coordinate system
        neko_InputBits mov_x = NEKO_KEY_UNKNOWN;
        neko_InputBits mov_nx = NEKO_KEY_UNKNOWN;
        neko_InputBits mov_y = NEKO_KEY_UNKNOWN;
        neko_InputBits mov_ny = NEKO_KEY_UNKNOWN;
        neko_InputBits mov_z = NEKO_KEY_UNKNOWN;
        neko_InputBits mov_nz = NEKO_KEY_UNKNOWN;

        // Origin movements according to the camera's coordinate system
        neko_InputBits mov_ori_u = NEKO_KEY_UNKNOWN;
        neko_InputBits mov_ori_w = NEKO_KEY_UNKNOWN;

        // Origin movements according to the world coordinate system
        neko_InputBits mov_ori_x = NEKO_KEY_UNKNOWN;
        neko_InputBits mov_ori_z = NEKO_KEY_UNKNOWN;

        // Rotations according to camera coordinate system (pitch, yaw roll)
        neko_InputBits rot_u = NEKO_KEY_UNKNOWN;
        neko_InputBits rot_nu = NEKO_KEY_UNKNOWN;
        neko_InputBits rot_v = NEKO_KEY_UNKNOWN;
        neko_InputBits rot_nv = NEKO_KEY_UNKNOWN;
        neko_InputBits rot_w = NEKO_KEY_UNKNOWN;
        neko_InputBits rot_nw = NEKO_KEY_UNKNOWN;

        // Rotations according to origin point
        neko_InputBits rot_x = NEKO_KEY_UNKNOWN;
        neko_InputBits rot_nx = NEKO_KEY_UNKNOWN;
        neko_InputBits rot_y = NEKO_KEY_UNKNOWN;
        neko_InputBits rot_ny = NEKO_KEY_UNKNOWN;
        neko_InputBits rot_z = NEKO_KEY_UNKNOWN;
        neko_InputBits rot_nz = NEKO_KEY_UNKNOWN;

        // Mask for mouse input mode changing bindings
        neko_InputBits ch_vcp = NEKO_KEY_UNKNOWN;
    };
}

#endif
