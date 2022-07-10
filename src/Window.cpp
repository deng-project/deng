// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Window.cpp - Nekowin library abstraction class implementation for DENG
// author: Karl-Mihkel Ott


#define WINDOW_CPP
#include <Window.h>

namespace DENG {

    Window::Window(int32_t _x, int32_t _y, neko_Hint _hints, const char *_title) {
        m_window = neko_NewWindow(_x, _y, _hints, 0, 0, _title);
    }

    Window::~Window() {
        neko_DestroyWindow(&m_window);
    }

    void Window::Initialise() {
        neko_InitAPI();
    }

    void Window::Deinitialise() {
        neko_DeinitAPI();
    }

    void Window::glMakeCurrent() {
        neko_glMakeCurrent(&m_window);
    }

    // static method:
    neko_InputBits Window::CreateInputMask(neko_HidEvent evs[8]) {
        return neko_CreateInputMask(evs);
    }


    void Window::ToggleVCMode(Libdas::Point2D<float> _origin) {
        m_window.vc_data.is_enabled = !m_window.vc_data.is_enabled;

        if (m_window.vc_data.is_enabled) {
            m_window.vc_data.orig_x = _origin.x;
            m_window.vc_data.orig_y = _origin.y;
            neko_SetMouseCoords(&m_window, m_window.vc_data.orig_x, m_window.vc_data.orig_y);
        }
    }


    char **Window::FindVulkanSurfaceExtensions(uint32_t* p_ext_c) const {
        return neko_FindRequiredVkExtensionStrings(p_ext_c);
    }


    void Window::ChangeSizeHints(neko_Hint _hints) {
        neko_UpdateSizeMode(&m_window, _hints);
    }


    void Window::ChangeVCMode(bool _is_vcp, Libdas::Point2D<float> _origin) {
        m_window.vc_data.is_enabled = _is_vcp;
        if (m_window.vc_data.is_enabled) {
            m_window.vc_data.orig_x = _origin.x;
            m_window.vc_data.orig_y = _origin.y;
            neko_SetMouseCoords(&m_window, m_window.vc_data.orig_x, m_window.vc_data.orig_y);
        }
    }


    void Window::ChangeCursor(neko_CursorMode _cur) {
        neko_SetMouseCursorMode(&m_window, _cur);
    }


    void Window::Update() {
        neko_UpdateWindow(&m_window);
    }


    VkResult Window::InitVkSurface(VkInstance _instance, VkSurfaceKHR& _surface) {
        return neko_InitVkSurface(&m_window, _instance, &_surface);
    }


    Libdas::Point2D<int64_t> Window::GetMouseDelta() {
        Libdas::Point2D<int64_t> pos;
        neko_FindDeltaMovement(&m_window, &pos.x, &pos.y);
        return pos;
    }
    
    
    bool Window::IsHidEventActive(neko_InputBits _bits) const {
        bool is_active = true;
        neko_HidEvent* ev = neko_UnmaskInput(_bits);

        for (int i = 0; i < 8; i++) {
            if (ev[i] == NEKO_HID_UNKNOWN)
                break;

            if (!neko_FindKeyStatus(ev[i], NEKO_INPUT_EVENT_TYPE_ACTIVE)) {
                is_active = false;
                break;
            }
        }

        return is_active;
    }


    bool Window::IsKeyPressed(neko_HidEvent _hid) const {
        return neko_FindKeyStatus(_hid, NEKO_INPUT_EVENT_TYPE_ACTIVE);
    }


    bool Window::IsKeyReleased(neko_HidEvent _hid) const {
        return neko_FindKeyStatus(_hid, NEKO_INPUT_EVENT_TYPE_RELEASED);
    }
}
