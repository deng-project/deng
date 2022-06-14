// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ViewModelsApp.cpp - View models application implementation
// author: Karl-Mihkel Ott

#include <Executables/ViewModelsApp.h>

namespace Executable {

    /***********************/
    /***** ImGuiCaller *****/
    /***********************/

    void ImGuiCaller::ShowMenuBar(ImGuiData* _p_data) {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open")) {
                    std::string fname = "";
                    if (fname != "") {
                        _p_data->p_app->PushModelLoader(fname);
                        _p_data->p_model_loaders = &_p_data->p_app->GetModelLoaders();
                        _p_data->reload_shaders = true;
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }


    void ImGuiCaller::MainImGuiContext(ImGuiData *_p_data) {
        Libdas::Point2D<int32_t> size = _p_data->p_win->GetSize();
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(size.x), static_cast<float>(size.y)), ImGuiCond_Always);
        ImGui::Begin("Scene editor", &_p_data->is_scene_editor, IMGUI_CONTEXT_WINDOW_FLAGS);
            ImGuiCaller::ShowMenuBar(_p_data);
        ImGui::End();
    }


    void ImGuiCaller::Callback(void *_data) {
        ImGuiData* p_data = reinterpret_cast<ImGuiData*>(_data);
        p_data->max_id = 1;

        ImGuiCaller::MainImGuiContext(p_data);
        //ImGuiCaller::ObjectManager(p_data);
    }


    /**************************/
    /***** ModelLoaderApp *****/
    /**************************/

    ModelLoaderApp::ModelLoaderApp(DENG::Window &_win, DENG::Renderer &_rend) : 
        m_window(_win), 
        m_renderer(_rend),
        m_editor_camera(m_renderer, m_window, m_editor_camera_conf, "Editor camera"),
        m_imgui_user_data(m_use_camera)
    {
        m_imgui_user_data.enabled_texture_count = 0;
        m_imgui_user_data.p_camera = &m_editor_camera;
        m_imgui_user_data.p_app = this;
        m_imgui_user_data.p_model_loaders = &m_loaders;
        m_imgui_user_data.p_win = &m_window;
        m_imgui.Attach(m_window, m_renderer, ImGuiCaller::Callback, &m_imgui_user_data);
        m_renderer.LoadShaders();
    }


    void ModelLoaderApp::PushModelLoader(const std::string &_fname) {
        m_loaders.emplace_back(_fname, m_renderer, m_editor_camera.GetUboOffset());
    }


    void ModelLoaderApp::Run() {
        while(m_window.IsRunning()) {
            m_renderer.ClearFrame();

            m_cur_time = std::chrono::system_clock::now();
            std::chrono::duration<float, std::milli> delta_time = m_cur_time - m_beg_time;

            if(delta_time.count() >= m_key_interval) {
                if(m_window.IsKeyPressed(NEKO_KEY_Q))
                    break;

                if(m_window.IsKeyPressed(NEKO_KEY_F)) {
                    m_window.ChangeSizeHints(NEKO_HINT_FULL_SCREEN);
                    m_beg_time = std::chrono::system_clock::now();
                }


                // toggle camera mode
                if(m_window.IsKeyPressed(NEKO_KEY_ESCAPE)) {
                    m_use_camera = !m_use_camera;

                    if(!m_use_camera) {
                        m_editor_camera.DisableCamera();
                        std::cout << "Camera is now disabled" << std::endl;
                    }
                    else {
                        m_editor_camera.EnableCamera();
                        std::cout << "Camera is now enabled" << std::endl;
                    }

                    m_beg_time = std::chrono::system_clock::now();
                }
            }

            DENG::ModelCameraUbo ubo;
            m_editor_camera.Update();
            m_imgui.Update();

            // foreach ModelLoader instance update it
            for (auto it = m_loaders.begin(); it != m_loaders.end(); it++)
                it->Update();

            // check if shaders should be reloaded
            if (m_imgui_user_data.reload_shaders) {
                DENG::GPUMemoryManager* mem_man = DENG::GPUMemoryManager::GetInstance();
                m_renderer.LoadShaders(mem_man->GetMaxUniformOffset());
                m_imgui_user_data.reload_shaders = false;
            }

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
