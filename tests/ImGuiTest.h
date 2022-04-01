// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ImGuiTest.h - API neutral ImGuiApp implementation
// author: Karl-Mihkel Ott

#ifndef IMGUI_TEST_H
#define IMGUI_TEST_H

#if defined(_WIN32) && !defined(_DEBUG)
    #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

#include <string>
#include <chrono>
#include <vector>
#include <cstring>
#include <variant>
#include <unordered_map>

#ifdef _DEBUG
    #include <iostream>
#endif

#include <libdas/include/Points.h>
#include <libdas/include/Vector.h>
#include <libdas/include/Matrix.h>

#include <Api.h>
#include <ErrorDefinitions.h>
#include <BaseTypes.h>
#include <Window.h>
#include <ShaderDefinitions.h>
#include <Renderer.h>

#if defined(USE_OPENGL)
    #include <OpenGLRenderer.h>
#elif defined(USE_VULKAN)
    #include <VulkanRenderer.h>
#endif

#include <ImGuiLayer.h>


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
            m_imgui.Attach(m_window, m_renderer, 0, ImGuiApp::_Draw, nullptr);
            m_renderer.LoadShaders();
        }

        void Run() {
            while(m_window.IsRunning()) {
                m_renderer.ClearFrame();
                m_imgui.Update(0, 0);
                m_renderer.RenderFrame();
                m_window.Update();
            }
        }

};

#endif
