/// DENG: dynamic engine - powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: sc_ed_ui.h - Scene editor ui element handler class
/// author: Karl-Mihkel Ott


#ifndef __SC_ED_UI_H
#define __SC_ED_UI_H

#ifdef __SC_ED_UI_CPP
    #include <deng/deng.h>
    #include <deng/ui.h>
    #include <imgui-layer/scene_editor/sc_ed_entity_panel.h>
#endif

namespace dengEditor {

    class SceneEditorUI : private SceneEditor3DEntityPanel {
    public:
        /// Render windows for new frame
        void newFrame();
    };
}

#endif
