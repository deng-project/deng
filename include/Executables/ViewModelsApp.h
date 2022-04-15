// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelLoader.h - API neutral DAS model loading implementation
// author: Karl-Mihkel Ott

#ifndef MODEL_LOADER_APP_H
#define MODEL_LOADER_APP_H

#if defined(_WIN32) && !defined(_DEBUG)
    #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

#include <string>
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

#include <Api.h>
#include <BaseTypes.h>
#include <ErrorDefinitions.h>
#include <ShaderDefinitions.h>
#include <Window.h>
#include <Renderer.h>
#include <ModelUniforms.h>
#include <ModelShaderManager.h>
#include <AnimationSampler.h>
#include <MeshLoader.h>
#include <ModelLoader.h>
#include <CameraTransformManager.h>
#include <Camera3D.h>
#include <EditorCamera.h>
#include <FirstPersonCamera.h>
#include <ImGuiLayer.h>
#include <PythonScriptExecutor.h>

// backend specific includes
#include <OpenGLRenderer.h>
#include <VulkanRenderer.h>

#define USE_VULKAN  1
#define USE_OPENGL  2

#define WIDTH   1280
#define HEIGHT  720


namespace Executable {

    struct ImGuiData {
        ImGuiData(bool &_use_camera) : use_camera(_use_camera) {}

        bool &use_camera;
        bool is_object_manager = true;
        std::vector<DENG::ModelLoader*> model_loaders;
    };

    class ModelLoaderApp {
        private: 
            DENG::ModelLoader m_loader;
            DENG::Window &m_window;
            DENG::Renderer &m_renderer;

            // implement three different camera types to use
            const DENG::FirstPersonCameraConfiguration m_first_person_camera_conf = {
                neko_CreateInputMask(1, NEKO_KEY_W),
                neko_CreateInputMask(1, NEKO_KEY_S),
                neko_CreateInputMask(1, NEKO_KEY_SPACE),
                neko_CreateInputMask(1, NEKO_KEY_LEFT_SHIFT),
                neko_CreateInputMask(1, NEKO_KEY_D),
                neko_CreateInputMask(1, NEKO_KEY_A)
            };
            
            const DENG::EditorCameraConfiguration m_editor_camera_conf = {
                neko_CreateInputMask(1, NEKO_MOUSE_SCROLL_UP),
                neko_CreateInputMask(1, NEKO_MOUSE_SCROLL_DOWN),
                neko_CreateInputMask(1, NEKO_MOUSE_BTN_2)
            };

            DENG::EditorCamera m_editor_camera;
            DENG::ImGuiLayer m_imgui;

            std::string m_file_name;
            const float m_key_interval = 1000;    // ms
            std::chrono::time_point<std::chrono::system_clock> m_beg_time = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock> m_cur_time = std::chrono::system_clock::now();


            // 0 - first person camera / 1 - editor camera
            bool m_use_camera = false;

            ImGuiData m_imgui_user_data;

        private:
            static void _ImGuiCallback(void *_data);

        public:
            ModelLoaderApp(const std::string &_file_name, DENG::Window &_win, DENG::Renderer &_rend);
            void Run();
    };
}


#endif
