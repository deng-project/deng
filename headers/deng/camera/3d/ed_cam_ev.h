/// NEKO: dynamic engine - small but powerful 2D and 3D game engine
/// licence: Apache, see LICENCE file
/// file: ed_cam_ev.h - editor camera event class header
/// author: Karl-Mihkel Ott

#ifndef __ED_CAM_EV_H
#define __ED_CAM_EV_H


#ifdef __ED_CAM_EV_CPP
    #include <string>
    #include <vulkan/vulkan.h>
    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <data/assets.h>
    
    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/camera/3d/cam_bindings.h>
    #include <deng/camera/3d/ev_base.h>
#endif


namespace deng {
    
    /// Event handler class for editor camera
    class __EditorCameraEv : protected __Event3DBase {
    private:
        deng_EditorCameraEvent m_editor_cam_ev;
        dengMath::vec2<deng_f64_t> m_last_rot = {0, 0};
        deng_vec_t m_zoom_step;
        deng_bool_t m_is_rot_cur = false;

    protected:
        /// Set the default camera rotation and position values
        void __initCamera(dengMath::CameraMatrix *p_vm, const dengMath::vec4<deng_vec_t> &origin);

        
        /// Check if any camera events have occured
        void __findEditorEvent();


        /// Zoom in the editor camera 
        void __zoomIn(dengMath::CameraMatrix *p_vm);


        /// Zoom out the editor camera
        void __zoomOut(dengMath::CameraMatrix *p_vm);


    public:
        __EditorCameraEv(const dengMath::vec2<deng_f64_t> &mouse_sens, deng_vec_t zoom_step,
            const dengMath::vec3<deng_vec_t> &origin, Window *p_ww);
        

        /// Check new editor camera events from IO bindings and perform camera updates
        void updateEv(dengMath::vec3<deng_vec_t> origin, dengMath::CameraMatrix *p_vm);


        /// Set the editor camera bindings
        void setBindings(const Camera3DBindings &bindings);

        
        /// Get the pointer to the window instance
        Window *getWinPtr();

        
        /// Set the window pointer
        void setWinPtr(Window *p_win);
    };
}

#endif
