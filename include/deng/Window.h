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
            Window(int32_t width, int32_t height, neko_SizeHint _size, const char *title);
            ~Window();

            static neko_InputBits CreateInputMask(neko_HidEvent evs[8]);
            static void Initialise();
            static void Deinitialise();
            static void LoadOpenGLPlatform();
            static void LoadOpenGLFunctions();

            void CreateOpenGLContext();
            void MakeOpenGLContextCurrent();
            void SwapBuffers();

            /// Toggle virtual cursor mode
            void ToggleVCMode(TRS::Point2D<int64_t> _origin);

            /// Search for all required vulkan extensions
            char **FindVulkanSurfaceExtensions(uint32_t *p_ext_c) const;
            void ChangeSizeHints(neko_SizeHint _hints);

            /// Explicitly change virtual cursor mode
            void ChangeVCMode(bool is_vcp, TRS::Point2D<int64_t> _origin);

            /// Change DENG cursor mode
            void ChangeCursor(neko_CursorMode cur);

            /// Update window and input devices data
            void Update();

            /// Check if the current window is still active and running
            bool IsRunning() const;

            /// Check if virtual cursor mode is enabled
            bool IsVirtualCursor() const;

            TRS::Point2D<int64_t> GetMousePosition() const;
            TRS::Point2D<int64_t> GetMouseDelta() const;
            neko_SizeHint GetSizeHints() const;
            const char* GetTitle() const;
            TRS::Point2D<int32_t> GetSize() const;
            inline TRS::Point2D<float> GetPixelSize() const;

            bool IsHidEventActive(neko_InputBits _bits) const;
            bool IsKeyPressed(neko_HidEvent _hid) const;
            bool IsKeyReleased(neko_HidEvent _hid) const;
            const EventQueue* GetActiveEventQueue() const;
            const EventQueue* GetReleasedEventQueue() const;

#ifdef _WIN32
            HWND GetWin32HWND();
            HINSTANCE GetWin32Instance();
#else
            Window GetXlibWindow();
            Display* GetXlibDisplay();
#endif
    };
}

#endif
