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

			// file
			EVT_MENU(ID_EDITOR_MENUBAR_FILE_NEW, GameEditor::_OnNewClick)
			EVT_MENU(ID_EDITOR_MENUBAR_FILE_OPEN, GameEditor::_OnOpenClick)
			EVT_MENU(ID_EDITOR_MENUBAR_FILE_SAVE, GameEditor::_OnSaveClick)
			EVT_MENU(ID_EDITOR_MENUBAR_FILE_IMPORT_DAS, GameEditor::_OnDasImportClick)
			EVT_MENU(ID_EDITOR_MENUBAR_FILE_IMPORT_GLTF, GameEditor::_OnGLTFImportClick)
			EVT_MENU(ID_EDITOR_MENUBAR_FILE_IMPORT_OBJ, GameEditor::_OnWavefrontOBJImportClick)
			EVT_MENU(ID_EDITOR_MENUBAR_FILE_IMPORT_STL, GameEditor::_OnSTLImportClick)

			// add
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_SKYBOX, GameEditor::_OnSkyboxClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_NODE, GameEditor::_OnNodeClick)

			// add::mesh
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_MESH_SPHERE, GameEditor::_OnSphereMeshClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_MESH_CUBE, GameEditor::_OnCubeMeshClick)
			// EVT_MENU(ID_EDITOR_MENUBAR_ADD_MESH_CONE, GameEditor::_OnConeMeshClick)

			// add::light
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_LIGHT_DIRECTIONAL_LIGHT, GameEditor::_OnDirectionalLightClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_LIGHT_POINT_LIGHT, GameEditor::_OnPointLightClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_LIGHT_AMBIENT_LIGHT, GameEditor::_OnAmbientLightClick)

			// add::camera
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_CAMERA_FIRST_PERSON_CAMERA, GameEditor::_OnFirstPersonCameraClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_CAMERA_THIRD_PERSON_CAMERA, GameEditor::_OnThirdPersonCameraClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_CAMERA_EDITOR_CAMERA, GameEditor::_OnEditorCameraClick)

#ifdef __DEBUG
			EVT_MENU(ID_EDITOR_MENUBAR_DEBUG_TRIANGLE, GameEditor::_OnDebugTriangleClick)
#endif
		wxEND_EVENT_TABLE()

		GameEditor::GameEditor() :
			wxFrame(NULL, wxID_ANY, "DENG game editor", wxDefaultPosition, wxSize(1880, 1020)),
			m_mgr(this, wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_ALLOW_FLOATING)
		{
			_CreateMenubar();
			CreateStatusBar();
			_CreateEditorLayout();
			if (m_viewport->GetBackend() == DXML::Configuration::Backend::OPENGL) {
				SetTitle("DENG game editor (OpenGL)");
			} else if (m_viewport->GetBackend() == DXML::Configuration::Backend::VULKAN) {
				SetTitle("DENG game editor (Vulkan)");
			}
			_CreateRuntimeToolbar();
		}


		GameEditor::~GameEditor() {
			delete m_grid;
			delete m_camera;
			m_model_loaders.clear();
			Registry* reg = Registry::GetInstance();
			reg->DeleteInstance();
			m_mgr.UnInit();
		}


		void GameEditor::_CreateMenubar() {
			m_menubar = new wxMenuBar();

			wxMenu* file = new wxMenu;
			file->Append(ID_EDITOR_MENUBAR_FILE_NEW, wxT("&New"), "Create a new deng project");
			file->Append(ID_EDITOR_MENUBAR_FILE_OPEN, wxT("&Open"), "Open an existing project");
			file->Append(ID_EDITOR_MENUBAR_FILE_SAVE, wxT("&Save"), "Save the current project");
			
			wxMenu* import = new wxMenu;
			import->Append(ID_EDITOR_MENUBAR_FILE_IMPORT_DAS, wxT("&DAS"), "Import a das model");
			import->Append(ID_EDITOR_MENUBAR_FILE_IMPORT_GLTF, wxT("&GLTF"), "Import a gltf model");
			import->Append(ID_EDITOR_MENUBAR_FILE_IMPORT_OBJ, wxT("&Wavefront Obj"), "Import a wavefront Obj model");
			import->Append(ID_EDITOR_MENUBAR_FILE_IMPORT_STL, wxT("&STL"), "Import a stl model");
			
			file->Append(ID_EDITOR_MENUBAR_FILE_IMPORT, wxT("&Import"), import, "Import a model");
			file->Append(wxID_EXIT, wxT("&Quit"), "Quit the application");
			m_menubar->Append(file, "&File");

			wxMenu* add = new wxMenu;
			add->Append(ID_EDITOR_MENUBAR_ADD_SKYBOX, wxT("&Skybox"), "Add a new skybox to the scene");
			add->Append(ID_EDITOR_MENUBAR_ADD_NODE, wxT("&Node"), "Add a new node to the scene");
			wxMenu* mesh = new wxMenu;
			mesh->Append(ID_EDITOR_MENUBAR_ADD_MESH_SPHERE, wxT("&Sphere"), "Add a sphere to currently selected node");
			mesh->Append(ID_EDITOR_MENUBAR_ADD_MESH_CUBE, wxT("&Cube"), "Add a cube");
			mesh->Append(ID_EDITOR_MENUBAR_ADD_MESH_CONE, wxT("&Cone"), "Add a cone");
			add->Append(ID_EDITOR_MENUBAR_ADD_MESH, wxT("&Mesh"), mesh, "Add a mesh");
			
			wxMenu* light = new wxMenu;
			light->Append(ID_EDITOR_MENUBAR_ADD_LIGHT_DIRECTIONAL_LIGHT, wxT("&Directional light"), "Add a directional light to the scene");
			light->Append(ID_EDITOR_MENUBAR_ADD_LIGHT_POINT_LIGHT, wxT("&Point light"), "Add a point light to the scene");
			light->Append(ID_EDITOR_MENUBAR_ADD_LIGHT_AMBIENT_LIGHT, wxT("&Ambient light"), "Add an ambient light to the scene");
			add->Append(ID_EDITOR_MENUBAR_ADD_LIGHT, wxT("&Light"), light, "Add a light source");

			wxMenu* camera = new wxMenu;
			camera->Append(ID_EDITOR_MENUBAR_ADD_CAMERA_FIRST_PERSON_CAMERA, wxT("&First person camera"), "Add a first person camera to the scene");
			camera->Append(ID_EDITOR_MENUBAR_ADD_CAMERA_THIRD_PERSON_CAMERA, wxT("&Third person camera"), "Add a third person camera to the scene");
			camera->Append(ID_EDITOR_MENUBAR_ADD_CAMERA_EDITOR_CAMERA, wxT("&Editor camera"), "Add an editor camera to the scene");
			add->Append(ID_EDITOR_MENUBAR_ADD_CAMERA, wxT("&Camera"), camera, "Add a camera to the scene");
			m_menubar->Append(add, "&Add");

#ifdef __DEBUG
			wxMenu* debug = new wxMenu;
			debug->Append(ID_EDITOR_MENUBAR_DEBUG_TRIANGLE, wxT("&Triangle"));
			m_menubar->Append(debug, "&Debug");
#endif
			SetMenuBar(m_menubar);
		}


		void GameEditor::_CreateRuntimeToolbar() {
			wxBitmap bmp = wxBitmap::NewFromPNGData(GetRunIcon16x16(), GetRunIcon16x16Size());
			wxBitmapButton* run = new wxBitmapButton(m_toolbar, wxID_ANY, bmp, wxDefaultPosition, wxDefaultSize, wxBU_RIGHT, wxDefaultValidator, wxT("Run"));
			m_toolbar->AddControl(run);
		}


		void GameEditor::_CreateEditorLayout() {
			m_toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxSize(300, 0));
			m_hierarchy = new wxTreeCtrl(this, ID_EDITOR_HIERARCHY_PANEL, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT | wxTR_EDIT_LABELS | wxTR_HAS_BUTTONS);
			m_viewport = new RendererViewport(this, DXML::Configuration::Backend::OPENGL);
			m_assets = new wxTextCtrl(this, wxID_ANY, "Assets panel", wxDefaultPosition, wxSize(200, 150), wxNO_BORDER | wxTE_MULTILINE);
			
			_SetupViewport();

			// m_empty_inspector = new wxStaticText(this, wxID_ANY, "Select a hierarchy node to view inspector panel");
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
			rend->LoadShaders();
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


		void GameEditor::_AppendModelToHierarchy() {
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
			SetTitle(m_project.GetXMLGame().meta.name + " | DENG game editor");
			
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


		void GameEditor::_OnNewClick(wxCommandEvent& _ev) {
			NewProjectWizard* wiz = new NewProjectWizard(this, &m_project);
			wiz->Bind(wxEVT_CLOSE_WINDOW, &GameEditor::_OnNewProjectWizardClose, this);
			wiz->Show();
			_ev.Skip();
		}


		void GameEditor::_OnOpenClick(wxCommandEvent& _ev) {
			wxFileDialog fdialog = wxFileDialog(this, "Choose a project file", wxEmptyString, wxEmptyString, "game.xml");
			if (fdialog.ShowModal() == wxID_OK) {
				auto path = fdialog.GetPath().ToStdString();
				if (m_project.LoadProject(path)) {
					_LoadProject();
				}
			}
		}


		void GameEditor::_OnSaveClick(wxCommandEvent& _ev) {

		}


		void GameEditor::_OnDasImportClick(wxCommandEvent& _ev) {
			wxFileDialog fdialog = wxFileDialog(this, "Choose a das file", wxEmptyString, wxEmptyString, "*.das");
			if (fdialog.ShowModal() == wxID_OK) {
				auto path = fdialog.GetPath().ToStdString();
				m_model_loaders.emplace_back(nullptr, path, *m_viewport->GetRenderer(), m_camera->GetId());
				Registry* reg = Registry::GetInstance();
				reg->AttachEntities();
				_AppendModelToHierarchy();
				m_viewport->GetRenderer()->LoadShaders();
			}
		}


		void GameEditor::_OnGLTFImportClick(wxCommandEvent& _ev) {
			wxMessageBox("GLTF is not yet supported", "Error", wxICON_ERROR | wxOK, this);
			_ev.Skip();
		}


		void GameEditor::_OnWavefrontOBJImportClick(wxCommandEvent &_ev) {
			wxMessageBox("GLTF is not yet supported", "Error", wxICON_ERROR | wxOK, this);
			_ev.Skip();
		}


		void GameEditor::_OnSTLImportClick(wxCommandEvent& _ev) {
			wxMessageBox("GLTF is not yet supported", "Error", wxICON_ERROR | wxOK, this);
			_ev.Skip();
		}

		
		void GameEditor::_OnSkyboxClick(wxCommandEvent& _ev) {
			SkyboxGeneratorDialog dialog(this);
			dialog.ShowModal();
		}


		void GameEditor::_OnNodeClick(wxCommandEvent& _ev) {

		}


		void GameEditor::_OnCubeMeshClick(wxCommandEvent& _ev) {
			CubeGeneratorDialog dialog(this);
			if (dialog.ShowModal() == wxID_OK) {
				CubeGenerator gen = dialog.ConstructCube();
				m_model_loaders.emplace_back(std::move(gen.ToModelLoader(nullptr, *m_viewport->GetRenderer(), m_camera->GetId(), "cube.das", "Cube")));
				Registry* reg = Registry::GetInstance();
				reg->AttachEntities();
				_AppendModelToHierarchy();
				m_viewport->GetRenderer()->LoadShaders();
			}
		}


		void GameEditor::_OnSphereMeshClick(wxCommandEvent& _ev) {
			SphereGeneratorDialog dialog(this);
			if (dialog.ShowModal() == wxID_OK) {
				SphereGenerator gen = std::move(dialog.ConstructSphere());
				m_model_loaders.emplace_back(std::move(gen.ToModelLoader(nullptr, *m_viewport->GetRenderer(), m_camera->GetId(), "sphere.das", "Sphere")));
				Registry* reg = Registry::GetInstance();
				reg->AttachEntities();
				_AppendModelToHierarchy();
				m_viewport->GetRenderer()->LoadShaders();
			}
		}


		//void GameEditor::_OnConeMeshClick(wxCommandEvent& _ev) {

		//}


		void GameEditor::_OnDirectionalLightClick(wxCommandEvent& _ev) {

		}


		void GameEditor::_OnPointLightClick(wxCommandEvent& _ev) {

		}


		void GameEditor::_OnAmbientLightClick(wxCommandEvent& _ev) {

		}


		void GameEditor::_OnFirstPersonCameraClick(wxCommandEvent& _ev) {

		}


		void GameEditor::_OnThirdPersonCameraClick(wxCommandEvent& _ev) {

		}


		void GameEditor::_OnEditorCameraClick(wxCommandEvent& _ev) {

		}

#ifdef __DEBUG
		void GameEditor::_OnDebugTriangleClick(wxCommandEvent& _ev) {
			if (!m_triangle) {
				std::array<TRS::Point2D<float>, 3> vtx = {
					TRS::Point2D<float>{ -1.f, -1.f },
					TRS::Point2D<float>{ 0.f, 1.f },
					TRS::Point2D<float>{ 1.f, -1.f }
				};
				m_triangle = new TriangleTester(nullptr, *m_viewport->GetRenderer(), vtx);
				m_triangle->ChangeColor({ 1.f, 0.f, 0.f, 1.f });

				Registry* reg = Registry::GetInstance();
				reg->AttachEntities();
				m_viewport->GetRenderer()->LoadShaders();
			}
		}
#endif
	}
}