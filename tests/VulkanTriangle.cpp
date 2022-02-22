// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanTriangle.cpp - Testing application that display's UV mapped triangle using Vulkan backend 
// author: Karl-Mihkel Ott

#define USE_VULKAN
#include "TriangleApp.h"

int main() {
    DENG::Window win = DENG::Window(WIDTH, HEIGHT, NEKO_HINT_API_VULKAN | NEKO_HINT_RESIZEABLE, "VulkanTriangle");
    DENG::VulkanRenderer renderer = DENG::VulkanRenderer(win);
    TriangleApp app = TriangleApp(win, renderer);
    app.Run();
    return 0;
}
