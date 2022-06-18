#include <cstdint>
#include <string>
#include <vector>
#include <variant>
#include <array>
#include <unordered_map>
#include <cstring>
#include <cmath>

#include <libdas/include/Points.h>
#include <libdas/include/Vector.h>

#include <Api.h>
#include <ErrorDefinitions.h>
#include <ShaderDefinitions.h>
#include <Window.h>
#include <Renderer.h>
#include <OpenGLRenderer.h>
#include <VulkanRenderer.h>
#include <GPUMemoryManager.h>

#define TEXTURE_FILE_NAME    "TriangleAppData/felix_texture.png"
#define WIDTH   1280
#define HEIGHT  720

#define FB_WIDTH    512
#define FB_HEIGHT   512


#define VERTEX_SHADER  "#version 450\n"\
                       "#extension GL_ARB_separate_shader_objects : enable\n"\
                       "layout(location = 0) in vec3 i_pos;\n"\
                       "layout(location = 1) in vec2 i_uv;\n"\
                       "\n"\
                       "layout(location = 0) out vec2 o_uv;\n"\
                       "void main() {\n"\
                       "    gl_Position = vec4(i_pos, 1.0f);\n"\
                       "    o_uv = i_uv;\n"\
                       "}\n"

#define FRAGMENT_SHADER  "#version 450\n"\
                         "#extension GL_ARB_separate_shader_objects : enable\n"\
                         "layout(location = 0) in vec2 i_uv;\n"\
                         "layout(binding = 0) uniform sampler2D smp;\n"\
                         "\n"\
                         "layout(location = 0) out vec4 o_color;\n"\
                         "\n"\
                         "void main() {\n"\
                         "    o_color = texture(smp, i_uv);\n"\
                         "}\n"

const static float verts[] = {
    -1.0f, 1.0f, 0.0f,
    0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f,
    0.0f, 1.0f
};

const static uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

int main() {
    DENG::Window win = DENG::Window(WIDTH, HEIGHT, NEKO_HINT_API_VULKAN | NEKO_HINT_RESIZEABLE, "Vulkan multiple framebuffers");
    DENG::RendererConfig conf = {};
    DENG::VulkanRenderer rend(win, conf);
    DENG::GPUMemoryManager *mem_manager = DENG::GPUMemoryManager::GetInstance();
    const uint32_t vert_offset = mem_manager->RequestMainMemoryLocationF(static_cast<uint32_t>(sizeof(float)), static_cast<uint32_t>(sizeof(verts)));
    const uint32_t idx_offset = mem_manager->RequestMainMemoryLocationF(static_cast<uint32_t>(sizeof(float)), static_cast<uint32_t>(sizeof(indices)));
    mem_manager->RequestUniformMemoryLocationF(rend, 1);

    // push indices and vertices
    rend.UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<const char*>(verts), static_cast<uint32_t>(sizeof(verts))), vert_offset);
    rend.UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<const char*>(indices), static_cast<uint32_t>(sizeof(indices))), idx_offset);

    {
        // create a new framebuffer image to use
        DENG::FramebufferDrawData fb_draw = {};
        fb_draw.image_name = "Framebuffer#1";

        // write the shader specification for Framebuffer#1
        fb_draw.shaders.emplace_back();
        fb_draw.shaders.back().attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
        fb_draw.shaders.back().attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC2_FLOAT);
        fb_draw.shaders.back().attribute_strides.push_back(sizeof(float) * 5);
        fb_draw.shaders.back().attribute_strides.push_back(sizeof(float) * 5);
        fb_draw.shaders.back().vertex_shader_src = VERTEX_SHADER;
        fb_draw.shaders.back().fragment_shader_src = FRAGMENT_SHADER;
        fb_draw.shaders.back().ubo_data_layouts.push_back({ 
            { 0, 0, 0 }, 
            DENG::UNIFORM_DATA_TYPE_IMAGE_SAMPLER, 
            SHADER_STAGE_FRAGMENT,
            DENG::UNIFORM_USAGE_PER_SHADER
        });

        fb_draw.shaders.back().enable_blend = true;
        fb_draw.shaders.back().enable_indexing = true;
        fb_draw.shaders.back().enable_texture_mapping = true;
        fb_draw.extent = { FB_WIDTH, FB_HEIGHT };

        // write mesh specification for Framebuffer#1
        fb_draw.meshes.emplace_back();
        fb_draw.meshes.back().name = "FramebufferMesh";
        fb_draw.meshes.back().shader_module_id = 0;
        fb_draw.meshes.back().commands.emplace_back();

        fb_draw.meshes.back().commands.back().indices_offset = idx_offset;
        fb_draw.meshes.back().commands.back().draw_count = 6;
        fb_draw.meshes.back().commands.back().texture_names.push_back("Framebuffer#1_Texture");
        fb_draw.meshes.back().commands.back().attribute_offsets.push_back(static_cast<std::size_t>(vert_offset));
        fb_draw.meshes.back().commands.back().attribute_offsets.push_back(static_cast<std::size_t>(vert_offset) + sizeof(float) * 3);
        rend.PushTextureFromFile("Framebuffer#1_Texture", TEXTURE_FILE_NAME);
        rend.PushFramebuffer(fb_draw);
    }

    uint32_t shader_id = 0;
    {
        const uint32_t mesh_id = rend.NewMeshReference();
        // write main swapchain shader specification
        DENG::ShaderModule module = {};
        module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
        module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC2_FLOAT);
        module.attribute_strides.push_back(sizeof(float) * 5);
        module.attribute_strides.push_back(sizeof(float) * 5);
        module.vertex_shader_src = VERTEX_SHADER;
        module.fragment_shader_src = FRAGMENT_SHADER;
        module.ubo_data_layouts.push_back({
            { 0, 0, 0 }, 
            DENG::UNIFORM_DATA_TYPE_IMAGE_SAMPLER, 
            SHADER_STAGE_FRAGMENT,
            DENG::UNIFORM_USAGE_PER_SHADER
        });
        module.enable_blend = true;
        module.enable_indexing = true;
        module.enable_custom_viewport = true;
        module.enable_texture_mapping = true;
        module.viewport = {
            (WIDTH - FB_WIDTH) / 2,
            (HEIGHT - FB_HEIGHT) / 2,
            FB_WIDTH,
            FB_HEIGHT
        };
        shader_id = rend.PushShader(module);

        DENG::MeshReference &mesh = rend.GetMeshes()[mesh_id];
        mesh.name = "SwapchainMesh";
        mesh.shader_module_id = 0;
        mesh.commands.emplace_back();
        mesh.commands.back().indices_offset = idx_offset;
        mesh.commands.back().draw_count = 6;
        mesh.commands.back().attribute_offsets.push_back(static_cast<std::size_t>(vert_offset));
        mesh.commands.back().attribute_offsets.push_back(static_cast<std::size_t>(vert_offset) + sizeof(float) * 3);
        mesh.commands.back().texture_names.push_back("Framebuffer#1");
    }

    rend.LoadShaders();

    // run loop
    while(win.IsRunning()) {
        rend.ClearFrame();
        DENG::ShaderModule &module = rend.GetShaderModules()[shader_id];
        module.viewport = {
            (WIDTH - FB_WIDTH) / 2,
            (HEIGHT - FB_HEIGHT) / 2,
            FB_WIDTH,
            FB_HEIGHT
        };

        rend.RenderFrame();
        win.Update();
    }

    return 0;
}
