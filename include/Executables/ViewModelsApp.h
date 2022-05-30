// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ViewModelsApp.h - API neutral DAS model loading header
// author: Karl-Mihkel Ott

#ifndef MODEL_LOADER_APP_H
#define MODEL_LOADER_APP_H

#if defined(_WIN32) && !defined(_DEBUG)
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
#include <NativeGUICaller.h>
#include <Renderer.h>
#include <ModelUniforms.h>
#include <ModelShaderGenerator.h>
#include <ModelShaderManager.h>
#include <AnimationSampler.h>
#include <MeshLoader.h>
#include <SkeletonDataManager.h>
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

// backend specific includes
#include <OpenGLRenderer.h>
#include <VulkanRenderer.h>

#define USE_VULKAN  1
#define USE_OPENGL  2

#define WIDTH   1280
#define HEIGHT  720


namespace Executable {

    class ModelLoaderApp;

    struct ImGuiData {
        ImGuiData(bool &_use_camera) : use_camera(_use_camera) {}

        bool &use_camera;
        bool is_object_manager = true;
        bool is_texture_picker = false;
        bool reload_shaders = false;
        DENG::EditorCamera *p_camera;
        std::vector<DENG::ModelLoader> *p_model_loaders;
        ModelLoaderApp* p_app = nullptr;
        DENG::Window *p_win = nullptr;

        // for texture picker
        uint32_t enabled_texture_count = 0;
        std::vector<bool> texture_picker_data;

        uint32_t max_id = 1;
    };

    struct ImGuiCaller {
        static void ShowTransformationProperties(DENG::NodeLoader& _node);
        static void ShowTexturePicker(ImGuiData *_p_data, DENG::ModelLoader &_loader, DENG::MeshLoader &_mesh);
        static void RecursiveNodeIteration(ImGuiData *_p_data, DENG::ModelLoader &_loader, DENG::NodeLoader &_node);
        static void ShowMenuBar(ImGuiData *_p_data);

        // main ImGui callback method
        static void Callback(void *_data);
    };

    class ModelLoaderApp {
        private: 
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
            std::vector<DENG::ModelLoader> m_loaders;

            DENG::ImGuiLayer m_imgui;

            std::string m_file_name;
            const float m_key_interval = 1000;    // ms
            std::chrono::time_point<std::chrono::system_clock> m_beg_time = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock> m_cur_time = std::chrono::system_clock::now();


            // 0 - first person camera / 1 - editor camera
            bool m_use_camera = false;

            ImGuiData m_imgui_user_data;

        public:
            ModelLoaderApp(DENG::Window &_win, DENG::Renderer &_rend);
            void PushModelLoader(const std::string &_fname);
            void Run();

            inline std::vector<DENG::ModelLoader>& GetModelLoaders() {
                return m_loaders;
            }
            
    };
}


#endif
