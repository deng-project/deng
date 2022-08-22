#define USE_VULKAN
#include "Grid3DTest.h"

int main() {
    DENG::Window::Initialise();
    {
        DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_VULKAN, "Vulkan grid test");
        DENG::Vulkan::Initialise();
        DENG::RendererConfig conf;
        conf.canvas_size = { WIDTH, HEIGHT };
        conf.title = win.GetTitle();
#ifdef _WIN32
        conf.win32_hwnd = win.GetWin32HWND();
        conf.win32_instance = win.GetWin32Instance();
#else
        conf.xlib_dpy = win.GetXlibDisplay();
        conf.xlib_win = win.GetXlibWindow();
#endif
        DENG::VulkanRenderer rend(conf);

        Executable::Grid3DTest grid_test(win, rend, conf);
        grid_test.Run();
    }
    DENG::Window::Deinitialise();
    return 0;
}
