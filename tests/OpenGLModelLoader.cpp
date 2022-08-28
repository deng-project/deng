// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLModelLoader.cpp - OpenGL model loader application implementation
// author: Karl-Mihkel Ott

#define USE_OPENGL
#include <ModelLoaderApp.h>

int main() {
    std::ifstream file("loader.conf");
    std::string file_name;
    file.seekg(std::ios_base::end);
    const size_t fsize = file.tellg();
    file_name.reserve(fsize);
    file.seekg(std::ios_base::beg);
    file.read(file_name.data(), fsize);
    file.close();

    DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_VULKAN, "VulkanModelLoader");
    DENG::RendererConfig conf = {};
    DENG::OpenGLRenderer rend(win, conf);
    ModelLoaderApp app(file_name, win, rend);
    app.Run();
    return EXIT_SUCCESS;
}
