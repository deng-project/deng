// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: EditorCamera.h - Editor camera class header
// author: Karl-Mihkel Ott


#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H


#ifdef EDITOR_CAMERA_CPP
    #include <string>
    #include <cstring>
    #include <vector>
    #include <variant>
    #include <chrono>
    #include <cmath>
#ifdef _DEBUG
    #include <iostream>
#endif

    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Quaternion.h>

    #include <Api.h>
    #include <BaseTypes.h>
    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <Window.h>
    #include <Renderer.h>
    #include <ModelUniforms.h>
    #include <CameraTransformManager.h>
    #include <Camera3D.h>
#endif

namespace DENG {

    class DENG_API EditorCamera : public Camera3D {
        private:
            bool m_is_enabled = false;
            const Libdas::Vector4<float> m_center_point = { 0.0f, 0.0f, 0.0f, 0.0f };

        public:
            EditorCamera(Window &_win, const Camera3DConfiguration &_conf, const std::string &_name, uint32_t _ubo_offset);

            virtual void EnableCamera() override;
            virtual void DisableCamera() override;
            virtual void Update(Renderer &_rend) override;
    };

    /// Main class for editor camera instance creation
    //class EditorCamera : private __EditorCameraEv, public __Camera3DBase {
    //private:
        //dengMath::vec3<deng_vec_t> m_origin;

    //public:
        //__EditorCamera(deng_vec_t zoom_step, const dengMath::vec3<deng_vec_t> &origin, const dengMath::vec2<deng_f64_t> &mouse_sens, 
            //deng_vec_t FOV, deng_vec_t near_plane, deng_vec_t far_plane, Window *p_ww );
        

        ///// Move origin point in world coordinate system by delta_mov
        //void moveOrigin(const dengMath::vec3<deng_vec_t> &delta_mov); 

        
        ///// Set camera control bindings for editor camera system
        //void setBindings(const Camera3DBindings &bindings);


        ///// Wrapper method for updating camera events
        //void update();
    //};
}

#endif
