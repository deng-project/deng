#define USE_VULKAN
#include "MultiFramebuffer.h"

int main() {
    DENG::Window::Initialise();
    {
        DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_API_VULKAN | NEKO_HINT_RESIZEABLE, "Vulkan multiple framebuffer test");
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

        Executable::MultiFramebuffer mf(win, rend, conf);
        mf.Run();
    }
    DENG::Window::Deinitialise();
    return 0;
}
