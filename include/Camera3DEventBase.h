/// NEKO: dynamic engine - small but powerful 2D and 3D game engine
/// licence: Apache, see LICENCE file
/// file: Camera3DEventBase.h - 3D camera base event class header
/// author: Karl-Mihkel Ott


#ifndef CAMERA_3D_EVENT_BASE_H
#define CAMERA_3D_EVENT_BASE_H


#ifdef CAMERA_3D_EVENT_BASE_CPP
    #include <string>
    #include <string.h>
    #include <vulkan/vulkan.h>

    #include <BaseTypes.h>
    #include <ErrorDefinitions.h>

    #include <libdas/include/Points.h>
    #include <Window.h>
    #include <CameraControlBindings.h>
#endif


#define BASE_MAX_VC_X   720
#define BASE_MAX_VC_Y   180


namespace DENG {

    class Camera3DEventBase {
    protected:
        Window *m_p_win = nullptr;
        Libdas::Point2D<uint64_t> m_mouse_pos = Libdas::Point2D<uint64_t>(0, 0);
        Libdas::Point2D<neko_VCPOverflowAction> m_vcp_overflow;
        Libdas::Point2D<Libdas::Point2D<int64_t>> m_vc_bounds;
        Libdas::Point2D<double> m_max_rot;
        CameraControlBindings m_bindings;

    protected:

        /// Camera mouse position update method 
        inline void _UpdateCameraMousePos() {
            m_mouse_pos = m_p_win->GetMousePosition();
        }


        /// Find the current mouse control rotation 
        Libdas::Point2D<double> _GetMouseRotation();


        /// Check if input input conditions are satified for certain action
        bool _CheckInputAction(CameraAction action);

    public:
        Camera3DEventBase (
            const Libdas::Point2D<neko_VCPOverflowAction> &vcp_act,
            const Libdas::Point2D<Libdas::Point2D<int64_t>> &vc_bounds,
            const Libdas::Point2D<double> &max_rot,
            Window *p_win
        );
    };
}

#endif
