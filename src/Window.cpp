// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Window.cpp - Nekowin library abstraction class implementation for DENG
// author: Karl-Mihkel Ott


#define WINDOW_CPP
#include <Window.h>

namespace DENG {

    Window::Window(int32_t x, int32_t y, neko_Hint hints, const char *title) {
        neko_InitAPI();
        m_surface = neko_NewWindow(x, y, hints, title);
        neko_SetMouseCursorMode(m_surface, NEKO_CURSOR_MODE_STANDARD);
    }

    Window::~Window() {
        neko_DestroyWindow(m_surface);
        neko_DeinitAPI();
    }

    // static method:
    neko_InputBits Window::CreateInputMask(uint32_t _ev_c, ...) {
        va_list args;
        va_start(args, _ev_c);
        neko_InputBits bits = neko_CreateInputMask(_ev_c, args);
        va_end(args);
        return bits;
    }



    void Window::ToggleVCMode() {
        if (neko_IsVCMode(m_surface)) {
            neko_SetMouseCursorMode(m_surface, NEKO_CURSOR_MODE_HIDDEN);
            neko_ToggleVCMode(m_surface);
        }

        else {
            neko_SetMouseCursorMode(m_surface, NEKO_CURSOR_MODE_STANDARD);
            neko_ToggleVCMode(m_surface);
        }
    }


    void Window::ChangeSizeHints(neko_Hint _hints) {
        neko_UpdateSizeMode(m_surface, _hints);
    }


    void Window::ChangeVCMode(bool _is_vcp) {
        neko_ChangeVCMode(_is_vcp, m_surface);
    }


    char** Window::FindVulkanSurfaceExtensions(size_t* p_ext_c) const {
        char** exts;
        neko_FindRequiredVkExtensionsStrings(&exts, p_ext_c);
        return exts;
    }


    void Window::ChangeCursor(neko_CursorMode _cur) {
        neko_SetMouseCursorMode(m_surface, _cur);
    }


    void Window::Update() {
        neko_UpdateWindow(m_surface);
    }


    bool Window::IsRunning() const {
        return neko_IsRunning(m_surface);
    }


    bool Window::IsVirtualCursor() const {
        return neko_IsVCMode(m_surface);
    }


    bool Window::IsResized() const {
        return neko_ResizeNotify(m_surface);
    }

    VkResult Window::InitVkSurface(VkInstance _instance, VkSurfaceKHR& _surface) {
        return neko_InitVKSurface(m_surface, _instance, &_surface);
    }


    Libdas::Point2D<uint64_t> Window::GetMousePosition() const {
        Libdas::Point2D<int64_t> pos;
        neko_GetMousePos(m_surface, &pos.x, &pos.y);
        return Libdas::Point2D<uint64_t>(static_cast<uint64_t>(pos.x), static_cast<uint64_t>(pos.y));
    }


    Libdas::Point2D<int64_t> Window::GetMouseDelta() const {
        Libdas::Point2D<int64_t> pos;
        neko_FindDeltaMovement(m_surface, &pos.x, &pos.y);
        return pos;
    }


    neko_Hint Window::GetHints() const {
        neko_Hint hints;
        neko_GetWindowHints(m_surface, &hints);
        return hints;
    }


    const std::string Window::GetTitle() const {
        return std::string(neko_GetTitle(m_surface));
    }


    const Libdas::Point2D<int32_t> Window::GetSize() const {
        Libdas::Point2D<int32_t> pos;
        neko_GetWindowSize(m_surface, &pos.x, &pos.y);
        return pos;
    }


    const Libdas::Point2D<float> Window::GetPixelSize() const {
        Libdas::Point2D<float> pix;
        neko_GetPixelSize(m_surface, &pix.x, &pix.y);
        return pix;
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
