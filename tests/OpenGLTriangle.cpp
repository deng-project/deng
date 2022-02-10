// win32 and release mode is used - make sure that cmd does not spawn
#if defined(_WIN32) && !defined(_DEBUG)
    #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

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
#include <ShaderDefinitions.h>
#include <Renderer.h>
#include <VulkanRenderer.h>
#include <OpenGLRenderer.h>

#define WIDTH   1280
#define HEIGHT  720

static float verts[] = {
    -0.5f, -0.5f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.5f, 0.0f,
    0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    1.0f, 1.0f
};


uint32_t indices[] = { 0, 1, 2 };


int main() {
    DENG::ShaderModule module;
    module.vertex_shader_file = "OpenGLTriangleData/VertexShader.glsl";
    module.fragment_shader_file = "OpenGLTriangleData/FragmentShaderMapped.glsl";

    // create uniform buffer layouts
    std::array<DENG::UniformDataLayout, 3> layouts;
    layouts[0].binding = 0;
    layouts[0].stage = SHADER_STAGE_VERTEX;
    layouts[0].type = DENG::UNIFORM_DATA_TYPE_BUFFER;
    layouts[0].ubo_size = sizeof(Libdas::Matrix4<float>);

    layouts[1].binding = 1;
    layouts[1].stage = SHADER_STAGE_VERTEX;
    layouts[1].type = DENG::UNIFORM_DATA_TYPE_BUFFER;
    layouts[1].ubo_size = sizeof(Libdas::Matrix4<float>);

    layouts[2].binding = 2;
    layouts[2].stage = SHADER_STAGE_FRAGMENT;
    layouts[2].type = DENG::UNIFORM_DATA_TYPE_IMAGE_SAMPLER;

    module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
    module.strides.push_back(sizeof(float) * 5);
    module.offsets.push_back(0);
    
    module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC2_FLOAT);
    module.strides.push_back(sizeof(float) * 5);
    module.offsets.push_back(sizeof(float) * 3);

    module.ubo_data_layouts.insert(module.ubo_data_layouts.end(), layouts.begin(), layouts.end());
    module.load_shaders_from_file = true;

    DENG::Window window(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_VULKAN, "OpenGLTriangle");
    DENG::VulkanRenderer renderer(window);
    
    // rotate the triangle 60 degrees along x axis
    const float u_rotation = PI / 3;
    Libdas::Matrix4<float> x_rot = Libdas::Matrix4<float> {
        { 1, 0, 0, 0 }, 
        { 0, cosf(u_rotation), -sinf(u_rotation), 0 }, 
        { 0, sinf(u_rotation), cosf(u_rotation), 0 }, 
        { 0, 0, 0, 1 }
    };

    Libdas::Matrix4<float> y_rot = Libdas::Matrix4<float>{
        { cosf(u_rotation), 0, sinf(u_rotation), 0 },
        { 0, 1, 0, 0 },
        { -sinf(u_rotation), 0, cosf(u_rotation), 0 },
        { 0, 0, 0, 1}
    };

    renderer.ClearFrame();
    renderer.PushShader(&module);
    renderer.UpdateVertexBuffer(std::make_pair(reinterpret_cast<char*>(verts), static_cast<uint32_t>(sizeof(verts))));
    renderer.UpdateIndexBuffer(std::make_pair(reinterpret_cast<char*>(indices), static_cast<uint32_t>(sizeof(indices))));
    renderer.UpdateUniform(reinterpret_cast<char*>(&x_rot), 0, 0);
    renderer.UpdateUniform(reinterpret_cast<char*>(&y_rot), 0, 1);

    DENG::MeshReference mesh;
    mesh.indices_count = 3;
    mesh.indices_offset = 0;
    mesh.name = "Generic mesh";
    mesh.shader_module_id = 0;
    mesh.texture_id = 0;
    mesh.vertices_offset = 0;

    DENG::TextureReference tex;
    tex.name = "Texture";
    tex.shader_module_id = 0;

    renderer.PushMeshReference(mesh);
    renderer.PushTextureFromFile(tex, "OpenGLTriangleData/felix_texture.png");
    renderer.LoadShaders();

    // main run loop
    while(window.IsRunning()) {
        renderer.ClearFrame();
        renderer.RenderFrame();
        window.Update();
        // renderer.UpdateUniforms(std::make_pair(reinterpret_cast<char*>(&ubo), static_cast<uint32_t>(sizeof(ubo))), 0, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
