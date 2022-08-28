#define USE_OPENGL
#include "Grid3DTest.h"

int main() {
    DENG::Window::Initialise();
    {
        DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE, "OpenGL grid test");
        DENG::RendererConfig conf;
        conf.canvas_size = { WIDTH, HEIGHT };
        conf.title = win.GetTitle();
        DENG::OpenGL::Initialise(win);
        DENG::OpenGLRenderer rend(conf);

        Executable::Grid3DTest grid_test(win, rend, conf);
        grid_test.Run();
    }
    DENG::Window::Deinitialise();
    return 0;
}
