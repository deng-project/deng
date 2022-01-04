// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLConfiguration.h - OpenGL renderer configuration variables
// author: Karl-Mihkel Ott


#ifndef OPENGL_CONFIGURATION_H
#define OPENGL_CONFIGURATION_H

namespace DENG {
    namespace OpenGL {

        struct ConfigVariables {
            deng_bool_t enable_vsync = false;
            dengMath::vec4<deng_vec_t> background = {0.0f, 0.0f, 0.0f, 1.0f};
            deng::Camera3D *p_cam = NULL;
            deng::Window *p_win = NULL;
        };
    }
}


#endif
