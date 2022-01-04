/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: resources.h - OpenGL resource structures header
/// author: Karl-Mihkel Ott


#ifndef __GL_RESOURCES_H
#define __GL_RESOURCES_H

namespace deng {
    namespace opengl {

        struct __gl_Resources {
            deng_ugl_t vert_array = 0;
            deng_ugl_t vert_buffer = 0;
            deng_ugl_t idx_buffer = 0;
            deng_ugl_t ubo_buffer = 0;
        };


        /// OpenGL specific texture data structure
        struct __gl_Texture {
            deng_Id base_id;
            deng_Id uuid;
            deng_ugl_t gl_id;
            deng_ugl_t unit_nr;
        };
    }
}

#endif
