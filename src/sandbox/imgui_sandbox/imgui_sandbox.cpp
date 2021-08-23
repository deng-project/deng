/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: imgui_sandbox.cpp - imgui layer testing application class implementation
/// author: Karl-Mihkel Ott

#define __IMGUI_SANDBOX_CPP
#include <../src/sandbox/imgui_sandbox/imgui_sandbox.h>

namespace deng {

    ImGUIApplication::ImGUIApplication() : 
        m_win(1280, 720, DENG_WINDOW_HINT_API_VULKAN | DENG_WINDOW_HINT_RESIZEABLE, "ImGUI demo"),
        m_cam(DENG_CAMERA_TYPE_EDITOR, static_cast<deng_vec_t>(dengMath::conversion::degToRad(65.0)), 
            {0.1f, -25.0f}, {0.7f, 0.7f, 0.7f}, {0.3, 0.3}, &m_win),
#ifdef __DEBUG
        m_rend(DENG_RENDERER_HINT_MSAA_2 | DENG_RENDERER_HINT_ENABLE_API_DEBUGGING, dengMath::vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f))
#else
        m_rend(DENG_RENDERER_HINT_MSAA_2, dengMath::vec4<deng_vec_t>(0.0f, 0.0f, 0.0f, 1.0f))
#endif
    { 
#ifdef __DEBUG
        LOG("DEBUGGING ENABLED");
#endif
        // Set bindings
        deng::Camera3DBindings bindings = {};
        bindings.mov_w = neko_CreateInputMask(1, NEKO_MOUSE_SCROLL_UP);
        bindings.mov_nw = neko_CreateInputMask(1, NEKO_MOUSE_SCROLL_DOWN);
        bindings.ch_vcp = neko_CreateInputMask(1, NEKO_MOUSE_BTN_2);
        m_cam.setBindings(bindings);

        // Load viking asset with its texture
        das_Texture viking_tex = {};
        das_LoadTexture(&viking_tex, "textures/viking_room.tga");
        
        das_Asset viking = {};
        das_LoadAsset(&viking, DAS_ASSET_MODE_UNDEFINED, das_ObjColorData{1.0f, 1.0f, 1.0f, 1.0f}, 
            false, viking_tex.uuid, (char*) "assets/viking.das");

        // Submit assets to the renderer
        m_rend.submitAsset(viking);
        m_rend.submitTexture(viking_tex);

        m_rend.setup(m_cam, m_win); 
    }


    /// General run method
    void ImGUIApplication::run() {
        // Initialise the gui manager class instance
        m_ui_man = std::make_unique<UIManager>(m_rend);
        while(m_win.isRunning()) {
            // Measure time consumed for renderer current frame
            auto t1 = std::chrono::high_resolution_clock::now();
            m_ui_man->updateIO(m_win);
            char buf[128] = { 0 };
            float fl = 0;

            ImGui::NewFrame();
                // This is a custom test window sample
                ImGui::Begin("Test window", NULL, ImGuiWindowFlags_MenuBar);
                    ImGui::Text("This button click prints out appropriate message regarding the button click");
                    if(ImGui::Button("Test"))
                        LOG("You just clicked a test button");
                    ImGui::Text("Framerate: %f FPS\n", m_ui_man->getIO()->Framerate);
                ImGui::End();

                // Use this to test functionality
                //ImGui::ShowDemoWindow();
            ImGui::EndFrame();

            ImGui::Render();
            ImDrawData *p_draw_data = ImGui::GetDrawData();

            // Update UI element generation
            m_ui_man->render(p_draw_data, m_win);

            // Update the renderer
            m_rend.update();

            //std::this_thread::sleep_for(std::chrono::milliseconds(10));
            auto t2 = std::chrono::high_resolution_clock::now();

            m_ui_man->setTime(t1, t2);
        }

        ImGui::DestroyContext();
    }
}


int main() {
    deng_InitWindowAPI();
    {
        deng::ImGUIApplication app;
        app.run();
    }
    deng_DeinitWindowAPI();

    return EXIT_SUCCESS;
}
