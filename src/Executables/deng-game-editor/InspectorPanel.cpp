// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: InspectorPanel.h - Inspector panel class implementation
// author: Karl-Mihkel Ott

#define INSPECTOR_PANEL_CPP
#include "deng/Executables/deng-game-editor/InspectorPanel.h"

namespace DENG {
	namespace Editor {

		InspectorPanel::InspectorPanel(wxWindow* _parent, Renderer *_rend) :
			wxWindow(_parent, wxID_ANY, wxDefaultPosition, wxSize(250, 600)) 
		{
			m_sizer = new wxBoxSizer(wxVERTICAL);
			m_node_inspector = new NodeInspectorPanel(this);
			m_ani_inspector = new AnimationInspectorPanel(this);
			m_mesh_inspector = new MeshInspectorPanel(this, _rend);
			
			m_sizer->Add(m_ani_inspector, 0, wxALL);
			m_sizer->Add(m_node_inspector, 0, wxALL);
			m_sizer->Add(m_mesh_inspector, 0, wxALL);
			_HideAll();
			SetSizer(m_sizer);
		}
	}
}