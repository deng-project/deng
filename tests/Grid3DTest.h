#include <vector>
#include <string>
#include <variant>
#include <chrono>
#include <unordered_map>
#include <array>
#include <cstring>
#include <cmath>
#ifdef _DEBUG
    #include <iostream>
#endif

#include <libdas/include/Points.h>
#include <libdas/include/Vector.h>
#include <libdas/include/Matrix.h>

#include <Api.h>
#include <BaseTypes.h>
#include <Window.h>
#include <ErrorDefinitions.h>
#include <ShaderDefinitions.h>
#include <Renderer.h>
#include <ModelUniforms.h>
#include <Camera3D.h>
#include <EditorCamera.h>
#include <GridGenerator.h>

#if defined(USE_VULKAN)
    #include <VulkanRenderer.h>
#elif defined(USE_OPENGL)
    #include <OpenGLRenderer.h>
#endif

#define WIDTH      1770
#define HEIGHT     1024

namespace Executable {

    class Grid3DTest {
        private:
            DENG::Window &m_window;
            DENG::Renderer &m_renderer;

            neko_HidEvent m_zoom_in[8] = { NEKO_MOUSE_SCROLL_UP, 0, 0, 0, 0, 0, 0, 0 };
            neko_HidEvent m_zoom_out[8] = { NEKO_MOUSE_SCROLL_DOWN, 0, 0, 0, 0, 0, 0, 0 };
            neko_HidEvent m_rotate_toggle[8] = { NEKO_MOUSE_BTN_2, 0, 0, 0, 0, 0, 0, 0 };

            const DENG::EditorCameraConfiguration m_camera_conf = {
                DENG::Window::CreateInputMask(m_zoom_in),
                DENG::Window::CreateInputMask(m_zoom_out),
                DENG::Window::CreateInputMask(m_rotate_toggle)
            };

            DENG::EditorCamera m_editor_camera;
            DENG::GridGenerator m_grid;
            const float m_key_interval = 800; // ms
            bool m_use_camera = false;

            std::chrono::time_point<std::chrono::system_clock> m_cur_time = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock> m_beg_time = std::chrono::system_clock::now();

        public:
            Grid3DTest(DENG::Window &_win, DENG::Renderer &_rend) :
                m_window(_win),
                m_renderer(_rend),
                m_editor_camera(m_renderer, m_window, m_camera_conf, "Editor camera"),
                m_grid(50.0f, 50.0f, 0.5f, 0.5f, m_editor_camera.GetUboOffset())
            {
                m_editor_camera.DisableCamera();
                m_grid.Attach(m_renderer);
                m_renderer.LoadShaders();
            }

            void Run() {
                while(m_window.IsRunning()) {
                    m_renderer.ClearFrame();
                    m_cur_time = std::chrono::system_clock::now();
                    std::chrono::duration<float, std::milli> delta_time = m_cur_time - m_beg_time;

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
    };
}

