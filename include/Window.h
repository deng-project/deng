// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Window.h - Nekowin library abstraction class header for DENG
// author: Karl-Mihkel Ott

#ifndef WINDOW_H
#define WINDOW_H

#ifdef WINDOW_CPP
    #include <cstdint>
    #include <vector>
    #include <string>
    #include <vulkan/vulkan.h>
#endif

#include <nekowin/include/nwin.h>

#define NOMINMAX
#define deng_LoadGL             neko_LoadGL
#define deng_InitWindowAPI      neko_InitAPI
#define deng_DeinitWindowAPI    neko_DeinitAPI
#define deng_IsRunning          neko_IsRunning
#define deng_CreateInputMask    neko_CreateInputMask
typedef neko_Window deng_Window;

#ifdef WINDOW_CPP
    #include <Api.h>
    #include <BaseTypes.h>
    #include <ErrorDefinitions.h>
    #include <libdas/include/Points.h>
#endif


namespace DENG {   
    
    class DENG_API Window {
        private:
            neko_Window m_surface;
            Libdas::Point2D<uint64_t> m_prev_vc_pos;

        public:
            Window(int32_t width, int32_t height, neko_Hint hints, const char *title);
            ~Window();


            /// Toggle virtual cursor mode
            void ToggleVCMode();


            /// Search for all required vulkan extensions
            char **FindVulkanSurfaceExtensions(size_t *p_ext_c) const;


            /// Get the current mouse position
            Libdas::Point2D<uint64_t> GetMousePosition() const;


            /// Get the mouse delta compared to previous frame mouse position
            Libdas::Point2D<int64_t> GetMouseDelta() const;


        // inlined methods
        public:
            inline void ChangeSizeHints(neko_Hint _hints) {
                neko_UpdateSizeMode(m_surface, _hints);
            }

            /// Explicitly change virtual cursor mode
            inline void ChangeVCMode(bool is_vcp) {
                neko_ChangeVCMode(is_vcp, m_surface);
            }

            /// Change DENG cursor mode
            inline void ChangeCursor(neko_CursorMode cur) {
                neko_SetMouseCursorMode(m_surface, cur);
            }

            /// Update window and input devices data
            inline void Update() const {
                neko_UpdateWindow(m_surface);
            }

            /// Check if the current window is still active and running
            inline bool IsRunning() const {
                return neko_IsRunning(m_surface);
            }

            /// Check if virtual cursor mode is enabled
            inline bool IsVirtualCursor() const {
                return neko_IsVCMode(m_surface);
            }

            inline bool IsResized() const {
                return neko_ResizeNotify(m_surface);
            }

            /// Create new vulkan surface instance
            inline VkResult InitVkSurface(VkInstance &instance, VkSurfaceKHR &surface) const {
                return neko_InitVKSurface(m_surface, instance, &surface);
            }

            inline neko_Hint GetHints() const {
                neko_Hint hints;
                neko_GetWindowHints(m_surface, &hints);
                return hints;
            }

            inline void SetVSync(bool _on) const {
                // neko_SetVSync(m_surface, _on);
            }

            inline const std::string GetTitle() const {
                return std::string(neko_GetTitle(m_surface));
            }

            inline const Libdas::Point2D<int32_t> GetSize() const {
                Libdas::Point2D<int32_t> pos;
                neko_GetWindowSize(m_surface, &pos.x, &pos.y);
                return pos;
            }

            inline const Libdas::Point2D<float> GetPixelSize() const {
                Libdas::Point2D<float> pix;
                neko_GetPixelSize(m_surface, &pix.x, &pix.y);
                return pix;
            }

            bool IsHidEventActive(neko_InputBits _bits) const;

            inline bool IsKeyPressed(neko_HidEvent _hid) const {
                return neko_FindKeyStatus(_hid, NEKO_INPUT_EVENT_TYPE_ACTIVE);
            }

            inline bool IsKeyReleased(neko_HidEvent _hid) const {
                return neko_FindKeyStatus(_hid, NEKO_INPUT_EVENT_TYPE_RELEASED);
            }
    };
}

#endif
