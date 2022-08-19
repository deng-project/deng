// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ViewModelsApp.h - API neutral DAS model loading header
// author: Karl-Mihkel Ott

#ifndef GAME_EDITOR_H
#define GAME_EDITOR_H

#if defined(_WIN32) && !defined(__DEBUG)
    #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

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

#ifdef __DEBUG
    #include <iostream>
#endif

// libdas includes
#include "trs/Points.h"
#include "trs/Vector.h"
#include "trs/Matrix.h"
#include "trs/Quaternion.h"

#include "mar/AsciiStreamReader.h"
#include "mar/AsciiLineReader.h"

#include "das/Api.h"
#include "das/ErrorHandlers.h"
#include "das/DasStructures.h"
#include "das/DasReaderCore.h"
#include "das/DasParser.h"
#include "das/Hash.h"

#include "deng/Api.h"
#include "deng/BaseTypes.h"
#include "deng/ErrorDefinitions.h"
#include "deng/ShaderDefinitions.h"
#include "deng/Window.h"
#include "deng/Renderer.h"
#include "deng/ModelUniforms.h"
#include "deng/ModelShaderGenerator.h"
#include "deng/ModelShaderManager.h"
#include "deng/AnimationSampler.h"
#include "deng/MeshLoader.h"
#include "deng/SkeletonLoader.h"
#include "deng/NodeLoader.h"
#include "deng/SceneLoader.h"
#include "deng/ModelLoader.h"
#include "deng/Camera3D.h"
#include "deng/EditorCamera.h"
#include "deng/FirstPersonCamera.h"
#include "deng/ImGuiLayer.h"
#include "deng/PythonScriptExecutor.h"
#include "deng/GPUMemoryManager.h"
#include "deng/GridGenerator.h"
#include "deng/FilePicker.h"
#include "deng/RenderState.h"

// backend specific includes
#include "deng/OpenGLRenderer.h"
#include "deng/VulkanRenderer.h"

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
        DENG::EditorCamera *camera = nullptr;
        ImVec2 uv = ImVec2();
        ImTextureID viewport = nullptr;
        DENG::FilePicker fp;
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

            neko_HidEvent m_zoom_in[8] = { 
                NEKO_MOUSE_SCROLL_UP, 
                NEKO_HID_UNKNOWN, 
                NEKO_HID_UNKNOWN, 
                NEKO_HID_UNKNOWN, 
                NEKO_HID_UNKNOWN, 
                NEKO_HID_UNKNOWN, 
                NEKO_HID_UNKNOWN, 
                NEKO_HID_UNKNOWN 
            };

            neko_HidEvent m_zoom_out[8] = { 
                NEKO_MOUSE_SCROLL_DOWN, 
                NEKO_HID_UNKNOWN, 
                NEKO_HID_UNKNOWN, 
                NEKO_HID_UNKNOWN, 
                NEKO_HID_UNKNOWN, 
                NEKO_HID_UNKNOWN, 
                NEKO_HID_UNKNOWN, 
                NEKO_HID_UNKNOWN 
            };

            neko_HidEvent m_rotate_toggle[8] = { 
                NEKO_MOUSE_BTN_2,
                NEKO_HID_UNKNOWN,
                NEKO_HID_UNKNOWN,
                NEKO_HID_UNKNOWN,
                NEKO_HID_UNKNOWN,
                NEKO_HID_UNKNOWN,
                NEKO_HID_UNKNOWN,
                NEKO_HID_UNKNOWN
            };

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
