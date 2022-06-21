#define USE_OPENGL
#include "MultiFramebuffer.h"

int main() {
    DENG::Window::Initialise();
    {
        DENG::Window win = DENG::Window(WIDTH, HEIGHT, NEKO_HINT_API_OPENGL | NEKO_HINT_RESIZEABLE, "OpenGL multiple framebuffer test");
        DENG::OpenGL::Initialise(win);
        DENG::OpenGLRenderer rend(win, {});

        Executable::MultiFramebuffer mf(win, rend);
        mf.Run();
    }
    DENG::Window::Deinitialise();
    return 0;
}
