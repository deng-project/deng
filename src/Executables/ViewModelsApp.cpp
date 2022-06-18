// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ViewModelsApp.cpp - View models application implementation
// author: Karl-Mihkel Ott

#include <Executables/ViewModelsApp.h>

namespace Executable {

    /*************************/
    /***** ImGuiCallback *****/
    /*************************/
    void ImGuiCallback::_ShowCoreUI(void *_gui) {
        EditorGuiData *gui_data = reinterpret_cast<EditorGuiData*>(_gui);
        ImVec2 size = { static_cast<float>(gui_data->win->GetSize().x), static_cast<float>(gui_data->win->GetSize().y) };
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::Begin("Editor context", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar);
            ImGuiID id = ImGui::GetID("EditorDockspace");
            ImGui::DockSpace(id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
            if(gui_data->once) {
                ImGui::DockBuilderRemoveNode(id);
                ImGui::DockBuilderAddNode(id, ImGuiDockNodeFlags_None);

                ImGuiID inspector = ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.25f, nullptr, &id);
                ImGuiID viewport = ImGui::DockBuilderSplitNode(id, ImGuiDir_Left, 0.75f, nullptr, &id);
                ImGuiID hierarchy = ImGui::DockBuilderSplitNode(viewport, ImGuiDir_Left, 0.15f, nullptr, &viewport);
                ImGuiID assets = ImGui::DockBuilderSplitNode(viewport, ImGuiDir_Down, 0.25f, nullptr, &viewport);

                ImGui::DockBuilderDockWindow("inspector", inspector);
                ImGui::DockBuilderDockWindow("viewport", viewport);
                ImGui::DockBuilderDockWindow("hierarchy", hierarchy);
                ImGui::DockBuilderDockWindow("assets", assets);

                gui_data->once = false;
            }
        ImGui::End();

        ImGui::Begin("inspector");
            ImGui::Text("Inspector");
        ImGui::End();

        ImGui::Begin("viewport");
            ImGui::Text("Viewport");
        ImGui::End();

        ImGui::Begin("hierarchy");
            ImGui::Text("Hierarchy");
        ImGui::End();

        ImGui::Begin("assets");
            ImGui::Text("Assets");
        ImGui::End();
    }


    ModelLoaderApp::ModelLoaderApp(DENG::Window &_win, DENG::Renderer &_rend) : 
        m_window(_win), 
        m_renderer(_rend),
        m_editor_camera(m_renderer, m_window, m_editor_camera_conf, "Editor camera"),
        m_imgui(),
        m_grid(50.0f, 50.0f, 0.5f, 0.5f, m_editor_camera.GetUboOffset())
    {
        m_editor_camera.DisableCamera();
        m_imgui_data.model_loaders = &m_model_loaders;
        m_imgui_data.win = &m_window;
        m_imgui.Attach(m_window, m_renderer, ImGuiCallback::_ShowCoreUI, &m_imgui_data);
        m_grid.Attach(m_renderer);
        m_renderer.GetShaderModules()[m_grid.GetShaderId()].enable_custom_viewport = true;
        m_renderer.LoadShaders();
    }


    void ModelLoaderApp::Run() {
        while(m_window.IsRunning()) {
            m_renderer.ClearFrame();

            m_cur_time = std::chrono::system_clock::now();
            std::chrono::duration<float, std::milli> delta_time = m_cur_time - m_beg_time;

            // check for any input events
            if(delta_time.count() >= m_key_interval) {
                if(m_window.IsKeyPressed(NEKO_KEY_Q))
                    break;

                if(m_window.IsKeyPressed(NEKO_KEY_F)) {
                    if(m_window.GetHints() & NEKO_HINT_FULL_SCREEN)
                        m_window.ChangeSizeHints(NEKO_HINT_RESIZEABLE);
                    else m_window.ChangeSizeHints(NEKO_HINT_FULL_SCREEN);
                    m_beg_time = std::chrono::system_clock::now();
                }

                if(m_window.IsKeyPressed(NEKO_KEY_ESCAPE)) {
                    m_use_camera = !m_use_camera;
                    if(!m_use_camera) {
                        m_editor_camera.DisableCamera();
                        LOG("Camera is now disabled");
                    } else {
                        m_editor_camera.EnableCamera();
                        LOG("Camera is now enabled");
                    }
                    m_beg_time = std::chrono::system_clock::now();
                }
            }

            m_editor_camera.Update();
            m_grid.Update(m_renderer, { 0.0f, 0.5f, 0.0f, 1.0f });
            m_imgui.Update();
            m_renderer.GetShaderModules()[m_grid.GetShaderId()].viewport = m_imgui_data.viewport;
            m_renderer.RenderFrame();
            m_window.Update();
        }
    }
}

int main(int argv, char *argc[]) {
    // lol
    if(argv < 1)
        std::exit(1);

    // Prompt the user, what backend to use
    DENG::PythonScriptExecutor py(argc[0]);
    int32_t backend = py.ExecuteFunction<int32_t>("BackendChooser", "Prompt");

    DENG::RendererConfig conf = { false, { 0.0f, 0.0f, 0.0f, 0.0f } };
    if(backend == USE_VULKAN) {
        DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_VULKAN, "VulkanModelLoader");
        DENG::VulkanRenderer rend(win, conf);
        Executable::ModelLoaderApp app(win, rend);
        app.Run();
    } else if(backend == USE_OPENGL) {
        DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_OPENGL, "OpenGLModelLoader");
        win.glMakeCurrent();
        DENG::OpenGLRenderer rend(win, conf);
        Executable::ModelLoaderApp app(win, rend);
        app.Run();
    }
    
    return EXIT_SUCCESS;
}
