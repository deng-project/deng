// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ViewModelsApp.h - API neutral DAS model loading header
// author: Karl-Mihkel Ott

#ifndef GAME_EDITOR_H
#define GAME_EDITOR_H

#if defined(_WIN32) && !defined(_DEBUG)
    #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

#define DENG_EDITOR
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <fstream>
#include <cstring>
#include <cmath>
#include <unordered_map>
#include <chrono>
#include <variant>
#include <any>
#include <cmath>

#ifdef _DEBUG
    #include <iostream>
#endif

// libdas includes
#include <libdas/include/Points.h>
#include <libdas/include/Vector.h>
#include <libdas/include/Matrix.h>
#include <libdas/include/Quaternion.h>

#include <libdas/include/Api.h>
#include <libdas/include/ErrorHandlers.h>
#include <libdas/include/AsciiStreamReader.h>
#include <libdas/include/AsciiLineReader.h>
#include <libdas/include/DasStructures.h>
#include <libdas/include/DasReaderCore.h>
#include <libdas/include/DasParser.h>
#include <libdas/include/Hash.h>

#include <Api.h>
#include <BaseTypes.h>
#include <ErrorDefinitions.h>
#include <ShaderDefinitions.h>
#include <Window.h>
#include <Renderer.h>
#include <ModelUniforms.h>
#include <ModelShaderGenerator.h>
#include <ModelShaderManager.h>
#include <AnimationSampler.h>
#include <MeshLoader.h>
#include <SkeletonLoader.h>
#include <NodeLoader.h>
#include <SceneLoader.h>
#include <ModelLoader.h>
#include <CameraTransformManager.h>
#include <Camera3D.h>
#include <EditorCamera.h>
#include <FirstPersonCamera.h>
#include <ImGuiLayer.h>
#include <PythonScriptExecutor.h>
#include <GPUMemoryManager.h>
#include <GridGenerator.h>
#include <FilePicker.h>
#include <RenderState.h>

// backend specific includes
#include <OpenGLRenderer.h>
#include <VulkanRenderer.h>

#define USE_VULKAN  1
#define USE_OPENGL  2

#define WIDTH   1880
#define HEIGHT  1080
#define FILEPICKER_WIDTH    720
#define FILEPICKER_HEIGHT   480
#define VIEWPORT_NAME "GameViewport"


namespace Executable {

    struct EditorInspectorData {
        enum {
            ENTITY_TYPE_ANIMATION,
            ENTITY_TYPE_SCENE,
            ENTITY_TYPE_NODE,
            ENTITY_TYPE_MESH,
            ENTITY_TYPE_NONE
        } entity_type = ENTITY_TYPE_NONE;

        // ENTITY_TYPE_ANIMATION: DENG::Animation* 
        // ENTITY_TYPE_SCENE: DENG::SceneLoader*
        // ENTITY_TYPE_NODE: DENG::NodeLoader*
        // ENTITY_TYPE_MESH: DENG::MeshLoader*
        // ENTITY_TYPE_NONE: nullptr \-_-/
        void *entity = nullptr;
        uint32_t model_id = 0;
    };

    struct EditorGuiData {
        EditorInspectorData inspector;

        std::vector<DENG::ModelLoader> model_loaders;
        DENG::Window *win = nullptr;
        DENG::Renderer *rend = nullptr;
        DENG::ImGuiLayer *imgui = nullptr;
        ImVec2 uv = ImVec2();
        ImTextureID viewport = nullptr;
        DENG::FilePicker fp;
        uint32_t camera_offset = 0;
        bool once = true;
        bool quit = false;
    };


    struct ImGuiCallback {
        // menubar callbacks
        static void _NewProject(EditorGuiData *_data);
        static void _OpenFile(EditorGuiData *_data);

        // dock window callbacks
        static void _CreateInspector(EditorGuiData *_data);
        static void _CreateRecursiveNodeTree(uint32_t _model_id, DENG::NodeLoader &_node, EditorGuiData *_data);
        static void _CreateHierarchy(EditorGuiData *_data);
        static void _ShowCoreUI(void *_data);
    };

    class ModelLoaderApp {
        private:
            const std::string m_viewport_framebuffer_name = VIEWPORT_NAME;
            EditorGuiData m_imgui_data;
            DENG::Window &m_window;
            DENG::Renderer &m_renderer;

            neko_HidEvent m_zoom_in[8] = { NEKO_MOUSE_SCROLL_UP, 0, 0, 0, 0, 0, 0, 0 };
            neko_HidEvent m_zoom_out[8] = { NEKO_MOUSE_SCROLL_DOWN, 0, 0, 0, 0, 0, 0, 0 };
            neko_HidEvent m_rotate_toggle[8] = { NEKO_MOUSE_BTN_2, 0, 0, 0, 0, 0, 0, 0 };

            const DENG::EditorCameraConfiguration m_editor_camera_conf = {
                DENG::Window::CreateInputMask(m_zoom_in),
                DENG::Window::CreateInputMask(m_zoom_out),
                DENG::Window::CreateInputMask(m_rotate_toggle)
            };

            DENG::EditorCamera m_editor_camera;
            DENG::ImGuiLayer m_imgui;
            DENG::GridGenerator m_grid;

            std::chrono::time_point<std::chrono::system_clock> m_cur_time = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock> m_beg_time = std::chrono::system_clock::now();
            const float m_key_interval = 1000; // ms
            bool m_use_camera = false;

        public:
            ModelLoaderApp(DENG::Window &_win, DENG::Renderer &_rend);
            void Run();
    };
}


#endif
