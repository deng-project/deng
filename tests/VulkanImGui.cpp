// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanImGui.cpp - Vulkan ImGui application implementation
// author: Karl-Mihkel Ott

#define USE_VULKAN
#include "ImGuiTest.h"

int main() {
    DENG::Window::Initialise();
    {
        DENG::Window win = DENG::Window(1280, 720, NEKO_HINT_API_VULKAN | NEKO_HINT_RESIZEABLE, "VulkanImGui");
        DENG::RendererConfig conf = { false, { 0.0f, 0.0f, 0.0f, 0.0f } };
        DENG::VulkanRenderer renderer = DENG::VulkanRenderer(win, conf);
        ImGuiApp app(win, renderer);
        app.Run();
    }
    DENG::Window::Deinitialise();
    return 0;
}
