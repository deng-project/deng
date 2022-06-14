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

#include <csignal>
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
            neko_Window m_window;
            Libdas::Point2D<uint64_t> m_prev_vc_pos;

        public:
            Window(int32_t width, int32_t height, neko_Hint hints, const char *title);
            ~Window();

            void glMakeCurrent();

            static neko_InputBits CreateInputMask(neko_HidEvent evs[8]);

            /// Toggle virtual cursor mode
            void ToggleVCMode();

            /// Search for all required vulkan extensions
            char **FindVulkanSurfaceExtensions(uint32_t *p_ext_c) const;
            void ChangeSizeHints(neko_Hint _hints);

            /// Explicitly change virtual cursor mode
            void ChangeVCMode(bool is_vcp);

            /// Change DENG cursor mode
            void ChangeCursor(neko_CursorMode cur);

            /// Update window and input devices data
            void Update();

            /// Check if the current window is still active and running
            inline bool IsRunning() const {
                return m_window.is_running;
            }

            /// Check if virtual cursor mode is enabled
            inline bool IsVirtualCursor() const {
                return m_window.vc_data.is_enabled;
            }

            inline bool IsResized() const {
                return m_window.resize_notify;
            }

            /// Create new vulkan surface instance
            VkResult InitVkSurface(VkInstance _instance, VkSurfaceKHR &_surface);

            inline Libdas::Point2D<uint64_t> GetMousePosition() const {
                Libdas::Point2D<uint64_t> pos;
                if(m_window.vc_data.is_enabled) {
                    pos.x = static_cast<uint64_t>(m_window.vc_data.x);
                    pos.y = static_cast<uint64_t>(m_window.vc_data.y);
                } else {
                    pos.x = static_cast<uint64_t>(m_window.mx);
                    pos.y = static_cast<uint64_t>(m_window.my);
                }
                return pos;
            }

            Libdas::Point2D<int64_t> GetMouseDelta();
            
            inline neko_Hint GetHints() const {
                return m_window.hints;
            }

            inline std::string GetTitle() const {
                return std::string(m_window.window_title);
            }

            inline Libdas::Point2D<int32_t> GetSize() const {
                Libdas::Point2D<int32_t> size = { m_window.cwidth, m_window.cheight };
                return size;
            }

            inline Libdas::Point2D<float> GetPixelSize() const {
                Libdas::Point2D<float> pix = { 
                    2.0f / static_cast<float>(m_window.cwidth),
                    2.0f / static_cast<float>(m_window.cheight)
                };

                return pix;
            }

            bool IsHidEventActive(neko_InputBits _bits) const;
            bool IsKeyPressed(neko_HidEvent _hid) const;
            bool IsKeyReleased(neko_HidEvent _hid) const;
    };
}

#endif
