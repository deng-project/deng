// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ViewModelsApp.cpp - View models application implementation
// author: Karl-Mihkel Ott

#include <Executables/ViewModelsApp.h>

namespace Executable {

    ModelLoaderApp::ModelLoaderApp(DENG::Window &_win, DENG::Renderer &_rend) : 
        m_window(_win), 
        m_renderer(_rend), 
        m_editor_camera(m_renderer, m_window, m_editor_camera_conf, "Editor camera"),
        m_grid(50.0f, 50.0f, 0.5f, 0.5f, m_editor_camera.GetUboOffset())
    {
        m_editor_camera.DisableCamera();
        m_grid.Attach(m_renderer);
        m_renderer.GetShaderModules()[m_grid.GetShaderId()].enable_custom_viewport = true;
        m_renderer.LoadShaders();
    }


    void ModelLoaderApp::Run() {
        const DENG::Viewport vp = { 50, 50, 720, 480 };
        while(m_window.IsRunning()) {
            m_renderer.ClearFrame();

            m_renderer.GetShaderModules()[m_grid.GetShaderId()].viewport = vp;

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
