// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLImGui.h - OpenGL ImGui application implementation
// author: Karl-Mihkel Ott

#define USE_OPENGL
#include "ImGuiTest.h"

int main() {
    DENG::Window::Initialise();
    {
        DENG::Window win = DENG::Window(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE, "OpenGLImGui");
        DENG::OpenGL::Initialise(win);
        DENG::RendererConfig conf = {};
        conf.canvas_size = { WIDTH, HEIGHT };
        conf.title = win.GetTitle();
        DENG::OpenGLRenderer renderer = DENG::OpenGLRenderer(conf);
        ImGuiApp app(win, renderer, conf);
        app.Run();
    }
    DENG::Window::Deinitialise();
    return 0;
}
