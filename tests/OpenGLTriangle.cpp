#include <string>

#ifdef _DEBUG
#include <iostream>
#endif

#include <vector>
#include <thread>
#include <chrono>
#include <array>
#include <cmath>
#include <cstring>

#include <libdas/include/Vector.h>
#include <libdas/include/Matrix.h>
#include <libdas/include/Quaternion.h>
#include <libdas/include/Points.h>

#include <Api.h>
#include <ErrorDefinitions.h>
#include <BaseTypes.h>
#include <Window.h>
#include <VertexAttributes.h>
#include <ShaderDefinitions.h>
#include <Renderer.h>
#include <VulkanRenderer.h>
#include <OpenGLRenderer.h>

#define WIDTH   1280
#define HEIGHT  720

static float verts[] = {
    -0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f
};


uint32_t indices[] = { 0, 1, 2 };


int main() {
    DENG::ShaderModule module;
    module.vertex_shader_src = 
        "#version 450\n"\
        "#extension GL_ARB_separate_shader_objects : enable\n"\
        "layout(location = 0) in vec3 in_pos;\n"\
        "\n"\
        "void main() {\n"\
        "   gl_Position = vec4(in_pos, 1.0f);\n"\
        "}\n";

    module.fragment_shader_src =
        "#version 450\n"\
        "#extension GL_ARB_separate_shader_objects : enable\n"\
        "layout(location = 0) out vec4 out_color;\n"\
        "\n"\
        "void main() {\n"\
        "   out_color = vec4(0.8f, 0.2f, 0.2f, 1.0f);\n"\
        "}\n";

    module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
    module.strides.push_back(sizeof(float) * 3);
    module.offsets.push_back(0);
    module.load_shaders_from_file = false;

    DENG::Window window(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_VULKAN, "OpenGLTriangle");
    DENG::VulkanRenderer renderer(window);

    renderer.ClearFrame();
    renderer.PushShader(&module);
    renderer.UpdateVertexBuffer(std::make_pair(reinterpret_cast<char*>(verts), static_cast<uint32_t>(sizeof(verts))));
    renderer.UpdateIndexBuffer(std::make_pair(reinterpret_cast<char*>(indices), static_cast<uint32_t>(sizeof(indices))));

    DENG::MeshReference mesh = { "Mesh", 0, 0, 3, 0 };
    renderer.PushMeshReference(mesh);
    renderer.LoadShaders();

    // main run loop
    while(window.IsRunning()) {
        renderer.ClearFrame();
        renderer.RenderFrame();
        window.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
