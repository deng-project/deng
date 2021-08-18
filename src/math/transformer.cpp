/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: transformer.cpp - custom transformation class implementations
/// author: Karl-Mihkel Ott


#define __TRANSFORMER_CPP
#include <math/transformer.h>


namespace dengMath {

    /*******************************/
    /******** Transformer2D ********/
    /*******************************/

    Transformer2D::Transformer2D() {
        // Set all transformation matrices into identity matrices
        reset();       
    }


    /// Apply the matrix for each vertex in the asset
    void Transformer2D::__matApply(mat3<deng_vec_t> &mat, das_Asset &asset) {
        if(asset.is_opengl) __matApplyMerVert(mat, asset);
        else __matApplyMulVert(mat, asset);
    }


    void Transformer2D::__matApplyMulVert(mat3<deng_vec_t> &mat, das_Asset &asset) {
        vec3<deng_vec_t> tmp;
        switch(asset.asset_mode) {
        case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
            for(deng_ui64_t i = 0; i < asset.vertices.v2d.mul.pn; i++) {
                tmp = asset.vertices.v2d.mul.pos[i];
                tmp = mat * tmp;

                asset.vertices.v2d.mul.pos[i].vert_x = tmp.first;
                asset.vertices.v2d.mul.pos[i].vert_y = tmp.second;
            }
            break;

        case DAS_ASSET_MODE_2D_UNMAPPED:
            for(deng_ui64_t i = 0; i < asset.vertices.v2d.mul.pn; i++) {
                tmp = asset.vertices.v2d.mul.pos[i];
                tmp = mat * tmp;

                asset.vertices.v2d.mul.pos[i].vert_x = tmp.first;
                asset.vertices.v2d.mul.pos[i].vert_y = tmp.second;
            }
            break;

        default:
            RUN_ERR("dengMath::Transformer2D::__matApply()", "Asset must be a 2D asset!");
            break;
        }
    }


    void Transformer2D::__matApplyMerVert(mat3<deng_vec_t> &mat, das_Asset &asset) {
        vec3<deng_vec_t> tmp = {};

        switch(asset.asset_mode) {
        case DAS_ASSET_MODE_2D_UNMAPPED:
            for(deng_ui64_t i = 0; i < asset.vertices.v2d.mer.n; i++) {
                tmp = asset.vertices.v2d.mer.uvert[i];
                tmp = mat * tmp;

                asset.vertices.v2d.mer.uvert[i].vert_x = tmp.first;
                asset.vertices.v2d.mer.uvert[i].vert_y = tmp.second;
            }
            break;

        case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
            for(deng_ui64_t i = 0; i < asset.vertices.v2d.mer.n; i++) {
                tmp = asset.vertices.v2d.mer.vert[i].pos;
                tmp = mat * tmp;
                
                asset.vertices.v2d.mer.vert[i].pos.vert_x = tmp.first;
                asset.vertices.v2d.mer.vert[i].pos.vert_y = tmp.second;
            }
            break;

        default: break;
        }
    }




    /// Rotate the given asset
    /// NOTE: Rotation must be in radians
    void Transformer2D::rotate(deng_vec_t rot, das_Asset &asset) {
        mat3<deng_vec_t> rot_mat;

        rot_mat.row1 = vec3<deng_vec_t>{cosf(rot), -sinf(rot), 0.0f};
        rot_mat.row2 = vec3<deng_vec_t>{sinf(rot), cosf(rot), 0.0f};
        rot_mat.row3 = vec3<deng_vec_t>(0.0f, 0.0f, 1.0f);
        
        __matApply(rot_mat, asset);
    }

    
    /// Scale the given asset
    void Transformer2D::scale(const vec2<deng_vec_t> &scale, das_Asset &asset) {
        mat3<deng_vec_t> mat;
        
        mat.row1 = vec3<deng_vec_t>{scale.first, 0.0f, 0.0f};
        mat.row2 = vec3<deng_vec_t>{0.0f, scale.second, 0.0f};
        mat.row3 = vec3<deng_vec_t>(0.0f, 0.0f, 0.0f);

        __matApply(mat, asset);
    }


    /// Shear the given asset with the factor of opposite coordinate
    void Transformer2D::shear(const vec2<deng_vec_t> &shear, das_Asset &asset) {
        mat3<deng_vec_t> mat;

        mat.row1 = vec3<deng_vec_t>{1.0f, shear.first, 0.0f};
        mat.row2 = vec3<deng_vec_t>{shear.second, 1.0f, 0.0f};
        mat.row3 = vec3<deng_vec_t>(0.0f, 0.0f, 0.0f);

        __matApply(mat, asset);
    }


    /// Translate the given asset vertices from custom coordinate system into
    /// world coordinate system
    void Transformer2D::translate (
        const vec2<deng_vec_t> &u,
        const vec2<deng_vec_t> &v,
        const vec2<deng_vec_t> &origin,
        das_Asset &asset
    ) {
        mat3<deng_vec_t> mat;

        mat.row1 = vec3<deng_vec_t>{u.first, v.first, origin.first};
        mat.row2 = vec3<deng_vec_t>{u.second, v.second, origin.second};
        mat.row3 = vec3<deng_vec_t>(0.0f, 0.0f, 1.0f);

        __matApply(mat, asset);
    }


    /// Set rotation for the class transformation matrix
    void Transformer2D::setRotation(deng_vec_t rot) {
        m_rot_mat.row1 = vec3<deng_vec_t>{cosf(rot), -sinf(rot), 0.0f};
        m_rot_mat.row2 = vec3<deng_vec_t>{sinf(rot), cosf(rot), 0.0f};
        m_rot_mat.row3 = vec3<deng_vec_t>(0.0f, 0.0f, 0.0f);
    }


    /// Set scale for class transformation matrix
    void Transformer2D::setScale(const vec2<deng_vec_t> &scale) {
        m_scale_mat.row1 = vec3<deng_vec_t>{scale.first, 0.0f, 0.0f};
        m_scale_mat.row2 = vec3<deng_vec_t>{0.0f, scale.second, 0.0f};
        m_scale_mat.row3 = vec3<deng_vec_t>(0.0f, 0.0f, 1.0f);
    }


    /// Set shear for class transformation matrix
    void Transformer2D::setShear(const vec2<deng_vec_t> &shear) {
        m_shear_mat.row1 = vec3<deng_vec_t>{1.0f, shear.first, 0.0f};
        m_shear_mat.row2 = vec3<deng_vec_t>{shear.second, 1.0f, 0.0f};
        m_shear_mat.row3 = vec3<deng_vec_t>(0.0f, 0.0f, 1.0f);
    }


    /// Set coordinate translation for class transformation matrix
    void Transformer2D::setTranslation (
        const vec2<deng_vec_t> &u,
        const vec2<deng_vec_t> &v,
        const vec2<deng_vec_t> &w,
        const vec2<deng_vec_t> &origin
    ) {
        m_translation_mat.row1 = vec3<deng_vec_t>{u.first, v.first, origin.first};
        m_translation_mat.row2 = vec3<deng_vec_t>{u.second, v.second, origin.second};
        m_translation_mat.row3 = vec3<deng_vec_t>(0.0f, 0.0f, 1.0f);
    }


    /// Get the class transformation matrix 
    mat3<deng_vec_t> Transformer2D::getTransformMat() {
        // The order of transformations is following: rotation, scaling, shearing and translation
        return (m_translation_mat * m_shear_mat * m_scale_mat * m_rot_mat);
    }

    
    /// Reset all transformations to identity matrices
    void Transformer2D::reset() {
        m_rot_mat.row1 = vec3<deng_vec_t>(1.0f, 0.0f, 0.0f);
        m_rot_mat.row2 = vec3<deng_vec_t>(0.0f, 1.0f, 0.0f);
        m_rot_mat.row3 = vec3<deng_vec_t>(0.0f, 0.0f, 1.0f);

        m_translation_mat.row1 = vec3<deng_vec_t>(1.0f, 0.0f, 0.0f);
        m_translation_mat.row2 = vec3<deng_vec_t>(0.0f, 1.0f, 0.0f);
        m_translation_mat.row3 = vec3<deng_vec_t>(0.0f, 0.0f, 1.0f);

        m_scale_mat.row1 = vec3<deng_vec_t>(1.0f, 0.0f, 0.0f);
        m_scale_mat.row2 = vec3<deng_vec_t>(0.0f, 1.0f, 0.0f);
        m_scale_mat.row3 = vec3<deng_vec_t>(0.0f, 0.0f, 1.0f);

        m_shear_mat.row1 = vec3<deng_vec_t>(1.0f, 0.0f, 0.0f);
        m_shear_mat.row2 = vec3<deng_vec_t>(0.0f, 1.0f, 0.0f);
        m_shear_mat.row3 = vec3<deng_vec_t>(0.0f, 0.0f, 1.0f);
    }


    /*******************************/
    /******** Transformer3D ********/
    /*******************************/
    Transformer3D::Transformer3D() {
        // Set all transformation matrices into identity matrices
        reset();       
    }


    void Transformer3D::__matApply(mat4<deng_vec_t> &mat, das_Asset &asset) {
        if(asset.is_opengl) __matApplyMerVert(mat, asset);
        else __matApplyMulVert(mat, asset);
    }


    /// Apply the matrix for each vertex in the asset
    void Transformer3D::__matApplyMulVert(mat4<deng_vec_t> &mat, das_Asset &asset) {
        vec4<deng_vec_t> tmp;

        switch(asset.asset_mode) {
        case DAS_ASSET_MODE_3D_UNMAPPED:
            for(deng_ui64_t i = 0; i < asset.vertices.v3d.mul.pn; i++) {
                tmp = asset.vertices.v3d.mul.pos[i];
                tmp = mat * tmp;

                asset.vertices.v3d.mul.pos[i].vert_x = tmp.first;
                asset.vertices.v3d.mul.pos[i].vert_y = tmp.second;
                asset.vertices.v3d.mul.pos[i].vert_z = tmp.third;
            }
            break;

        case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
            for(deng_ui64_t i = 0; i < asset.vertices.v3d.mul.pn; i++) {
                tmp = asset.vertices.v3d.mul.pos[i];
                tmp.fourth = 1.0f;
                tmp = mat * tmp;

                asset.vertices.v3d.mul.pos[i].vert_x = tmp.first;
                asset.vertices.v3d.mul.pos[i].vert_y = tmp.second;
                asset.vertices.v3d.mul.pos[i].vert_z = tmp.third;
            }
            break;

        default:
            RUN_ERR("dengMath::Transformer3D::__matApplyMulVert()", "Asset type must be one of 3D types");
        }
    }


    void Transformer3D::__matApplyMerVert(mat4<deng_vec_t> &mat, das_Asset &asset) {
        vec4<deng_vec_t> tmp;

        switch(asset.asset_mode) {
        case DAS_ASSET_MODE_3D_UNMAPPED:
            for(deng_ui64_t i = 0; i < asset.vertices.v3d.mer.n; i++) {
                tmp = asset.vertices.v3d.mer.uvert[i].pos;
                tmp.fourth = 1.0f;
                tmp = mat * tmp;

                asset.vertices.v3d.mer.uvert[i].pos.vert_x = tmp.first;
                asset.vertices.v3d.mer.uvert[i].pos.vert_y = tmp.second;
                asset.vertices.v3d.mer.uvert[i].pos.vert_z = tmp.third;
            }
            break;

        case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
            for(deng_ui64_t i = 0; i < asset.vertices.v3d.mer.n; i++) {
                tmp = asset.vertices.v3d.mer.vert[i].pos;
                tmp.fourth = 1.0f;
                tmp = mat * tmp;

                asset.vertices.v3d.mer.uvert[i].pos.vert_x = tmp.first;
                asset.vertices.v3d.mer.uvert[i].pos.vert_y = tmp.second;
                asset.vertices.v3d.mer.uvert[i].pos.vert_z = tmp.third;
            }
            break;

        default:
            RUN_ERR("dengMath::Transformer3D::__matApplyMerVert()", "Asset type must be one of 3D types");
        }
    }


    /// Rotate the given asset.
    /// NOTE: Rotation must be in radians
    void Transformer3D::rotate(const vec3<deng_vec_t> &rot, das_Asset &asset) {
        mat4<deng_vec_t> x, y, z;
        LOG("Rot: " + std::to_string(rot.first) + ", " + std::to_string(rot.second) + ", " + std::to_string(rot.third));
        x.row1 = vec4<deng_vec_t>(1.0f, 0.0f, 0.0f, 0.0f);
        x.row2 = vec4<deng_vec_t>{0.0f, cosf(rot.first), -sinf(rot.first), 0.0f};
        x.row3 = vec4<deng_vec_t>{0.0f, sinf(rot.first), cosf(rot.first), 0.0f};
        x.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f);

        y.row1 = vec4<deng_vec_t>{cosf(rot.second), 0.0f, sinf(rot.second), 0.0f};
        y.row2 = vec4<deng_vec_t>(0.0f, 1.0f, 0.0f, 0.0f);
        y.row3 = vec4<deng_vec_t>{-sinf(rot.second), 0.0f, cosf(rot.second), 0.0f};
        y.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f);

        z.row1 = vec4<deng_vec_t>{cosf(rot.third), -sinf(rot.third), 0.0f, 0.0f}; 
        z.row2 = vec4<deng_vec_t>{sinf(rot.third), cosf(rot.third), 0.0f, 0.0f}; 
        z.row3 = vec4<deng_vec_t>(0.0f, 0.0f, 1.0f, 0.0f); 
        z.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f); 

        // Rotation transformations are performed in z, x, y order
        mat4<deng_vec_t> rot_mat = y * x * z;
        __matApply(rot_mat, asset);
    }


    /// Scale the given asset
    void Transformer3D::scale(const vec3<deng_vec_t> &scale, das_Asset &asset) {
        mat4<deng_vec_t> mat;
        mat.row1 = vec4<deng_vec_t>{scale.first, 0.0f, 0.0f, 0.0f};
        mat.row2 = vec4<deng_vec_t>{0.0f, scale.second, 0.0f, 0.0f};
        mat.row3 = vec4<deng_vec_t>{0.0f, 0.0f, scale.third, 0.0f};
        mat.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 0.0f);

        __matApply(mat, asset);
    }


    /// Translate the given asset vertices from custom coordinate system into world coordinate system
    void Transformer3D::translate (
        const vec3<deng_vec_t> &u,
        const vec3<deng_vec_t> &v,
        const vec3<deng_vec_t> &w,
        const vec3<deng_vec_t> &origin,
        das_Asset &asset
    ) {
        mat4<deng_vec_t> mat;
        mat.row1 = vec4<deng_vec_t>{u.first, v.first, w.first, origin.first};
        mat.row2 = vec4<deng_vec_t>{u.second, v.second, w.second, origin.second};
        mat.row3 = vec4<deng_vec_t>{u.third, v.third, w.third, origin.third};
        mat.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f);

        __matApply(mat, asset);
    }


    /// Set rotation for the class transformation matrix
    void Transformer3D::setRotation(const vec3<deng_vec_t> &rot) {
        m_rot_x_mat.row1 = vec4<deng_vec_t>(1.0f, 0.0f, 0.0, 0.0f);
        m_rot_x_mat.row2 = vec4<deng_vec_t>{0.0f, cosf(rot.first), -sinf(rot.first), 0.0f};
        m_rot_x_mat.row3 = vec4<deng_vec_t>{0.0f, sinf(rot.first), cosf(rot.first), 0.0f};
        m_rot_x_mat.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0, 1.0f);

        m_rot_y_mat.row1 = vec4<deng_vec_t>{cosf(rot.second), 0.0f, sinf(rot.second), 0.0f};
        m_rot_y_mat.row2 = vec4<deng_vec_t>(0.0f, 1.0f, 0.0f, 0.0f);
        m_rot_y_mat.row3 = vec4<deng_vec_t>{-sinf(rot.second), 1.0f, cosf(rot.second), 0.0f};
        m_rot_y_mat.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f);

        m_rot_z_mat.row1 = vec4<deng_vec_t>{cosf(rot.third), -sinf(rot.third), 0.0f, 0.0f};
        m_rot_z_mat.row2 = vec4<deng_vec_t>{sinf(rot.third), sinf(rot.third), 0.0f, 0.0f};
        m_rot_z_mat.row3 = vec4<deng_vec_t>(0.0f, 0.0f, 1.0f, 0.0f);
        m_rot_z_mat.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f);
    }


    /// Set scale for class transformation matrix
    void Transformer3D::setScale(const vec3<deng_vec_t> &scale) {
        m_scale_mat.row1 = vec4<deng_vec_t>{scale.first, 0.0f, 0.0f, 0.0f};
        m_scale_mat.row2 = vec4<deng_vec_t>{0.0f, scale.second, 0.0f, 0.0f};
        m_scale_mat.row3 = vec4<deng_vec_t>{0.0f, 0.0f, scale.third, 0.0f};
        m_scale_mat.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f);
    }


    /// Set coordinate translation for class transformation matrix
    void Transformer3D::setTranslation (
        const vec3<deng_vec_t> &u,
        const vec3<deng_vec_t> &v,
        const vec3<deng_vec_t> &w,
        const vec3<deng_vec_t> &origin
    ) {
        m_translation_mat.row1 = vec4<deng_vec_t>{u.first, v.first, w.first, origin.first};
        m_translation_mat.row2 = vec4<deng_vec_t>{u.second, v.second, w.second, origin.second};
        m_translation_mat.row3 = vec4<deng_vec_t>{u.third, v.third, w.third, origin.third};
        m_translation_mat.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f);
    }


    /// Get the class transformation matrix 
    mat4<deng_vec_t> Transformer3D::getTransformMat() {
        // Tranformation order: rotations, scaling and translation
        return (
            m_translation_mat *
            m_scale_mat *
            m_rot_z_mat *
            m_rot_y_mat *
            m_rot_x_mat
        );
    }


    /// Reset all transformations to identity matrices
    void Transformer3D::reset() {
        m_rot_x_mat.row1 = vec4<deng_vec_t>(1.0f, 0.0f, 0.0f, 0.0f);
        m_rot_x_mat.row2 = vec4<deng_vec_t>(0.0f, 1.0f, 0.0f, 0.0f);
        m_rot_x_mat.row3 = vec4<deng_vec_t>(0.0f, 0.0f, 1.0f, 0.0f);
        m_rot_x_mat.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f);

        m_rot_y_mat.row1 = vec4<deng_vec_t>(1.0f, 0.0f, 0.0f, 0.0f);
        m_rot_y_mat.row2 = vec4<deng_vec_t>(0.0f, 1.0f, 0.0f, 0.0f);
        m_rot_y_mat.row3 = vec4<deng_vec_t>(0.0f, 0.0f, 1.0f, 0.0f);
        m_rot_y_mat.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f);

        m_rot_z_mat.row1 = vec4<deng_vec_t>(1.0f, 0.0f, 0.0f, 0.0f);
        m_rot_z_mat.row2 = vec4<deng_vec_t>(0.0f, 1.0f, 0.0f, 0.0f);
        m_rot_z_mat.row3 = vec4<deng_vec_t>(0.0f, 0.0f, 1.0f, 0.0f);
        m_rot_z_mat.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f);

        m_translation_mat.row1 = vec4<deng_vec_t>(1.0f, 0.0f, 0.0f, 0.0f);
        m_translation_mat.row2 = vec4<deng_vec_t>(0.0f, 1.0f, 0.0f, 0.0f);
        m_translation_mat.row3 = vec4<deng_vec_t>(0.0f, 0.0f, 1.0f, 0.0f);
        m_translation_mat.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f);

        m_scale_mat.row1 = vec4<deng_vec_t>(1.0f, 0.0f, 0.0f, 0.0f);
        m_scale_mat.row2 = vec4<deng_vec_t>(0.0f, 1.0f, 0.0f, 0.0f);
        m_scale_mat.row3 = vec4<deng_vec_t>(0.0f, 0.0f, 1.0f, 0.0f);
        m_scale_mat.row4 = vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f);
    }
}
