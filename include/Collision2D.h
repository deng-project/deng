// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Collision2D.h - 2D collision detection class header
// author: Karl-Mihkel Ott


#ifndef COLLISION_2D_H
#define COLLISION_2D_H

#ifdef COLLISION_2D_CPP
    #include <vector>
    #include <string>
    #include <stdlib.h>

    #include <base_types.h>
    #include <err_def.h>
    #include <assets.h>
    #include <deng_math.h>
#endif

namespace DENG {
    class Collision2D {
    public:
        bool FindPtCollision (
            const dengMath::vec2<deng_vec_t> &point,
            const das_Asset *p_asset,
            const dengMath::vec2<size_t> &vert_bounds
        ); 

        bool FindPtCollision ( 
            const dengMath::vec2<deng_vec_t> &point,
            const das_ObjPosData2D *verts,
            const deng_ui32_t vert_c
        );
    };
}

#endif
