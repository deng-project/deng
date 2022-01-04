/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: Transformer.cpp - custom transformation class implementations
/// author: Karl-Mihkel Ott


#ifndef TRANSFORMER_H
#define TRANSFORMER_H


#ifdef TRANSFORMER_CPP
    #include <type_traits>
    #include <stdlib.h>
    #include <string.h>
	#include <string>
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


namespace DENG {

    /*******************************/
    /******** Transformer2D ********/
    /*******************************/

    /// Perform transformation calculations on 2D assets using CPU
    class Transformer2D {
    private: 
        mat3<deng_vec_t> m_rot_mat;

        mat3<deng_vec_t> m_translation_mat;
        mat3<deng_vec_t> m_scale_mat;
        mat3<deng_vec_t> m_shear_mat;

    private:
        /// Apply the matrix for each vertex in the asset
        void __matApply(mat3<deng_vec_t> &mat, das_Asset &asset);
        void __matApplyMulVert(mat3<deng_vec_t> &mat, das_Asset &asset);
        void __matApplyMerVert(mat3<deng_vec_t> &mat, das_Asset &asset);

    public:
        Transformer2D();

        /// Rotate the given asset
        /// NOTE: Rotation must be in radians
        void rotate(deng_vec_t rot, das_Asset &asset);


        /// Scale the given asset
        void scale(const vec2<deng_vec_t> &scale, das_Asset &asset);


        /// Shear the given asset with the factor of opposite coordinate
        void shear(const vec2<deng_vec_t> &shear, das_Asset &asset);


        /// Translate the given asset vertices from custom coordinate system into world coordinate system
        void translate (
            const vec2<deng_vec_t> &u,
            const vec2<deng_vec_t> &v,
            const vec2<deng_vec_t> &origin,
            das_Asset &asset
        );

        
        /// Set rotation for the class transformation matrix
        void setRotation(deng_vec_t rot);


        /// Set scale for class transformation matrix
        void setScale(const vec2<deng_vec_t> &scale);

        
        /// Set shear for class transformation matrix
        void setShear(const vec2<deng_vec_t> &shear);


        /// Set coordinate translation for class transformation matrix
        void setTranslation (
            const vec2<deng_vec_t> &u,
            const vec2<deng_vec_t> &v,
            const vec2<deng_vec_t> &w,
            const vec2<deng_vec_t> &origin
        );


        /// Get the class transformation matrix 
        mat3<deng_vec_t> getTransformMat();


        /// Reset all transformations to identity matrices
        void reset();
    };


    /*******************************/
    /******** Transformer3D ********/
    /*******************************/

    /// Perform transformation calculations on 3D assets using CPU
    class Transformer3D {
    private:
        mat4<deng_vec_t> m_rot_x_mat;
        mat4<deng_vec_t> m_rot_y_mat;
        mat4<deng_vec_t> m_rot_z_mat;

        mat4<deng_vec_t> m_translation_mat;
        mat4<deng_vec_t> m_scale_mat;

    private:
        /// Apply the matrix for each vertex in the asset
        void __matApply(mat4<deng_vec_t> &mat, das_Asset &asset);
        void __matApplyMulVert(mat4<deng_vec_t> &mat, das_Asset &asset);
        void __matApplyMerVert(mat4<deng_vec_t> &mat, das_Asset &asset);

    public:
        Transformer3D();


        /// Rotate the given asset.
        /// NOTE: Rotation must be in radians
        void rotate(const vec3<deng_vec_t> &rot, das_Asset &asset);


        /// Scale the given asset
        void scale(const vec3<deng_vec_t> &scale, das_Asset &asset);


        /// Translate the given asset vertices from custom coordinate system into
        /// world coordinate system
        void translate (
            const vec3<deng_vec_t> &u,
            const vec3<deng_vec_t> &v,
            const vec3<deng_vec_t> &w,
            const vec3<deng_vec_t> &origin,
            das_Asset &asset
        );

        
        /// Set rotation for the class transformation matrix
        void setRotation(const vec3<deng_vec_t> &rot);


        /// Set scale for class transformation matrix
        void setScale(const vec3<deng_vec_t> &scale);


        /// Set coordinate translation for class transformation matrix
        void setTranslation (
            const vec3<deng_vec_t> &u,
            const vec3<deng_vec_t> &v,
            const vec3<deng_vec_t> &w,
            const vec3<deng_vec_t> &origin
        );


        /// Get the class transformation matrix 
        mat4<deng_vec_t> getTransformMat();


        /// Reset all transformations to identity matrices
        void reset();
    };
}

#endif
