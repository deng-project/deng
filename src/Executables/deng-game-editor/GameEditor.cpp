// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GameEditor.cpp - Main game editor gui implementation
// author: Karl-Mihkel Ott

#define GAME_EDITOR_CPP
#include "deng/Executables/deng-game-editor/GameEditor.h"

namespace DENG {
	namespace Editor {


		wxBEGIN_EVENT_TABLE(GameEditor, wxFrame)
			// hierarchy
			EVT_TREE_ITEM_ACTIVATED(ID_EDITOR_HIERARCHY_PANEL, GameEditor::_OnHierarchyItemClick)
		wxEND_EVENT_TABLE()

		GameEditor::GameEditor(ProjectDataManager *_data_man) :
			wxFrame(NULL, wxID_ANY, "DENG game editor", wxDefaultPosition, wxSize(1880, 1020)),
			m_mgr(this, wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_ALLOW_FLOATING),
			m_project(_data_man)
		{
			m_menubar = new EditorMenubar;
			SetMenuBar(m_menubar);
			
			_CreateEditorLayout();
			if (m_viewport->GetBackend() == DXML::GRAPHICS_BACKEND_OPENGL) {
				SetTitle("DENG game editor (OpenGL)");
			} else if (m_viewport->GetBackend() == DXML::GRAPHICS_BACKEND_VULKAN) {
				SetTitle("DENG game editor (Vulkan)");
			}
			_CreateRuntimeToolbar();
		}


		GameEditor::~GameEditor() {
			delete m_grid;
			delete m_camera;
			
			Destroy();

			m_model_loaders.clear();
			Registry* reg = Registry::GetInstance();
			reg->DeleteInstance();
			m_mgr.UnInit();
		}


		void GameEditor::_CreateRuntimeToolbar() {
			wxBitmap bmp = wxBitmap::NewFromPNGData(GetRunIcon16x16(), GetRunIcon16x16Size());
			wxBitmapButton* run = new wxBitmapButton(m_toolbar, wxID_ANY, bmp, wxDefaultPosition, wxDefaultSize, wxBU_RIGHT, wxDefaultValidator, wxT("Run"));
			m_toolbar->AddControl(run);
		}


		void GameEditor::_CreateEditorLayout() {
			m_toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxSize(300, 0));
			m_hierarchy = new wxTreeCtrl(this, ID_EDITOR_HIERARCHY_PANEL, wxDefaultPosition, wxSize(200, 600), wxTR_HIDE_ROOT | wxTR_EDIT_LABELS | wxTR_HAS_BUTTONS);
			m_viewport = new RendererViewport(this, DXML::GRAPHICS_BACKEND_OPENGL);
			m_assets = new wxTextCtrl(this, wxID_ANY, "Assets panel", wxDefaultPosition, wxSize(600, 200), wxNO_BORDER | wxTE_MULTILINE);
			m_status = new wxStatusBar(this, wxID_ANY);
			SetStatusBar(m_status);

			_SetupViewport();
			m_inspector_panel = new InspectorPanel(this, m_viewport->GetRenderer());

			// implicit root node
			m_root = m_hierarchy->AddRoot("hidden");

			m_mgr.AddPane(m_toolbar, wxTOP, "Toolbar");
			m_mgr.AddPane(m_hierarchy, wxLEFT, "Scene hierarchy");
			m_mgr.AddPane(m_viewport, wxCENTER, "Viewport");
			m_mgr.AddPane(m_assets, wxBOTTOM, "Assets");
			m_mgr.AddPane(m_inspector_panel, wxRIGHT, "Inspector");
			m_mgr.Update();
		}


		void GameEditor::_SetupViewport() {
			m_viewport->Setup();

			DENG::Renderer* rend = m_viewport->GetRenderer();

			m_camera = new EditorCamera(NULL, "__GameEditor__", *rend);
			m_grid = new GridGenerator(NULL, "__Grid__", *rend, 50.f, 50.f, 0.4f, 0.4f, m_camera->GetId());
			m_camera->BindScript<EditorCameraController>(m_viewport);

			Registry* reg = Registry::GetInstance();
			reg->AttachEntities();
			m_viewport->Update();
		}


		void GameEditor::_RecurseNodesToHierarchy(wxTreeItemId _root_id, NodeLoader &_node) {
			std::queue<std::pair<wxTreeItemId, NodeLoader*>> nodes;
			nodes.push(std::make_pair(_root_id, &_node));

			while (!nodes.empty()) {
				wxTreeItemId id = m_hierarchy->AppendItem(nodes.front().first, nodes.front().second->GetName(), -1, -1, new TreeItemNode(nodes.front().second));

				// check if the node contains a skeleton or a mesh
				if (nodes.front().second->GetMeshLoader()) {
					m_hierarchy->AppendItem(id, nodes.front().second->GetMeshLoader()->GetName(), -1, -1, new TreeItemMesh(nodes.front().second->GetMeshLoader()));
				}
				if (nodes.front().second->GetSkeleton()) {
					m_hierarchy->AppendItem(id, nodes.front().second->GetSkeleton()->GetName(), -1, -1, new TreeItemSkeleton(nodes.front().second->GetSkeleton()));
				}
				
				// add child nodes
				for (auto it = nodes.front().second->GetChildNodes().begin(); it != nodes.front().second->GetChildNodes().end(); it++) {
					nodes.push(std::make_pair(id, &(*it)));
				}

				nodes.pop();
			}
		}


		void GameEditor::AppendModelToHierarchy() {
			ModelLoader& model = m_model_loaders.back();
			wxTreeItemId model_id = m_hierarchy->AppendItem(m_root, model.GetName(), -1, -1, new TreeItem(TREE_ITEM_TYPE_MODEL));

			// for each animation in the model append it to the hierarchy
			for (auto ani_it = model.GetAnimations().begin(); ani_it != model.GetAnimations().end(); ani_it++) {
				m_hierarchy->AppendItem(model_id, ani_it->name, -1, -1, new TreeItemAnimation(&(*ani_it)));
			}

			// for each scene in the model append it to the hierarchy
			for (auto sc_it = model.GetScenes().begin(); sc_it != model.GetScenes().end(); sc_it++) {
				wxTreeItemId scene_id = m_hierarchy->AppendItem(model_id, sc_it->GetName(), -1, -1, new TreeItemScene(&(*sc_it)));

				// for each node in the scene append it to the hierarchy
				for (auto node_it = sc_it->GetRootNodes().begin(); node_it != sc_it->GetRootNodes().end(); node_it++) {
					_RecurseNodesToHierarchy(scene_id, *node_it);
				}
			}
		}


		void GameEditor::_LoadProject() {
			// set the title accordingly

			
			// for(auto it = game.rt.levels.)

		}


		void GameEditor::_OnNewProjectWizardClose(wxCloseEvent& _ev) {
			NewProjectWizard* wiz = (NewProjectWizard*)wxWindow::FindWindowById(ID_NEW_PROJECT_WIZARD);
			if (wiz->Success()) {
				_LoadProject();
			} else {
				wxMessageBox(std::string("Could not create a new project"), "Error", wxICON_ERROR | wxOK);
			}
			wiz->Destroy();
		}


		void GameEditor::_OnHierarchyItemClick(wxTreeEvent& _ev) {
			TreeItem* item_type = reinterpret_cast<TreeItem*>(m_hierarchy->GetItemData(_ev.GetItem()));
			DENG_ASSERT(item_type);

			switch (item_type->GetType()) {
				case TREE_ITEM_TYPE_MODEL:
					std::cout << "Clicked model tree item" << std::endl;
					break;

				case TREE_ITEM_TYPE_SCENE:
					std::cout << "Clicked scene tree item" << std::endl;
					break;

				case TREE_ITEM_TYPE_NODE:
				{
					TreeItemNode* node_tree_item = (TreeItemNode*)item_type;
					m_inspector_panel->ShowNodePanel(node_tree_item->GetNodeLoader());
					break;
				}

				case TREE_ITEM_TYPE_SKELETON:
					std::cout << "Clicked skeleton tree item" << std::endl;
					break;

				case TREE_ITEM_TYPE_MESH:
				{
					TreeItemMesh* mesh_tree_item = (TreeItemMesh*)item_type;
					m_inspector_panel->ShowMeshPanel(mesh_tree_item->GetMeshLoader());
					break;
				}

				case TREE_ITEM_TYPE_ANIMATION:
				{
					TreeItemAnimation* ani_tree_item = (TreeItemAnimation*)item_type;
					m_inspector_panel->ShowAnimationPanel(ani_tree_item->GetAnimation());
					break;
				}

				default:
					break;
			}
			_ev.Skip();
		}
	}
}