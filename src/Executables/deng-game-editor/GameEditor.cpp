// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GameEditor.cpp - Main game editor gui implementation
// author: Karl-Mihkel Ott

#define GAME_EDITOR_CPP
#include "deng/Executables/deng-game-editor/GameEditor.h"

namespace DENG {
	namespace Editor {

		GameEditor::GameEditor() :
			wxFrame(NULL, wxID_ANY, "Untitled project", wxDefaultPosition, wxSize(1880, 1020)),
			m_mgr(this, wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_RECTANGLE_HINT)
		{
			wxTextCtrl *hierarchy = new wxTextCtrl(this, wxID_ANY, "Hierarchy panel", wxDefaultPosition, wxSize(200, 150), wxNO_BORDER | wxTE_MULTILINE);
			RendererViewport* viewport = new RendererViewport(this, DXML::Configuration::Backend::OPENGL);
			wxTextCtrl *assets = new wxTextCtrl(this, wxID_ANY, "Assets panel", wxDefaultPosition, wxSize(200, 150), wxNO_BORDER | wxTE_MULTILINE);
			wxTextCtrl* inspector = new wxTextCtrl(this, wxID_ANY, "Inspector panel", wxDefaultPosition, wxSize(200, 150), wxNO_BORDER | wxTE_MULTILINE);

			m_mgr.AddPane(hierarchy, wxLEFT, "Scene hierarchy");
			m_mgr.AddPane(viewport, wxCENTER, "Viewport");
			m_mgr.AddPane(assets, wxBOTTOM, "Assets");
			m_mgr.AddPane(inspector, wxRIGHT, "Inspector");
			m_mgr.Update();
			viewport->Setup();

			DENG::Renderer *rend = viewport->GetRenderer();
			
			m_camera = new EditorCamera(NULL, "__GameEditor__", *rend, EditorCameraConfiguration());
			m_grid = new GridGenerator(NULL, "__Grid__", *rend, 50.f, 50.f, 0.4f, 0.4f, m_camera->GetId());
			m_camera->BindScript<EditorCameraController>();

			Registry* reg = Registry::GetInstance();
			reg->AttachEntities();
			rend->LoadShaders();
			viewport->Refresh();
			viewport->Update();
		}


		GameEditor::~GameEditor() {
			delete m_grid;
			delete m_camera;
			Registry* reg = Registry::GetInstance();
			reg->DeleteInstance();
			m_mgr.UnInit();
		}
	}
}