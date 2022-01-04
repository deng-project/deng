// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SceneEditor.h - Main scene editor class header
// author: Karl-Mihkel Ott

#ifndef SCENE_EDITOR_H
#define SCENE_EDITOR_H


#ifdef SCENE_EDITOR_CPP
    #include <deng/deng.h>
    #include <deng/ui.h>
    #include <imgui-layer/scene_editor/sc_ed_entity_panel.h>
#endif


/// Some settings definitions
#define WIDTH               1280
#define HEIGHT              720

namespace dengEditor {

    class SceneEditor3D : SceneEditor3DEntityPanel {
    private:
        deng::Window m_win;
        deng::Camera3D m_cam;
        deng::Renderer m_rend;
        std::unique_ptr<deng::UIManager> m_ui_man;

    public:
        SceneEditor3D(deng_RendererHintBits rend_hints, deng_WindowHint win_hints);
        void run();
    };
}

#endif
