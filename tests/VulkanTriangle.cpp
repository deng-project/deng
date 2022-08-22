// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanTriangle.cpp - Testing application that display's UV mapped triangle using Vulkan backend 
// author: Karl-Mihkel Ott

#define USE_VULKAN
#include "TriangleApp.h"

int main() {
    DENG::Window::Initialise();
    {
        DENG::Window win = DENG::Window(WIDTH, HEIGHT, NEKO_HINT_API_VULKAN | NEKO_HINT_RESIZEABLE, "VulkanTriangle");
        DENG::RendererConfig conf = {};
        conf.canvas_size = { WIDTH, HEIGHT };
        conf.title = win.GetTitle();
#ifdef _WIN32
        conf.win32_hwnd = win.GetWin32HWND();
        conf.win32_instance = win.GetWin32Instance();
#else
        conf.xlib_dpy = win.GetXlibDisplay();
        conf.xlib_win = win.GetXlibWindow();
#endif
        DENG::VulkanRenderer renderer = DENG::VulkanRenderer(conf);
        TriangleApp app = TriangleApp(win, renderer, conf);
        app.Run();
    }
    DENG::Window::Deinitialise();
    return 0;
}
