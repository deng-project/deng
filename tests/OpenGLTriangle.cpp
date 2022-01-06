#include <string>

#ifdef _DEBUG
#include <iostream>
#endif

#include <vector>
#include <array>
#include <cmath>
#include <cstring>

#include <libdas/include/Vector.h>
#include <libdas/include/Matrix.h>
#include <libdas/include/Quaternion.h>
#include <libdas/include/Points.h>

#include <BaseTypes.h>
#include <Window.h>
#include <VertexAttributes.h>
#include <Renderer.h>
#include <ShaderDefinitions.h>
#include <OpenGLBufferLoader.h>
#include <OpenGLShaderLoader.h>
#include <OpenGLRenderer.h>

#define WIDTH   1280
#define HEIGHT  720

#define FOV     65.0f / 180.0f * PI
#define PLANES  Libdas::Point2D<float>{-0.1f, 25.0f}

static DENG::UnmappedVertexAttribute attrs[] = {
    { {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f} },
    { {0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 0.0f} },
    { {0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f} }
};


uint32_t indices[] = { 0, 1, 2 };


int main() {
    uint32_t camera_reg = 0;
    DENG::Window window(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_OPENGL, "OpenGLTriangle");
    DENG::OpenGLRenderer renderer(window);

    renderer.ClearFrame();
    std::vector<uint8_t> vertices(sizeof(attrs));
    std::memcpy(vertices.data(), attrs, sizeof(attrs));

    std::vector<uint8_t> iv(sizeof(indices));
    std::memcpy(iv.data(), indices, sizeof(indices));

    renderer.UpdateVertexBuffer(vertices);
    renderer.UpdateIndexBuffer(iv);

    DENG::MeshReference mesh = { "Mesh", 0, 0, 3, 0 };
    renderer.PushMeshReference(mesh);

    // main run loop
    while(window.IsRunning()) {
        renderer.ClearFrame();
        renderer.RenderFrame();
        window.Update();
    }
}
