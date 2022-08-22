// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLTriangle.cpp - Testing application that display's UV mapped triangle using OpenGL backend 
// author: Karl-Mihkel Ott

#define USE_OPENGL
#include "TriangleApp.h"

int main() {
    DENG::Window::Initialise();
    {
        DENG::Window win = DENG::Window(WIDTH, HEIGHT, NEKO_HINT_API_OPENGL | NEKO_HINT_RESIZEABLE, "OpenGLTriangle");
        DENG::OpenGL::Initialise(win);
        DENG::RendererConfig conf = {};
        conf.canvas_size = { WIDTH, HEIGHT };
        conf.title = win.GetTitle();

        DENG::OpenGLRenderer renderer = DENG::OpenGLRenderer(conf);
        TriangleApp app = TriangleApp(win, renderer, conf);
        app.Run();
    }
    DENG::Window::Deinitialise();
    return 0;
}
