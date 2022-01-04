/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: projection_mat.cpp - projection matrix calculation class implementation
/// author: Karl-Mihkel Ott


#define __PROJECTION_MAT_CPP
#include <math/projection_mat.h>


namespace dengMath {

    ProjectionMatrix::ProjectionMatrix (
        deng_vec_t fov,
        const vec2<deng_vec_t> &planes,
        deng_vec_t aspect_ratio
    ) {
        m_fov = fov;
        m_near = planes.first;
        m_far = planes.second;
        m_aspect_ratio = aspect_ratio;

        __calcMatrix();
    }


    /// Calculate the value of the perspective projection matrix from given private class members
    void ProjectionMatrix::__calcMatrix() {
        m_mat.row1 = vec4<deng_vec_t>{m_aspect_ratio / tanf(m_fov / 2), 0.0f, 0.0f, 0.0f};
        m_mat.row2 = vec4<deng_vec_t>{0.0f, 1 / tanf(m_fov / 2), 0.0f, 0.0f};
        m_mat.row3 = vec4<deng_vec_t>{0.0f, 0.0f, m_far / (m_far + m_near), 1.0};
        m_mat.row4 = vec4<deng_vec_t>{0.0f, 0.0f, (m_far * m_near) / (m_far + m_near), 0.0f};
    }


    /// Change field of view of the matrix
    void ProjectionMatrix::setFov(deng_vec_t fov) {
        m_fov = fov;
        __calcMatrix();
    }


    /// Change surfaces aspect ratio when surface was resized
    void ProjectionMatrix::setAspectRatio(deng_vec_t aspect_ratio) {
        m_aspect_ratio = aspect_ratio;
        __calcMatrix();
    }


    /// Change view near and far view planes
    void ProjectionMatrix::setViewPlanes(const vec2<deng_vec_t> &planes) {
        m_near = planes.first;
        m_far = planes.second;
        __calcMatrix();
    }

    /// Get the calculated projection matrix
    const mat4<deng_vec_t> &ProjectionMatrix::getProjectionMatrix() {
        return m_mat;
    }
}
