// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLImGui.h - OpenGL ImGui application implementation
// author: Karl-Mihkel Ott

#define USE_OPENGL
#include "ImGuiTest.h"

int main() {
    DENG::Window win = DENG::Window(1280, 720, DENG_WINDOW_HINT_API_OPENGL | DENG_WINDOW_HINT_RESIZEABLE, "OpenGLImGui");
    DENG::OpenGLRenderer renderer = DENG::OpenGLRenderer(win);
    ImGuiApp app(win, renderer);
    app.Run();
    return 0;
}
