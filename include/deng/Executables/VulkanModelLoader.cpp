// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanModelLoader.cpp - Vulkan model loader application implementation
// author: Karl-Mihkel Ott

#define USE_VULKAN
#include <ModelLoaderApp.h>

int main() {
    // loader.conf file parsing
    std::ifstream file("loader.conf");
    std::string file_name;
    file.seekg(0, std::ios_base::end);
    const size_t fsize = file.tellg();
    file_name.resize(fsize);
    file.seekg(0, std::ios_base::beg);
    file.read(file_name.data(), fsize);
    file.close();

    if(file_name.back() == '\n')
        file_name = file_name.substr(0, file_name.size() - 1);

    DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_VULKAN, "VulkanModelLoader");
    DENG::RendererConfig conf = { false, true };
    DENG::VulkanRenderer rend(win, conf);
    ModelLoaderApp app(file_name, win, rend);
    app.Run();
    return EXIT_SUCCESS;
}
