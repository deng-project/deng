// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanModelLoader.cpp - Vulkan model loader application implementation
// author: Karl-Mihkel Ott

#define USE_VULKAN
#include <ModelLoaderApp.h>

int main() {
    DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_VULKAN, "VulkanModelLoader");
    DENG::RendererConfig conf = {};
    DENG::VulkanRenderer rend(win, conf);
    ModelLoaderApp app(win, rend);
    app.Run();
    return EXIT_SUCCESS;
}
