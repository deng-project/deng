/// DENG: dynamic engine - small but powerful 2D and 3D game engine
/// licence: Apache, see LICENCE file
/// file: camera_mat.h - camera matrix class header
/// author: Karl-Mihkel Ott


#ifndef __CAMERA_MAT_H
#define __CAMERA_MAT_H


#ifdef __CAMERA_MAT_CPP
    #include <type_traits>
    #include <stdlib.h>
    #include <string.h>
    #include <cmath>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <data/assets.h>
    #include <math/vec2.h>
    #include <math/vec3.h>
    #include <math/vec4.h>
    #include <math/mat3.h>
    #include <math/mat4.h>
    
    #define __DENG_CAMERA_RIGHT     vec4<deng_vec_t>(1.0f, 0.0f, 0.0f, 0.0f);
    #define __DENG_CAMERA_UP        vec4<deng_vec_t>(0.0f, 1.0f, 0.0f, 0.0f);
    #define __DENG_CAMERA_FORWARD   vec4<deng_vec_t>{0.0f, 0.0f, -1.0f, 0.0f};
#endif


namespace dengMath {

    class CameraMatrix {
    private:
        vec4<deng_vec_t> m_camera_pos;
        mat4<deng_vec_t> m_camera_mat;

        deng_vec_t m_x_rot;
        deng_vec_t m_y_rot;

        vec4<deng_vec_t> m_rs = vec4<deng_vec_t>(1.0f, 0.0f, 0.0f, 0.0f);
        vec4<deng_vec_t> m_ts = vec4<deng_vec_t>(0.0f, 1.0f, 0.0f, 0.0f);
        vec4<deng_vec_t> m_fs = vec4<deng_vec_t>{0.0f, 0.0f, -1.0f, 0.0f};

        // Camera coordinate specific rotation matrices
        mat4<deng_vec_t> m_rot_x_mat;
        mat4<deng_vec_t> m_rot_y_mat;

    public:
        CameraMatrix(deng_CameraType type);

        
        /* 
         * Force set camera position to a new one
         */
        void setCameraPosition(const vec3<deng_vec_t> &camera_pos);


        /*
         * Move camera by one movement step
         */
        void moveCamera (
            const vec3<deng_vec_t> &mov_speed, 
            deng_bool_t is_world,
            deng_bool_t ignore_pitch,
            const deng_CoordinateAxisType &movement_type
        );


        /*
         * Set new rotation for the camera relative to the its coordinate system
         */
        void setCameraRotation (
            deng_vec_t x_rot, 
            deng_vec_t y_rot
        );


        /*
         * Set new rotation for the camera relative to its origin point in world coordinates
         */
        void setOriginRotation (
            dengMath::vec3<deng_vec_t> point,
            deng_vec_t x_rot,
            deng_vec_t y_rot
        );


        /*
         * Create transformation matrix for camera system based
         * on previously submitted values
         */
        void camTransform(deng_bool_t is_world_origin);

    // Getter methods
    public: 
        
        /*
         * Get the transformation matrix
         */
        mat4<deng_vec_t> getTransformMat();


        /*
         * Get the current position of the camera
         */
        vec4<deng_vec_t> getPosition();

        
        /*
         * Get current camera sides' coordinates
         */
        void getSides (
            vec4<deng_vec_t> *p_u,
            vec4<deng_vec_t> *p_v,
            vec4<deng_vec_t> *p_w
        );
    };
}

#endif
