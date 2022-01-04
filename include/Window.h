/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: Window.h - Nekowin library abstraction class header for DENG
/// author: Karl-Mihkel Ott

#ifndef WINDOW_H
#define WINDOW_H

#ifdef WINDOW_CPP
    #include <stdlib.h>
    #include <vector>
    #include <string>
    #include <string.h>

    #include <base_types.h>
    #include <err_def.h>
    #include <assets.h>
    #include <deng_math.h>
#endif

#define NOMINMAX
#define deng_LoadGL             neko_LoadGL
#define deng_InitWindowAPI      neko_InitAPI
#define deng_DeinitWindowAPI    neko_DeinitAPI
#define deng_IsRunning          neko_IsRunning
#define deng_CreateInputMask    neko_CreateInputMask
typedef neko_Window deng_Window;

#include <nekowin/include/nwin.h>


namespace DENG {   
    
    class Window {
    private:
        neko_Window m_surface;
        DENG::vec2<deng_px_t> m_prev_vc_pos;

    public:
        Window(deng_i32_t width, deng_i32_t height, deng_WindowHint win_hints, const char *title);
        ~Window();


        /// Toggle virtual cursor mode
        void toggleVCMode();


        /// Explicitly change virtual cursor mode
        void changeVCMode(const deng_bool_t is_vcp);


        /// Change DENG cursor mode
        void changeCursor(deng_CursorMode cur);


        /// Check if virtual cursor mode is enabled
        const deng_bool_t isVCP();


        const deng_bool_t resizeNotify();


        /// Update window and input devices data
        void update();


        /// Check if the current window is still active and running
        const bool isRunning();


        /// Create new vulkan surface instance
        VkResult initVkSurface(VkInstance &instance,
            VkSurfaceKHR &surface);


        /// Search for all required vulkan extensions
        char **findVulkanSurfaceExtensions(size_t *p_ext_c, deng_bool_t enable_vl);


        /// Get the current mouse position
        dengMath::vec2<deng_px_t> getMPos() const;


        /// Get the mouse delta compared to previous frame mouse position
        dengMath::vec2<deng_px_t> getMDelta() const;


    // Getter methods
    public:
        const deng_WindowHint getHints() const;
        const std::string getTitle() const;
        dengMath::vec2<deng_i32_t> getSize() const;
        dengMath::vec2<deng_vec_t> getPixelSize() const;
    };
}

#endif
