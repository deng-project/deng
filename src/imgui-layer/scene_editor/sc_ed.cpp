/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: sc_ed.cpp - main scene editor class implementation file
/// author: Karl-Mihkel Ott


#define __SC_ED_CPP
#include <imgui-layer/scene_editor/sc_ed.h>


namespace dengEditor {

    SceneEditor3D::SceneEditor3D(deng_RendererHintBits rend_hints, deng_WindowHint win_hints) : 
        m_win(WIDTH, HEIGHT, DENG_WINDOW_HINT_API_VULKAN, "DENG - Scene editor"),
        m_cam(DENG_CAMERA_TYPE_EDITOR, static_cast<deng_vec_t>(dengMath::Conversion::degToRad(65.0)),
            {-0.1f, -25.0f}, {0.7f, 0.7f, 0.7f}, {0.3f, 0.3f}, false, &m_win),
        m_rend(rend_hints, dengMath::vec4<deng_vec_t>{0.0f, 0.0f, 0.0f, 1.0f})
    {
        deng::Camera3DBindings bindings = {};
        bindings.mov_w = neko_CreateInputMask(1, NEKO_MOUSE_SCROLL_UP);
        bindings.mov_nw = neko_CreateInputMask(1, NEKO_MOUSE_SCROLL_UP);
        bindings.ch_vcp = neko_CreateInputMask(1, NEKO_MOUSE_BTN_2);
        m_cam.setBindings(bindings);

        // Setup the renderer
        m_rend.setup(m_cam, m_win);
    }


    void SceneEditor3D::run() {
        m_ui_man = std::make_unique<deng::UIManager>(m_rend);

        // Main loop
        while(deng_IsRunning()) {
            // Start measuring time for FPS counter 
            auto t1 = std::chrono::high_resolution_clock::now();

            // Update the UI manager io data
            m_ui_man->updateIO(m_win);

            ImGui::NewFrame();
                SceneEditor3DEntityPanel::spawnEntityPanel(m_rend);
            ImGui::EndFrame();
            ImGui::Render();

            ImDrawData *p_draw_data = ImGui::GetDrawData();
            m_ui_man->render(p_draw_data, m_win);
            m_rend.update();

            auto t2 = std::chrono::high_resolution_clock::now();
            m_ui_man->setTime(t1, t2);
        }
    }
}


int main() {
    dengEditor::SceneEditor3D sc(DENG_RENDERER_HINT_MSAA_2, DENG_WINDOW_HINT_API_VULKAN);
    sc.run();
    return 0;
}
