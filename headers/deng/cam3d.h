/// DENG: dynamic engine - powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: cam3d.h - class header for handling multiple 3D camera modes
/// author: Karl-Mihkel Ott

#ifndef __CAMERA_H
#define __CAMERA_H

#ifdef __CAMERA_CPP
    #define __DENG_DEFAULT_ZOOM_STEP 0.1f
    
    #include <memory>
    #include <mutex>
    #include <cstring>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/window.h>
#endif


#include <deng/camera/3d/cam_bindings.h>
#include <deng/camera/3d/cam_base.h>
#include <deng/camera/3d/ev_base.h>
#include <deng/camera/3d/ed_cam_ev.h>
#include <deng/camera/3d/ed_cam.h>
#include <deng/camera/3d/fpp_cam_ev.h>
#include <deng/camera/3d/fpp_cam.h>
#include <deng/camera/3d/tpp_cam.h>


namespace deng {

    /// DENG engine camera handling class
    /// This class is universal and it can be used to create first person, editor and third person
    /// camera systems
    class Camera3D {
    private:
        std::unique_ptr<__FPPCamera> fpp_cam = {};
        std::unique_ptr<__EditorCamera> ed_cam = {};
        Window *m_p_win;
        deng_CameraType m_cam_type;

    public:
        Camera3D (
            deng_CameraType cam_type,
            deng_vec_t fov,
            const dengMath::vec2<deng_vec_t> &planes,
            const dengMath::vec3<deng_vec_t> &mov_speed,
            const dengMath::vec2<deng_f64_t> &rot_sens,
            deng_bool_t ignore_pitch_rot,
            Window *win
        );


        /// Set the camera key bindings
        /// NOTE: These bindings are the ones that control camera movement and rotation actions
        void setBindings(const Camera3DBindings &bindings);


        /// Move camera's origin in world space by delta_mov
        /// NOTE: This method only effects editor camera and third person perspective camera
        /// systems. If camera type is first person perspective then runtime exception is thrown
        void moveOrigin(const dengMath::vec3<deng_vec_t> &delta_mov);


        /// Move camera's position by delta_mov in camera's coordinate system
        void moveCameraPOVC(const dengMath::vec3<deng_vec_t> &delta_mov, deng_bool_t ignore_pitch);


        /// Move camera's position in world coordinate system
        void moveCameraPOVW(const dengMath::vec3<deng_vec_t> &delta_mov, deng_bool_t ignore_pitch);


        /// Rotate camera's position by rot in camera's coordinate system
        void rotCameraPOVC(const dengMath::vec2<deng_vec_t> &rot);


        /// Rotate camera's position by rot in world's coordinate system
        void rotCameraPOVW(const dengMath::vec2<deng_vec_t> &rot);


        /// Main camera system update method
        void update();

        
        /// Get the camera type of the current camera instance
        deng_CameraType getType();


        /// Get the dot product of camera's view and projection matrix
        dengMath::mat4<deng_vec_t> getCameraMat();


        /// Get the bare camera view matrix
        dengMath::mat4<deng_vec_t> getViewMat();

        
        /// Returns true if camera has perspective view
        deng_bool_t isPerspective();
    };
}

#endif
