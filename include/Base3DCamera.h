// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Base3DCamera.h - Common 3D camera functionality class header
// author: Karl-Mihkel Ott

#ifndef BASE_3D_CAMERA_H
#define BASE_3D_CAMERA_H


#ifdef BASE_3D_CAMERA_CPP
    #include <stdlib.h>
    #include <string.h>
    #include <vulkan/vulkan.h>

    #include <base_types.h>
    #include <err_def.h>
    #include <assets.h>

    #include <vec2.h>
    #include <vec3.h>
    #include <vec4.h>
    #include <mat3.h>
    #include <mat4.h>
    #include <camera_mat.h>
    #include <projection_mat.h>
    #include <deps/nekowin/include/key_translation.h>
    #include <window.h>
#endif

namespace deng {
    
    class Base3DCamera {
    protected:
        deng_vec_t m_fov; // Radians
        dengMath::vec4<deng_vec_t> m_origin = dengMath::vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 0.0f); // Origin doesn't accept transformations
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
