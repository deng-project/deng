/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: cfg_vars.h - OpenGL renderer configuration variables
/// author: Karl-Mihkel Ott


#ifndef __GL_CFG_VARS_H
#define __GL_CFG_VARS_H

namespace deng {
    namespace opengl {

        struct __gl_ConfigVars {
            deng_bool_t enable_vsync = false;
            dengMath::vec4<deng_vec_t> background = {0.0f, 0.0f, 0.0f, 1.0f};
            deng::Camera3D *p_cam = NULL;
            deng::Window *p_win = NULL;
        };
    }
}


#endif
