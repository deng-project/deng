// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: SceneEditorEntityPanel.h - Scene editor's entity panel class header
// author: Karl-Mihkel Ott

#ifndef SCENE_EDITOR_ENTITY_PANEL_H
#define SCENE_EDITOR_ENTITY_PANEL_H

#ifdef SCENE_EDITOR_ENTITY_PANEL_CPP
    #include <deng/deng.h>
    #include <deng/ui.h>
#endif

namespace DENG {

    class SceneEditor3DEntityPanel {
    private:
        /// Boolean flags for handling user input
        bool m_new_scene = false;
        bool m_open_scene = false;
        bool m_import = false;
        bool m_save_scene = false;
        bool m_preferences = false;
        

    private:
        /// Upper elements
        void __mkEntityPanelMenuBar();
        void __mkEntityPanelAssetList(const std::vector<deng_Id> &assets, deng::Registry &reg);
        void __mkEntityPanelTextureList(const std::vector<deng_Id> &textures, deng::Registry &reg);

        /// Entity panel headers
        void __mkLightingHeader();
        void __mkAntiAliasingHeader();
        void __mkAudioHeader();
        void __mkMaterialPropertyHeader();
        void __mkCameraHeader();

    public:
        /// Create entity panel instance for current frame
        void spawnEntityPanel(deng::Renderer &rend);
    };
}

#endif
