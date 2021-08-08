/// DENG: dynamic engine - powerful 2D and 3D game engine
/// licence: Apache, see LICENCE file
/// file: cam_base.h - common 3D camera functionality class header
/// author: Karl-Mihkel Ott

#ifndef __CAM_BASE_H
#define __CAM_BASE_H


#ifdef __CAM_BASE_CPP
    #include <stdlib.h>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/vec2.h>
    #include <math/vec3.h>
    #include <math/vec4.h>
    #include <math/mat3.h>
    #include <math/mat4.h>
    #include <math/camera_mat.h>
    #include <math/projection_mat.h>
    #include <key_translation.h>
    #include <deng/window.h>
#endif

namespace deng {
    
    class __Camera3DBase {
    protected:
        deng_vec_t m_fov; // Radians
        dengMath::vec4<deng_vec_t> m_origin = dengMath::vec4<deng_vec_t>{0.0f, 0.0f, 0.0f, 0.0f}; // Origin doesn't accept transformations
        dengMath::CameraMatrix m_cam_mat;
        dengMath::ProjectionMatrix m_proj_mat;

    public:
        __Camera3DBase(deng_CameraType type, deng_vec_t fov,
            const dengMath::vec2<deng_vec_t> &planes, deng_vec_t aspect_ratio);


        /// Following methods are for moving the camera position in its coordinate system
        void moveU(deng_vec_t delta, deng_bool_t ignore_pitch);
        void moveV(deng_vec_t delta, deng_bool_t ignore_pitch);
        void moveW(deng_vec_t delta, deng_bool_t ignore_pitch);


        /// Following methods are for moving the camera position in world coordinate system
        void moveX(deng_vec_t delta, deng_bool_t ignore_pitch);
        void moveY(deng_vec_t delta, deng_bool_t ignore_pitch);
        void moveZ(deng_vec_t delta, deng_bool_t ignore_pitch);

        
        /// Following methods are for rotating the camera in its coordinate system
        void rotU(deng_vec_t rot);
        void rotV(deng_vec_t rot);


        /// Following methods are for rotating the camera in origin specific coordinate system
        void rotX(deng_vec_t rot);
        void rotY(deng_vec_t rot);


    // Getters
    public:
        dengMath::mat4<deng_vec_t> getCamMat(); 
        dengMath::mat4<deng_vec_t> getProjMat();
    };
}

#endif
