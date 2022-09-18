// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: InspectorPanel.h - Inspector panel class header
// author: Karl-Mihkel Ott


#ifndef INSPECTOR_PANEL_H
#define INSPECTOR_PANEL_H

#ifdef INSPECTOR_PANEL_CPP
	#include <wx/wx.h>
	#include <wx/valnum.h>
	#include <wx/collpane.h>
	#include "deng/deng.h"
#endif

#include "deng/Executables/deng-game-editor/AnimationInspectorPanel.h"
#include "deng/Executables/deng-game-editor/NodeInspectorPanel.h"

namespace DENG {
	namespace Editor {

		class InspectorPanel : public wxWindow {
			private:
				wxBoxSizer* m_sizer = nullptr;
				AnimationInspectorPanel* m_ani_inspector = nullptr;
				NodeInspectorPanel* m_node_inspector = nullptr;

			private:
				inline void _HideAll() {
					m_ani_inspector->Hide();
					m_node_inspector->Hide();
				}

			public:
				InspectorPanel(wxWindow* _parent);
				
				inline void ShowAnimationPanel(Animation* _ani) {
					_HideAll();
					m_ani_inspector->View(_ani);
				}

				inline void ShowNodePanel(NodeLoader* _node) {
					_HideAll();
					m_node_inspector->View(_node);
				}
				//void ShowMeshPanel(MeshLoader* _mesh);
				//void ShowScenePanel(SceneLoader* _scene);
				//void ShowModelPanel(ModelLoader* _model);
		};
	}
}

#endif