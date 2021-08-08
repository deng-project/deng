/// DENG: dynamic engine - powerful 2D and 3D game engine
/// licence: Apache, see LICENCE file
/// file: window.cpp - nekowin library abstraction class implementation for DENG
/// author: Karl-Mihkel Ott


#define __WINDOW_CPP
#include <deng/window.h>

namespace deng {
    Window::Window(deng_i32_t x, deng_i32_t y, neko_Hint hints, const char *title) {
        m_surface = neko_NewWindow(x, y, hints, title);
    }

    Window::~Window() {
        neko_DestroyWindow(m_surface);
    }


    /// Set virtual cursor mode that locks real cursor to the center of the window
    void Window::toggleVCMode() {    
        if(neko_IsVCMode(m_surface)) {
            neko_SetMouseCursorMode(m_surface, NEKO_CURSOR_MODE_HIDDEN);
            neko_ToggleVCMode(m_surface);
        } 

        else { 
            neko_SetMouseCursorMode(m_surface, NEKO_CURSOR_MODE_STANDARD);
            neko_ToggleVCMode(m_surface);
        }
    }


    void Window::changeVCMode(const deng_bool_t is_vcp) {
        neko_ChangeVCMode(is_vcp, m_surface);
    }
    

    /// Hide the cursor's visbility
    void Window::changeCursor(deng_CursorMode cur) {
        neko_SetMouseCursorMode(m_surface, cur);
    }


    /// Update window and input devices data
    void Window::update() {
        neko_UpdateWindow(m_surface);
    }


    /// Create new vulkan surface instance
    VkResult Window::initVkSurface (
        VkInstance &instance,
        VkSurfaceKHR &surface
    ) {
        return neko_InitVKSurface(m_surface, instance, &surface);
    }


    /// Find all required surface extensions
    char **Window::findVulkanSurfaceExtensions (
        size_t *p_ext_c, 
        deng_bool_t enable_vl
    ) {
        char **exts;
        neko_FindRequiredVkExtensionsStrings(&exts, p_ext_c, enable_vl);

        return exts;
    }


    /// Get the current mouse position
    dengMath::vec2<deng_i64_t> Window::getMPos() const {
        dengMath::vec2<deng_i64_t> pos;
        neko_GetMousePos(m_surface, &pos.first, &pos.second);
        return dengMath::vec2<deng_px_t>{static_cast<deng_px_t>(pos.first), static_cast<deng_px_t>(pos.second)};
    }


    /// Get the mouse delta compared to previous frame mouse position
    dengMath::vec2<deng_px_t> Window::getMDelta() const {
        dengMath::vec2<deng_ui64_t> pos;
        neko_FindDeltaMovement(m_surface, &pos.first, &pos.second);
        return dengMath::vec2<deng_px_t>{static_cast<deng_px_t>(pos.first), static_cast<deng_px_t>(pos.second)};
    }


    /// Get the vector size for one pixel in surface
    dengMath::vec2<deng_vec_t> Window::getPixelSize() const { 
        dengMath::vec2<deng_f32_t> pix;
        neko_GetPixelSize(m_surface, &pix.first, &pix.second);
        return pix;
    }

    
    const deng_WindowHint Window::getHints() const {
        neko_Hint hints;
        neko_GetWindowHints(m_surface, &hints);
        return hints;
    }


    std::string Window::getTitle() const { 
        return std::string(neko_GetTitle(m_surface));
    }


    /// Get the size of the window
    dengMath::vec2<deng_i32_t> Window::getSize() const { 
        dengMath::vec2<deng_i32_t> pos;
        neko_GetWindowSize(m_surface, &pos.first, &pos.second);
        return pos;
    }


    /// Check if virtual cursor mode is enabled
    deng_bool_t Window::isVCP() { return neko_IsVCMode(m_surface); }
}
