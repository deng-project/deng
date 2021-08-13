/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: collision.cpp - 2D collision detection class header
/// author: Karl-Mihkel Ott


#ifndef __COLLISION_H
#define __COLLISION_H

#ifdef __COLLISION_CPP
    #include <vector>
    #include <string>
    #include <stdlib.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <data/assets.h>
    #include <math/deng_math.h>
#endif

namespace dengUtils {
    class Collision2D {
    public:
        deng_bool_t findPtCollision (
            const dengMath::vec2<deng_vec_t> &point,
            const das_Asset *p_asset,
            const dengMath::vec2<size_t> &vert_bounds
        ); 

        deng_bool_t findPtCollision ( 
            const dengMath::vec2<deng_vec_t> &point,
            const das_ObjPosData2D *verts,
            const deng_ui32_t vert_c
        );
    };
}

#endif
