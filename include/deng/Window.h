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
#include "nwin/nwin.h"

#define NOMINMAX
#define deng_LoadGL             neko_LoadGL
#define deng_InitWindowAPI      neko_InitAPI
#define deng_DeinitWindowAPI    neko_DeinitAPI
#define deng_IsRunning          neko_IsRunning
#define deng_CreateInputMask    neko_CreateInputMask
typedef neko_Window deng_Window;

#ifdef WINDOW_CPP
    #include "trs/Points.h"

    #include "deng/Api.h"
    #include "deng/BaseTypes.h"
    #include "deng/ErrorDefinitions.h"
#endif


namespace DENG {   
    
    class DENG_API Window {
        private:
            neko_Window m_window;

        public:
            Window(int32_t width, int32_t height, neko_Hint hints, const char *title);
            ~Window();

            static neko_InputBits CreateInputMask(neko_HidEvent evs[8]);
            static void Initialise();
            static void Deinitialise();

            void glMakeCurrent();

            /// Toggle virtual cursor mode
            void ToggleVCMode(TRS::Point2D<float> _origin);

            /// Search for all required vulkan extensions
            char **FindVulkanSurfaceExtensions(uint32_t *p_ext_c) const;
            void ChangeSizeHints(neko_Hint _hints);

            /// Explicitly change virtual cursor mode
            void ChangeVCMode(bool is_vcp, TRS::Point2D<float> _origin);

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
                return m_window.input.cursor.is_virtual;
            }

            inline bool IsResized() const {
                return m_window.resize_notify;
            }

            /// Create new vulkan surface instance
            VkResult InitVkSurface(VkInstance _instance, VkSurfaceKHR &_surface);

            inline TRS::Point2D<uint64_t> GetMousePosition() const {
                return TRS::Point2D<uint64_t>(static_cast<uint64_t>(m_window.input.cursor.x), static_cast<uint64_t>(m_window.input.cursor.y));
            }

            inline TRS::Point2D<int64_t> GetMouseDelta() {
                return TRS::Point2D<int64_t> {
                    m_window.input.cursor.orig_x - m_window.input.cursor.x, // invalid
                    m_window.input.cursor.orig_y - m_window.input.cursor.y 
                };
            }
            
            inline neko_Hint GetHints() const {
                return m_window.hints;
            }

            inline std::string GetTitle() const {
                return std::string(m_window.window_title);
            }

            inline TRS::Point2D<int32_t> GetSize() const {
                TRS::Point2D<int32_t> size = { m_window.cwidth, m_window.cheight };
                return size;
            }

            inline TRS::Point2D<float> GetPixelSize() const {
                TRS::Point2D<float> pix = { 
                    2.0f / static_cast<float>(m_window.cwidth),
                    2.0f / static_cast<float>(m_window.cheight)
                };

                return pix;
            }

            bool IsHidEventActive(neko_InputBits _bits) const;

            inline bool IsKeyPressed(neko_HidEvent _hid) const {
                return m_window.input.raw.active_table[_hid];
            }

            inline bool IsKeyReleased(neko_HidEvent _hid) const {
                return m_window.input.raw.released_table[_hid];
            }

            inline const EventQueue *GetActiveEventQueue() const {
                return &m_window.input.raw.active_queue;
            }

            inline const EventQueue *GetReleasedEventQueue() const {
                return &m_window.input.raw.released_queue;
            }
    };
}

#endif
