// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLImGui.h - OpenGL ImGui application implementation
// author: Karl-Mihkel Ott

#define USE_OPENGL
#include "ImGuiTest.h"

int main() {
    DENG::Window win = DENG::Window(1280, 720, NEKO_HINT_API_OPENGL | NEKO_HINT_RESIZEABLE, "OpenGLImGui");
    DENG::OpenGL::Initialise(win);
    DENG::RendererConfig conf = {}; // keep default
    DENG::OpenGLRenderer renderer = DENG::OpenGLRenderer(win, conf);
    ImGuiApp app(win, renderer);
    app.Run();
    return 0;
}
