// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: TriangleApp.h - API neutral triangle app implementation
// author: Karl-Mihkel Ott

#ifndef TRIANGLE_APP_H
#define TRIANGLE_APP_H

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

#include <libdas/include/Points.h>
#include <libdas/include/Vector.h>
#include <libdas/include/Matrix.h>

#include <Api.h>
#include <ErrorDefinitions.h>
#include <BaseTypes.h>
#include <Window.h>
#include <ShaderDefinitions.h>
#include <Renderer.h>

// backend specific includes
#ifdef USE_OPENGL
    #include <OpenGLRenderer.h>
#endif

#ifdef USE_VULKAN
    #include <VulkanRenderer.h>
#endif

#define WIDTH   1280
#define HEIGHT  720


const static float g_verts[] = {
    -0.5f, -0.5f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.5f, 0.0f,
    0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    1.0f, 1.0f
};


const static uint32_t g_indices[] = { 0, 1, 2 };

// App data definitions
#define VERTEX_SHADER   "TriangleAppData/VertexShader.glsl"
#define FRAGMENT_SHADER "TriangleAppData/FragmentShaderMapped.glsl"
#define TEXTURE_FILE    "TriangleAppData/felix_texture.png"

// Mesh and texture name definitions
#define MESH_NAME       "Generic triangle"
#define TEXTURE_NAME    "Felix Argyle texture"

class TriangleApp {
    private:
        DENG::ShaderModule m_module;
        DENG::UniformDataLayout m_sampler_layout;
        DENG::Window &m_window;
        DENG::Renderer &m_renderer;

        // mesh and texture references to use
        DENG::MeshReference m_mesh;
        const DENG::TextureReference m_tex = { TEXTURE_NAME, 0, 2, UINT32_MAX };

    public:
        TriangleApp(DENG::Window &_win, DENG::Renderer &_rend) : m_window(_win), m_renderer(_rend) {
            // setup shader module
            m_module.vertex_shader_file = VERTEX_SHADER;
            m_module.fragment_shader_file = FRAGMENT_SHADER;

            // specify vertex attributes
            m_module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC3_FLOAT);
            m_module.offsets.push_back(0);
            m_module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC2_FLOAT);
            m_module.offsets.push_back(sizeof(float) * 3);

            // setup sampler layout specification
            m_sampler_layout.binding = 0;
            m_sampler_layout.stage = SHADER_STAGE_FRAGMENT;
            m_sampler_layout.type = DENG::UNIFORM_DATA_TYPE_IMAGE_SAMPLER;

            // specify uniform sampler layout
            m_module.ubo_data_layouts.push_back(m_sampler_layout);
            m_module.load_shaders_from_file = true;


            m_renderer.ClearFrame();
            m_renderer.UpdateVertexBuffer(std::make_pair(reinterpret_cast<const char*>(g_verts), static_cast<uint32_t>(sizeof(g_verts))));
            m_renderer.UpdateIndexBuffer(std::make_pair(reinterpret_cast<const char*>(g_indices), static_cast<uint32_t>(sizeof(g_indices))));
            uint32_t shader_id = m_renderer.PushShader(m_module);
            uint32_t tex_id = m_renderer.PushTextureFromFile(m_tex, TEXTURE_FILE);

            // create the mesh object
            m_mesh.commands.emplace_back();
            m_mesh.commands.back().vertices_offset = 0;
            m_mesh.commands.back().indices_offset = 0;
            m_mesh.commands.back().indices_count = 3;
            m_mesh.commands.back().texture_id = tex_id;
            m_mesh.shader_module_id = shader_id;
            m_mesh.name = MESH_NAME;

            m_renderer.PushMeshReference(m_mesh);
            m_renderer.LoadShaders();
        }


        void Run() {
            while(m_window.IsRunning()) {
                m_renderer.ClearFrame();
                m_renderer.RenderFrame();
                m_window.Update();
            }
        }
};


#endif
