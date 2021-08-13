/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: projection_mat.h - projection matrix calculation class header
/// author: Karl-Mihkel Ott


#ifndef __PROJECTION_MAT_H
#define __PROJECTION_MAT_H


#ifdef __PROJECTION_MAT_CPP
    #include <type_traits>
    #include <stdlib.h>
    #include <cmath>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <data/assets.h>
    #include <math/vec2.h>
    #include <math/vec3.h>
    #include <math/vec4.h>
    #include <math/mat3.h>
    #include <math/mat4.h>
#endif

namespace dengMath {

    /// Class for creating new and modifying existing projection matrices
    class ProjectionMatrix {
    private:
        mat4<deng_vec_t> m_mat;
        deng_vec_t m_fov;
        deng_vec_t m_aspect_ratio;
        deng_vec_t m_near;
        deng_vec_t m_far;

    private:
        
        /// Calculate the value of the perspective projection matrix from given private class members
        void __calcMatrix();

    public:
        ProjectionMatrix(deng_vec_t fov, const vec2<deng_vec_t> &planes, deng_vec_t aspect_ratio);


        /// Change field of view of the matrix
        void setFov(deng_vec_t fov);


        /// Change surfaces aspect ratio when surface was resized
        void setAspectRatio(deng_vec_t aspect_ratio);


        /// Change view near and far view planes
        void setViewPlanes(const vec2<deng_vec_t> &planes);

    public:

        /// Get the calculated projection matrix
        const mat4<deng_vec_t> &getProjectionMatrix();
    };
}

#endif
