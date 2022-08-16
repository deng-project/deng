#include <cstdint>
#include <string>
#include <vector>
#include <variant>
#include <array>
#include <unordered_map>
#include <cstring>
#include <cmath>

#include "trs/Points.h"
#include "trs/Vector.h"

#include "deng/Api.h"
#include "deng/ErrorDefinitions.h"
#include "deng/ShaderDefinitions.h"
#include "deng/Window.h"
#include "deng/Renderer.h"
#if defined(USE_OPENGL)
    #include "deng/OpenGLRenderer.h"
#elif defined(USE_VULKAN)
    #include "deng/VulkanRenderer.h"
#endif
#include "deng/GPUMemoryManager.h"

#define TEXTURE_FILE_NAME           "TriangleAppData/felix_texture.png"
#define WIDTH                       1280
#define HEIGHT                      720

#define FB_WIDTH                    512
#define FB_HEIGHT                   512


#define VERTEX_SHADER               "#version 450\n"\
                                    "#extension GL_ARB_separate_shader_objects : enable\n"\
                                    "layout(location = 0) in vec3 i_pos;\n"\
                                    "layout(location = 1) in vec2 i_uv;\n"\
                                    "\n"\
                                    "layout(location = 0) out vec2 o_uv;\n"\
                                    "void main() {\n"\
                                    "    gl_Position = vec4(i_pos, 1.0f);\n"\
                                    "    o_uv = i_uv;\n"\
                                    "}\n"

#define FRAGMENT_SHADER             "#version 450\n"\
                                    "#extension GL_ARB_separate_shader_objects : enable\n"\
                                    "layout(location = 0) in vec2 i_uv;\n"\
                                    "layout(binding = 0) uniform sampler2D smp;\n"\
                                    "\n"\
                                    "layout(location = 0) out vec4 o_color;\n"\
                                    "\n"\
                                    "void main() {\n"\
                                    "    o_color = texture(smp, i_uv);\n"\
                                    "}\n"

#define FRAGMENT_SHADER_MAIN        "#version 450\n"\
                                    "#extension GL_ARB_separate_shader_objects : enable\n"\
                                    "layout(location = 0) in vec2 i_uv;\n"\
                                    "layout(binding = 1) uniform sampler2D smp;\n"\
                                    "\n"\
                                    "layout(location = 0) out vec4 o_color;\n"\
                                    "\n"\
                                    "void main() {\n"\
                                    "    o_color = texture(smp, i_uv);\n"\
                                    "}\n"

#define FRAMEBUFFER_NAME            "Framebuffer#1"
#define FRAMEBUFFER_MESH_NAME       "Framebuffer#1_Mesh"
#define FRAMEBUFFER_TEXTURE_NAME    "Framebuffer#1_Texture"

namespace Executable {

    const static float g_verts[] = {
        -1.0f, 1.0f, 0.0f,
        0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,
        0.0f, 1.0f
    };

#if defined(USE_OPENGL)
    const static float g_fb_verts[] = {
        -1.0f, 1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        0.0f, 0.0f
    };
#elif defined(USE_VULKAN)
    const static float g_fb_verts[] = {
        -1.0f, 1.0f, 0.0f,
        0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,
        0.0f, 1.0f
    };
#endif

    const static uint32_t g_indices[] = { 0, 1, 2, 2, 3, 0 };


    class MultiFramebuffer {
        private:
            DENG::Window &m_window;
            DENG::Renderer &m_renderer;
            uint32_t m_vert_offset = 0;
            uint32_t m_fb_vert_offset = 0;
            uint32_t m_idx_offset = 0;
            uint32_t m_main_shader_id = 0;

        private:
            void _CreateCustomFramebuffer() {
                // create a new framebuffer image to use
                DENG::FramebufferDrawData fb_draw = {};
                fb_draw.extent = { FB_WIDTH, FB_HEIGHT };
                fb_draw.image_name = FRAMEBUFFER_NAME;

                // write the shader specification for Framebuffer#1
                fb_draw.shaders.emplace_back();
                fb_draw.shaders.back().first.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
                fb_draw.shaders.back().first.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC2_FLOAT);
                fb_draw.shaders.back().first.attribute_strides.push_back(sizeof(float) * 5);
                fb_draw.shaders.back().first.attribute_strides.push_back(sizeof(float) * 5);
                fb_draw.shaders.back().first.vertex_shader_src = VERTEX_SHADER;
                fb_draw.shaders.back().first.fragment_shader_src = FRAGMENT_SHADER;
                fb_draw.shaders.back().first.ubo_data_layouts.push_back({ 
                    { 0, 0, 0 }, 
                    DENG::UNIFORM_DATA_TYPE_IMAGE_SAMPLER, 
                    SHADER_STAGE_FRAGMENT,
                    DENG::UNIFORM_USAGE_PER_SHADER
                });

                fb_draw.shaders.back().first.enable_blend = true;
                fb_draw.shaders.back().first.enable_indexing = true;
                fb_draw.shaders.back().first.enable_texture_mapping = true;
                fb_draw.shaders.back().second = DENG::RESOURCE_ADDED;
                fb_draw.extent = { FB_WIDTH, FB_HEIGHT };

                // write mesh specification for Framebuffer#1
                fb_draw.meshes.emplace_back();
                fb_draw.meshes.back().first.name = FRAMEBUFFER_MESH_NAME;
                fb_draw.meshes.back().first.shader_module_id = 0;
                fb_draw.meshes.back().first.commands.emplace_back();

                fb_draw.meshes.back().first.commands.back().indices_offset = m_idx_offset;
                fb_draw.meshes.back().first.commands.back().draw_count = static_cast<uint32_t>(sizeof(g_indices) / sizeof(uint32_t));
                fb_draw.meshes.back().first.commands.back().texture_names.push_back("Framebuffer#1_Texture");
                fb_draw.meshes.back().first.commands.back().attribute_offsets.push_back(static_cast<std::size_t>(m_vert_offset));
                fb_draw.meshes.back().first.commands.back().attribute_offsets.push_back(static_cast<std::size_t>(m_vert_offset) + sizeof(float) * 3);
                fb_draw.meshes.back().second = DENG::RESOURCE_ADDED;
                m_renderer.PushTextureFromFile(FRAMEBUFFER_TEXTURE_NAME, TEXTURE_FILE_NAME);
                m_renderer.PushFramebuffer(fb_draw);
            }


            void _CreateMainFramebuffer() {
                const uint32_t mesh_id = m_renderer.NewMeshReference();
                // write main swapchain shader specification
                DENG::ShaderModule module = {};
                module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
                module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC2_FLOAT);
                module.attribute_strides.push_back(sizeof(float) * 5);
                module.attribute_strides.push_back(sizeof(float) * 5);
                module.vertex_shader_src = VERTEX_SHADER;
                module.fragment_shader_src = FRAGMENT_SHADER_MAIN;
                module.ubo_data_layouts.push_back({
                    { 1, 0, 0 }, 
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
                m_main_shader_id = m_renderer.PushShader(module);

                DENG::MeshReference &mesh = m_renderer.GetMesh(mesh_id);
                mesh.name = "SwapchainMesh";
                mesh.shader_module_id = 0;
                mesh.commands.emplace_back();
                mesh.commands.back().indices_offset = m_idx_offset;
                mesh.commands.back().draw_count = static_cast<uint32_t>(sizeof(g_indices) / sizeof(uint32_t));
                mesh.commands.back().attribute_offsets.push_back(static_cast<std::size_t>(m_fb_vert_offset));
                mesh.commands.back().attribute_offsets.push_back(static_cast<std::size_t>(m_fb_vert_offset) + sizeof(float) * 3);
                mesh.commands.back().texture_names.push_back(FRAMEBUFFER_NAME);
            }

        public:
            MultiFramebuffer(DENG::Window &_win, DENG::Renderer &_rend) :
                m_window(_win),
                m_renderer(_rend)
            {
                DENG::GPUMemoryManager *mem_manager = DENG::GPUMemoryManager::GetInstance();
                m_vert_offset = mem_manager->RequestMainMemoryLocationF(static_cast<uint32_t>(sizeof(float)), static_cast<uint32_t>(sizeof(g_verts)));
                m_fb_vert_offset = mem_manager->RequestMainMemoryLocationF(static_cast<uint32_t>(sizeof(float)), static_cast<uint32_t>(sizeof(g_fb_verts)));
                m_idx_offset = mem_manager->RequestMainMemoryLocationF(static_cast<uint32_t>(sizeof(uint32_t)), static_cast<uint32_t>(sizeof(g_indices)));
                mem_manager->RequestUniformMemoryLocationF(m_renderer, 1);

                m_renderer.UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<const char*>(g_verts), static_cast<uint32_t>(sizeof(g_verts))), m_vert_offset);
                m_renderer.UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<const char*>(g_fb_verts), static_cast<uint32_t>(sizeof(g_fb_verts))), m_fb_vert_offset);
                m_renderer.UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<const char*>(g_indices), static_cast<uint32_t>(sizeof(g_indices))), m_idx_offset);
                _CreateCustomFramebuffer();
                _CreateMainFramebuffer();
                m_renderer.LoadShaders();
            }


            ~MultiFramebuffer() {
                DENG::GPUMemoryManager::DeleteInstance();
            }


            void Run() {
                while(m_window.IsRunning()) {
                    m_renderer.ClearFrame();
                    DENG::ShaderModule &module = m_renderer.GetShaderModule(m_main_shader_id);
                    module.viewport = {
                        static_cast<uint32_t>((m_window.GetSize().x - FB_WIDTH) / 2),
                        static_cast<uint32_t>((m_window.GetSize().y - FB_HEIGHT) / 2),
                        FB_WIDTH,
                        FB_HEIGHT
                    };

                    m_renderer.RenderFrame();
                    m_window.Update();
                }
            }
    };
}
