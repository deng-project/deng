/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: collision.cpp - 2D collision detection class implementation
/// author: Karl-Mihkel Ott


#define __COLLISION_CPP
#include <utils/collision.h>

namespace dengUtils {
    /*********************************************/
    /******* 2D COLLISION DETECTION METHODS ******/
    /*********************************************/

    /// Find point collision with asset
    deng_bool_t Collision2D::findPtCollision (
        const dengMath::vec2<deng_vec_t> &point,
        const das_Asset *p_asset,
        const dengMath::vec2<size_t> &vert_bounds
    ) {
        size_t i, j;
        deng_bool_t is_colliding = false;
        dengMath::vec2<deng_vec_t> *vc;
        dengMath::vec2<deng_vec_t> *vn;

        // Check every triangle for potential collision
        for(i = vert_bounds.first; i < vert_bounds.second; i++) {
            j = i + 1;
            if(j == vert_bounds.second) 
                j = vert_bounds.first;
            
            vc = (dengMath::vec2<deng_vec_t>*) &p_asset->vertices.v2d.mul.pos[i];
            vn = (dengMath::vec2<deng_vec_t>*) &p_asset->vertices.v2d.mul.pos[j];

            if
            (
                ((vc->second >= point.second && vn->second < point.second) ||
                (vc->second < point.second && vn->second >= point.second)) &&
                (point.first < (vn->first - vc->first) * (point.second - vc->second) / (vn->second - vc->second) + vc->first)
            ) is_colliding = !is_colliding;
        }

        return is_colliding;
    }


    /// Find point collision with specified vertices
    deng_bool_t Collision2D::findPtCollision (
        const dengMath::vec2<deng_vec_t> &point,
        const das_ObjPosData2D *verts,
        const deng_ui32_t vert_c
    ) {
        size_t i, j;
        deng_bool_t is_colliding = false;
        dengMath::vec2<deng_vec_t> *vn, *vc;

        for(i = 0; i < vert_c; i++) {
            j = i + 1;
            if(j >= vert_c)
                j = 0;

            vc = (dengMath::vec2<deng_vec_t>*) &verts[i];
            vn = (dengMath::vec2<deng_vec_t>*) &verts[j];

            if
            (
                ((vc->second >= point.second && vn->second < point.second) ||
                (vc->second < point.second && vn->second >= point.second)) &&
                (point.first < (vn->first - vc->first) * (point.second - vc->second) / (vn->second - vc->second) + vc->first)
            ) is_colliding = !is_colliding;
        }

        return is_colliding;
    }
}
