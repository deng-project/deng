#define USE_OPENGL
#include "Grid3DTest.h"

int main() {
    DENG::Window::Initialise();
    {
        DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_OPENGL, "OpenGL grid test");
        DENG::OpenGL::Initialise(win);
        DENG::OpenGLRenderer rend(win, {});

        Executable::Grid3DTest grid_test(win, rend);
        grid_test.Run();
    }
    DENG::Window::Deinitialise();
    return 0;
}
