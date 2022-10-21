#include <vector>
#include <string>
#include <variant>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <array>
#include <cstring>
#include <cmath>
#ifdef __DEBUG
    #include <iostream>
#endif
#include <vulkan/vulkan.h>

#include "trs/Points.h"
#include "trs/Vector.h"
#include "trs/Matrix.h"
#include "trs/Quaternion.h"

#include "deng/Api.h"
#include "deng/BaseTypes.h"
#include "deng/Window.h"
#include "deng/ErrorDefinitions.h"
#include "deng/ShaderDefinitions.h"
#include "deng/Renderer.h"
#include "deng/ModelUniforms.h"
#include "deng/Entity.h"
#include "deng/ScriptableEntity.h"
#include "deng/Registry.h"
#include "deng/Camera3D.h"
#include "deng/EditorCamera.h"
#include "deng/GridGenerator.h"

#if defined(USE_VULKAN)
    #include "deng/VulkanRenderer.h"
#elif defined(USE_OPENGL)
    #include "deng/OpenGLRenderer.h"
#endif

#define WIDTH      1280
#define HEIGHT     720

namespace Executable {

    class CameraController : public DENG::ScriptComponent {
        private:
            DENG::Window* m_window;

        public:
            CameraController(DENG::Entity* _cam, DENG::Window* _win) :
                ScriptComponent(_cam),
                m_window(_win) {}

            void OnAttach() {
                ((DENG::EditorCamera*)m_scriptable_entity)->EnableCamera();
            }

            void OnUpdate() {
                if (m_window->IsKeyPressed(NEKO_MOUSE_BTN_2)) {
                    m_window->ChangeVCMode(true, { m_window->GetSize().x / 2, m_window->GetSize().y / 2 });
                    ((DENG::EditorCamera*)m_scriptable_entity)->Rotate({ m_window->GetMouseDelta().x, m_window->GetMouseDelta().y});
                }
                else {
                    m_window->ChangeVCMode(false, { m_window->GetSize().x / 2, m_window->GetSize().y / 2 });
                    if (m_window->IsKeyPressed(NEKO_MOUSE_SCROLL_DOWN)) {
                        ((DENG::EditorCamera*)m_scriptable_entity)->ZoomOut();
                    }
                    if (m_window->IsKeyPressed(NEKO_MOUSE_SCROLL_UP)) {
                        ((DENG::EditorCamera*)m_scriptable_entity)->ZoomIn();
                    }
                }
            }

            void OnDestroy() {}
    };

    class Grid3DTest {
        private:
            DENG::Window &m_window;
            DENG::Renderer &m_renderer;
            DENG::RendererConfig& m_config;

            DENG::EditorCamera m_editor_camera;
            DENG::GridGenerator m_grid;
            const float m_key_interval = 800; // ms
            bool m_use_camera = false;

            std::chrono::time_point<std::chrono::system_clock> m_cur_time = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock> m_beg_time = std::chrono::system_clock::now();

        public:
            Grid3DTest(DENG::Window &_win, DENG::Renderer &_rend, DENG::RendererConfig &_conf) :
                m_window(_win),
                m_renderer(_rend),
                m_config(_conf),
                m_editor_camera(NULL, "Editor camera", m_renderer),
                m_grid(NULL, "__grid__", m_renderer, 50.0f, 50.0f, 0.5f, 0.5f, m_editor_camera.GetId())
            {
                m_editor_camera.BindScript<CameraController>(&m_window);
                DENG::Registry* reg = DENG::Registry::GetInstance();
                reg->AttachEntities();
                m_renderer.LoadShaders();
            }

            void Run() {
                DENG::Registry* reg = DENG::Registry::GetInstance();

                while(m_window.IsRunning()) {
                    m_renderer.ClearFrame();
                    m_cur_time = std::chrono::system_clock::now();
                    std::chrono::duration<float, std::milli> delta_time = m_cur_time - m_beg_time;

                    if(delta_time.count() >= m_key_interval) {
                        if(m_window.IsKeyPressed(NEKO_KEY_Q))
                            break;

                        if(m_window.IsKeyPressed(NEKO_KEY_F)) {
                            if(m_window.GetSizeHints() == NEKO_HINT_FULL_SCREEN)
                                m_window.ChangeSizeHints(NEKO_HINT_RESIZEABLE);
                            else m_window.ChangeSizeHints(NEKO_HINT_FULL_SCREEN);
                            m_beg_time = std::chrono::system_clock::now();
                        }
                    }

                    reg->Update();
                    
                    m_renderer.RenderFrame();
                    m_window.SwapBuffers();
                    m_window.Update();
                    m_config.canvas_size = {
                        static_cast<uint32_t>(m_window.GetSize().x),
                        static_cast<uint32_t>(m_window.GetSize().y)
                    };
                }
            }
    };
}

