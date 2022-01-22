/// DENG: dynamic engine - small but powerful 2D and 3D game engine
/// licence: Apache, see LICENCE file
/// file: Window.cpp - Nekowin library abstraction class implementation for DENG
/// author: Karl-Mihkel Ott


#define WINDOW_CPP
#include <Window.h>

namespace DENG {

    Window::Window(int32_t x, int32_t y, WindowHint hints, const char *title) {
        neko_InitAPI();
        m_surface = neko_NewWindow(x, y, hints, title);
        neko_SetMouseCursorMode(m_surface, NEKO_CURSOR_MODE_STANDARD);
    }

    Window::~Window() {
        neko_DestroyWindow(m_surface);
        neko_DeinitAPI();
    }


    /// Set virtual cursor mode that locks real cursor to the center of the window
    void Window::ToggleVCMode() {    
        if(neko_IsVCMode(m_surface)) {
            neko_SetMouseCursorMode(m_surface, NEKO_CURSOR_MODE_HIDDEN);
            neko_ToggleVCMode(m_surface);
        } 

        else { 
            neko_SetMouseCursorMode(m_surface, NEKO_CURSOR_MODE_STANDARD);
            neko_ToggleVCMode(m_surface);
        }
    }


    /// Find all required surface extensions
    char **Window::FindVulkanSurfaceExtensions(size_t *p_ext_c) const {
        char **exts;
        neko_FindRequiredVkExtensionsStrings(&exts, p_ext_c);
        return exts;
    }


    /// Get the current mouse position
    Libdas::Point2D<uint64_t> Window::GetMousePosition() const {
        Libdas::Point2D<int64_t> pos;
        neko_GetMousePos(m_surface, &pos.x, &pos.y);
        return Libdas::Point2D<uint64_t>(static_cast<uint64_t>(pos.x), static_cast<uint64_t>(pos.y));
    }


    /// Get the mouse delta compared to previous frame mouse position
    Libdas::Point2D<uint64_t> Window::GetMouseDelta() const {
        Libdas::Point2D<uint64_t> pos;
        neko_FindDeltaMovement(m_surface, &pos.x, &pos.y);
        return Libdas::Point2D<uint64_t>(static_cast<uint64_t>(pos.x), static_cast<uint64_t>(pos.y));
    }
}
