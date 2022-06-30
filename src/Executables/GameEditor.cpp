// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ViewModelsApp.cpp - View models application implementation
// author: Karl-Mihkel Ott

#include <Executables/GameEditor.h>

namespace Executable {

    /*************************/
    /***** ImGuiCallback *****/
    /*************************/
    //void ImGuiCallback::_NewProject(EditorGuiData *_data) {
    //}

    void ImGuiCallback::_OpenFile(EditorGuiData *_data) {
        _data->fp.SelectFile(".das", FILEPICKER_WIDTH, FILEPICKER_HEIGHT, "DENG: Open a 3D model");
        const std::string file = _data->fp.GetPickedFile();
        _data->imgui->SetContext(_data->imgui->GetContext());

        if(file != "") {
            _data->model_loaders.emplace_back(file, *_data->rend, _data->camera_offset, VIEWPORT_NAME);
            _data->rend->LoadShaders();
        }

        DENG::RenderState *rs = DENG::RenderState::GetInstance();
        if(rs->GetPrimary() == DENG::RENDERER_TYPE_OPENGL)
            _data->win->glMakeCurrent();
    }


    void ImGuiCallback::_ShowCoreUI(void *_gui) {
        EditorGuiData *gui_data = reinterpret_cast<EditorGuiData*>(_gui);

        // docking configuration
        {
            ImVec2 size = { static_cast<float>(gui_data->win->GetSize().x), static_cast<float>(gui_data->win->GetSize().y) };
            ImGui::SetNextWindowSize(size, ImGuiCond_Always);
            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
            ImGui::Begin("Editor context", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar);
                if(ImGui::BeginMenuBar()) {
                    if(ImGui::BeginMenu("File")) {
                        ImGui::MenuItem("New");     // TASK: create new project
                        if(ImGui::MenuItem("Open")) // TASK: open existing project
                            _OpenFile(gui_data);
                        ImGui::MenuItem("Import");  // TASK: import resources (das, gltf, obj, stl)
                        ImGui::MenuItem("Export");  // TASK: export resources (das)
                        ImGui::MenuItem("Save");    // TASK: save project
                        ImGui::MenuItem("Save as"); // TASK: save project as
                        ImGui::MenuItem("Quit");    // TASK: quit editor
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenuBar();
                }

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
        }

        ImGui::Begin("inspector");
            ImGui::Text("Inspector");
        ImGui::End();

        // viewport window
        ImGui::Begin("viewport");
            ImVec2 size = ImGui::GetContentRegionAvail();
            ImGui::Image(gui_data->viewport, size);
        ImGui::End();

        ImGui::Begin("hierarchy");
        ImGui::End();

        ImGui::Begin("assets");
            ImGui::Text("Assets");
        ImGui::End();
    }


    ModelLoaderApp::ModelLoaderApp(DENG::Window &_win, DENG::Renderer &_rend) : 
        m_window(_win), 
        m_renderer(_rend),
        m_editor_camera(m_renderer, m_window, m_editor_camera_conf, "Editor camera"),
        m_grid(50.0f, 50.0f, 0.5f, 0.5f, m_editor_camera.GetUboOffset(), VIEWPORT_NAME)
    {
        m_editor_camera.DisableCamera();
        m_imgui_data.viewport = (void*) reinterpret_cast<size_t>(m_viewport_framebuffer_name.c_str());
        m_imgui_data.rend = &m_renderer;
        m_imgui_data.win = &m_window;
        m_imgui_data.imgui = &m_imgui;
        m_imgui_data.camera_offset = m_editor_camera.GetUboOffset();

        // add #VIEWPORT_NAME framebuffer to renderer
        DENG::FramebufferDrawData fb;
        fb.image_name = VIEWPORT_NAME; 
        fb.extent = { WIDTH, HEIGHT };

        m_renderer.PushFramebuffer(fb);
        m_imgui.Attach(m_window, m_renderer, ImGuiCallback::_ShowCoreUI, &m_imgui_data);
        m_grid.Attach(m_renderer);
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

            for(auto it = m_imgui_data.model_loaders.begin(); it != m_imgui_data.model_loaders.end(); it++) {
                it->Update();
            }

            m_editor_camera.Update();
            m_grid.Update(m_renderer, { 0.0f, 0.5f, 0.0f, 1.0f });
            m_imgui.Update();
            m_renderer.RenderFrame();
            m_window.Update();
        }
    }
}

int main(int argv, char *argc[]) {
    // lol
    if(argv < 1)
        std::exit(1);

    DENG::Window::Initialise();

    // Prompt the user, what backend to use
    DENG::PythonScriptExecutor py(argc[0]);
    int32_t backend = py.ExecuteFunction<int32_t>("BackendChooser", "Prompt");

    DENG::RendererConfig conf = { false, { 0.0f, 0.0f, 0.0f, 0.0f } };
    if(backend == USE_VULKAN) {
        DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_VULKAN, "DENG: Untitled project (Vulkan)");
        DENG::Vulkan::Initialise();
        DENG::VulkanRenderer rend(win, conf);
        Executable::ModelLoaderApp app(win, rend);
        app.Run();
    } else if(backend == USE_OPENGL) {
        DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_OPENGL, "DENG: Untitled project (OpenGL)");
        DENG::OpenGL::Initialise(win);
        DENG::OpenGLRenderer rend(win, conf);
        Executable::ModelLoaderApp app(win, rend);
        app.Run();
    }
    
    DENG::Window::Deinitialise();
    return EXIT_SUCCESS;
}
