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

// backend specific includes
#ifdef USE_OPENGL
    #include <OpenGLRenderer.h>
#endif

#ifdef USE_VULKAN
    #include <VulkanRenderer.h>
#endif

#define WIDTH   1280
#define HEIGHT  720

#define CONFIG_FILE  "loader.conf"

class ModelLoaderApp {
    private: 
        DENG::ModelLoader m_loader;
        DENG::Window &m_window;
        DENG::Renderer &m_renderer;
        DENG::Camera3D m_camera;
        std::string m_file_name;
        const float m_key_interval =  100;    // ms
        std::chrono::time_point<std::chrono::system_clock> m_beg_time = std::chrono::system_clock::now();
        std::chrono::time_point<std::chrono::system_clock> m_cur_time = std::chrono::system_clock::now();
        bool m_is_camera_enabled = false;

    public:
        ModelLoaderApp(const std::string &_file_name, DENG::Window &_win, DENG::Renderer &_rend) : 
            m_loader(_file_name, _rend, 0, 0), 
            m_window(_win), 
            m_renderer(_rend),
            m_camera(m_window, "First person camera", 0, 0)
        {
            m_loader.Attach();
            m_renderer.LoadShaders();
        }

        void Run() {
            while(m_window.IsRunning()) {
                m_renderer.ClearFrame();

                m_cur_time = std::chrono::system_clock::now();
                std::chrono::duration<float, std::milli> delta_time = m_cur_time - m_beg_time;

                if(delta_time.count() >= m_key_interval && m_window.IsKeyPressed(NEKO_KEY_Q))
                    break;

                if(delta_time.count() >= m_key_interval && m_window.IsKeyPressed(NEKO_KEY_F)) {
                    m_window.ChangeSizeHints(NEKO_HINT_FULL_SCREEN);
                    m_beg_time = std::chrono::system_clock::now();
                }


                if(delta_time.count() >= m_key_interval && m_window.IsKeyPressed(NEKO_KEY_ESCAPE)) {
                    if(m_is_camera_enabled)
                        m_camera.DisableCamera();
                    else m_camera.EnableCamera();

                    m_is_camera_enabled = !m_is_camera_enabled;
                    m_beg_time = std::chrono::system_clock::now();
                }

                DENG::ModelCameraUbo ubo;
                m_camera.Update(m_renderer);
                m_loader.Update();
                m_renderer.RenderFrame();
                m_window.Update();
            }
        }
};


#endif
