// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ImGuiTest.h - API neutral ImGuiApp implementation
// author: Karl-Mihkel Ott

#ifndef IMGUI_TEST_H
#define IMGUI_TEST_H

#if defined(_WIN32) && !defined(__DEBUG)
    #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

#include <string>
#include <chrono>
#include <vector>
#include <array>
#include <cstring>
#include <variant>
#include <memory>
#include <unordered_map>

#ifdef _DEBUG
    #include <iostream>
#endif

#include "trs/Points.h"
#include "trs/Vector.h"
#include "trs/Matrix.h"

#include "deng/Api.h"
#include "deng/ErrorDefinitions.h"
#include "deng/BaseTypes.h"
#include "deng/Window.h"
#include "deng/ShaderDefinitions.h"
#include "deng/Renderer.h"

#if defined(USE_OPENGL)
    #include "deng/OpenGLRenderer.h"
#elif defined(USE_VULKAN)
    #include "deng/VulkanRenderer.h"
#endif

#include "deng/ImGuiLayer.h"


class ImGuiApp {
    private:
        DENG::Renderer &m_renderer;
        DENG::Window &m_window;
        DENG::ImGuiLayer m_imgui;

    private:
        static void _Draw(void *_user_data) {
            if(!_user_data) 
                ImGui::ShowDemoWindow();
        }

    public:
        ImGuiApp(DENG::Window &_win, DENG::Renderer &_rend) : m_renderer(_rend), m_window(_win) {
            m_imgui.Attach(m_window, m_renderer, ImGuiApp::_Draw, nullptr);
            m_renderer.LoadShaders();
        }

        void Run() {
            while(m_window.IsRunning()) {
                m_renderer.ClearFrame();
                m_imgui.Update();
                m_renderer.RenderFrame();
                m_window.Update();
            }
        }

};

#endif
