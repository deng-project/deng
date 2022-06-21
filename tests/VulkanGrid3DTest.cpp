#define USE_VULKAN
#include "Grid3DTest.h"

int main() {
    DENG::Window::Initialise();
    {
        DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_VULKAN, "Vulkan grid test");
        DENG::Vulkan::Initialise();
        DENG::VulkanRenderer rend(win, {});

        Executable::Grid3DTest grid_test(win, rend);
        grid_test.Run();
    }
    DENG::Window::Deinitialise();
    return 0;
}
