// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GameEditor.h - Main game editor gui header
// author: Karl-Mihkel Ott

#ifndef GAME_EDITOR_H
#define GAME_EDITOR_H

#ifdef GAME_EDITOR_CPP
    #include <any>
    #include <string>
    #include <vector>
    #include <array>
    #include <variant>
    #include <chrono>
    #include <unordered_map>
    #include <vulkan/vulkan.h>
    #include <wx/wx.h>
    #include <wx/aui/aui.h>

    #include "trs/Vector.h"
    #include "trs/Points.h"
    #include "trs/Quaternion.h"	
    #include "dxml/GameStructs.h"
    
    #include "deng/Api.h"
    #include "deng/ProjectDataManager.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Renderer.h"
    #include "deng/GPUMemoryManager.h"

    #include "deng/Executables/deng-game-editor/GuiIds.h"
    #include "deng/Executables/deng-game-editor/StaticResources.h"
    #include "deng/Executables/deng-game-editor/OpenGLLoader.h"
    #include "deng/Executables/deng-game-editor/RendererViewport.h"
#endif


namespace DENG {
    namespace Editor {

        class GameEditor : public wxFrame {
            private:
                wxAuiManager m_mgr;
                wxMenuBar* m_menubar = nullptr;

            public:
                GameEditor();
                ~GameEditor();
        };
    }
}

#endif
