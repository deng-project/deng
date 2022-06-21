#define USE_VULKAN
#include "MultiFramebuffer.h"

int main() {
    DENG::Window::Initialise();
    {
        DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_API_VULKAN | NEKO_HINT_RESIZEABLE, "Vulkan multiple framebuffer test");
        DENG::VulkanRenderer rend(win, {});

        Executable::MultiFramebuffer mf(win, rend);
        mf.Run();
    }
    DENG::Window::Deinitialise();
    return 0;
}
