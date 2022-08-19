// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Window.cpp - Nekowin library abstraction class implementation for DENG
// author: Karl-Mihkel Ott


#define WINDOW_CPP
#include "deng/Window.h"

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


    void Window::ToggleVCMode(TRS::Point2D<float> _origin) {
        m_window.input.cursor.is_virtual = !m_window.input.cursor.is_virtual;

        if (m_window.input.cursor.is_virtual) {
            m_window.input.cursor.orig_x = _origin.x;
            m_window.input.cursor.orig_y = _origin.y;
            neko_SetMouseCoords(&m_window, m_window.input.cursor.orig_x, m_window.input.cursor.orig_y);
        }
    }


    char **Window::FindVulkanSurfaceExtensions(uint32_t* p_ext_c) const {
        return neko_FindRequiredVkExtensionStrings(p_ext_c);
    }


    void Window::ChangeSizeHints(neko_Hint _hints) {
        neko_UpdateSizeMode(&m_window, _hints);
    }


    void Window::ChangeVCMode(bool _is_vcp, TRS::Point2D<float> _origin) {
        m_window.input.cursor.is_virtual = _is_vcp;
        if (m_window.input.cursor.is_virtual) {
            m_window.input.cursor.orig_x = _origin.x;
            m_window.input.cursor.orig_y = _origin.y;
            neko_SetMouseCoords(&m_window, m_window.input.cursor.orig_x, m_window.input.cursor.orig_y);
        }
    }


    void Window::ChangeCursor(neko_CursorMode _cur) {
        neko_SetMouseCursorMode(&m_window, _cur);
    }


    void Window::Update() {
        neko_UpdateWindow(&m_window);
    }


    bool Window::IsRunning() const {
        return m_window.is_running;
    }


    bool Window::IsVirtualCursor() const {
        return m_window.input.cursor.is_virtual;
    }


    bool Window::IsResized() const {
        return m_window.resize_notify;
    }


    VkResult Window::InitVkSurface(VkInstance _instance, VkSurfaceKHR& _surface) {
        return neko_InitVkSurface(&m_window, _instance, &_surface);
    }


    TRS::Point2D<int64_t> Window::GetMousePosition() const {
        return TRS::Point2D<int64_t>(
            m_window.input.cursor.x,
            m_window.input.cursor.y
        );
    }


    TRS::Point2D<int64_t> Window::GetMouseDelta() const {
        return TRS::Point2D<int64_t> {
            m_window.input.cursor.delta_x,
            m_window.input.cursor.delta_y
        };
    }


    neko_Hint Window::GetHints() const {
        return m_window.hints;
    }


    const char* Window::GetTitle() const {
        return m_window.window_title;
    }

    
    TRS::Point2D<int32_t> Window::GetSize() const {
        return TRS::Point2D<int32_t>(m_window.cwidth, m_window.cheight);
    }


    TRS::Point2D<float> Window::GetPixelSize() const {
        return TRS::Point2D<float>{
            2.0f / static_cast<float>(m_window.cwidth),
            2.0f / static_cast<float>(m_window.cheight)
        };
    }


    bool Window::IsHidEventActive(neko_InputBits _bits) const {
        bool is_active = true;
        neko_HidEvent* ev = neko_UnmaskInput(_bits);

        for (int i = 0; i < 8; i++) {
            if (ev[i] == NEKO_HID_UNKNOWN)
                break;

            if (!m_window.input.raw.active_table[ev[i]]) {
                is_active = false;
                break;
            }
        }

        return is_active;
    }


    bool Window::IsKeyPressed(neko_HidEvent _hid) const {
        return m_window.input.raw.active_table[_hid];
    }


    bool Window::IsKeyReleased(neko_HidEvent _hid) const {
        return m_window.input.raw.released_table[_hid];
    }


    const EventQueue* Window::GetActiveEventQueue() const {
        return &m_window.input.raw.active_queue;
    }


    const EventQueue* Window::GetReleasedEventQueue() const {
        return &m_window.input.raw.released_queue;
    }
}
