// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GameEditor.cpp - Main game editor gui implementation
// author: Karl-Mihkel Ott

#define GAME_EDITOR_CPP
#include "deng/Executables/deng-game-editor/GameEditor.h"

namespace DENG {
	namespace Editor {


		wxBEGIN_EVENT_TABLE(GameEditor, wxFrame)
			// file
			EVT_MENU(ID_NEW, GameEditor::_OnNewClick)
			EVT_MENU(ID_OPEN, GameEditor::_OnOpenClick)
			EVT_MENU(ID_SAVE, GameEditor::_OnSaveClick)

			// file::import
			EVT_MENU(ID_DAS, GameEditor::_OnDasImportClick)
			EVT_MENU(ID_GLTF, GameEditor::_OnGLTFImportClick)
			EVT_MENU(ID_OBJ, GameEditor::_OnWavefrontOBJImportClick)
			EVT_MENU(ID_STL, GameEditor::_OnSTLImportClick)

			// add
			EVT_MENU(ID_NODE, GameEditor::_OnNodeClick)

			// add::mesh
			EVT_MENU(ID_SPHERE, GameEditor::_OnSphereMeshClick)
			EVT_MENU(ID_CUBE, GameEditor::_OnCubeMeshClick)
			EVT_MENU(ID_CONE, GameEditor::_OnConeMeshClick)

			// add::light
			EVT_MENU(ID_DIRECTIONAL_LIGHT, GameEditor::_OnDirectionalLightClick)
			EVT_MENU(ID_POINT_LIGHT, GameEditor::_OnPointLightClick)
			EVT_MENU(ID_AMBIENT_LIGHT, GameEditor::_OnAmbientLightClick)

			// add::camera
			EVT_MENU(ID_FIRST_PERSON_CAMERA, GameEditor::_OnFirstPersonCameraClick)
			EVT_MENU(ID_THIRD_PERSON_CAMERA, GameEditor::_OnThirdPersonCameraClick)
			EVT_MENU(ID_EDITOR_CAMERA, GameEditor::_OnEditorCameraClick)
		wxEND_EVENT_TABLE()

		GameEditor::GameEditor() :
			wxFrame(NULL, wxID_ANY, "DENG game editor", wxDefaultPosition, wxSize(1880, 1020)),
			m_mgr(this, wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_ALLOW_FLOATING)
		{
			_CreateMenubar();
			_CreateEditorLayout();
			_CreateRuntimeToolbar();
			_SetupViewport();
		}


		GameEditor::~GameEditor() {
			delete m_grid;
			delete m_camera;
			Registry* reg = Registry::GetInstance();
			reg->DeleteInstance();
			m_mgr.UnInit();
		}


		void GameEditor::_CreateMenubar() {
			m_menubar = new wxMenuBar();
			wxMenu* file = new wxMenu;
			file->Append(ID_NEW, wxT("&New"), "Create a new deng project");
			file->Append(ID_OPEN, wxT("&Open"), "Open an existing project");
			file->Append(ID_SAVE, wxT("&Save"), "Save the current project");
			
			wxMenu* import = new wxMenu;
			import->Append(ID_DAS, wxT("&DAS"), "Import a das model");
			import->Append(ID_GLTF, wxT("&GLTF"), "Import a gltf model");
			import->Append(ID_OBJ, wxT("&Wavefront Obj"), "Import a wavefront Obj model");
			import->Append(ID_STL, wxT("&STL"), "Import a stl model");
			
			file->Append(ID_IMPORT, wxT("&Import"), import, "Import a model");
			file->Append(wxID_EXIT, wxT("&Quit"), "Quit the application");
			m_menubar->Append(file, "&File");

			wxMenu* add = new wxMenu;
			add->Append(ID_NODE, wxT("&Node"), "Add a new node to the scene");
			wxMenu* mesh = new wxMenu;
			mesh->Append(ID_SPHERE, wxT("&Sphere"), "Add a sphere to currently selected node");
			mesh->Append(ID_CUBE, wxT("&Cube"), "Add a cube");
			mesh->Append(ID_CONE, wxT("&Cone"), "Add a cone");
			add->Append(ID_MESH, wxT("&Mesh"), mesh, "Add a mesh");
			
			wxMenu* light = new wxMenu;
			light->Append(ID_DIRECTIONAL_LIGHT, wxT("&Directional light"), "Add a directional light to the scene");
			light->Append(ID_POINT_LIGHT, wxT("&Point light"), "Add a point light to the scene");
			light->Append(ID_AMBIENT_LIGHT, wxT("&Ambient light"), "Add an ambient light to the scene");
			add->Append(ID_LIGHT, wxT("&Light"), light, "Add a light source");

			wxMenu* camera = new wxMenu;
			camera->Append(ID_FIRST_PERSON_CAMERA, wxT("&First person camera"), "Add a first person camera to the scene");
			camera->Append(ID_THIRD_PERSON_CAMERA, wxT("&Third person camera"), "Add a third person camera to the scene");
			camera->Append(ID_EDITOR_CAMERA, wxT("&Editor camera"), "Add an editor camera to the scene");
			add->Append(ID_CAMERA, wxT("&Camera"), camera, "Add a camera to the scene");
			m_menubar->Append(add, "&Add");
			SetMenuBar(m_menubar);
		}


		void GameEditor::_CreateRuntimeToolbar() {
			wxBitmap bmp = wxBitmap::NewFromPNGData(GetRunIcon16x16(), GetRunIcon16x16Size());
			wxBitmapButton* run = new wxBitmapButton(m_toolbar, wxID_ANY, bmp, wxDefaultPosition, wxDefaultSize, wxBU_RIGHT, wxDefaultValidator, wxT("Run"));
			m_toolbar->AddControl(run);
		}


		void GameEditor::_CreateEditorLayout() {
			m_toolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxSize(300, 0));
			m_hierarchy = new wxTreeCtrl(this, wxID_ANY);
			m_viewport = new RendererViewport(this, DXML::Configuration::Backend::VULKAN);
			m_assets = new wxTextCtrl(this, wxID_ANY, "Assets panel", wxDefaultPosition, wxSize(200, 150), wxNO_BORDER | wxTE_MULTILINE);
			m_inspector = new wxTextCtrl(this, wxID_ANY, "Inspector panel", wxDefaultPosition, wxSize(200, 150), wxNO_BORDER | wxTE_MULTILINE);

			auto id = m_hierarchy->AddRoot("Level 0");
			m_hierarchy->AppendItem(id, "Some model desu");

			m_mgr.AddPane(m_toolbar, wxTOP, "Toolbar");
			m_mgr.AddPane(m_hierarchy, wxLEFT, "Scene hierarchy");
			m_mgr.AddPane(m_viewport, wxCENTER, "Viewport");
			m_mgr.AddPane(m_assets, wxBOTTOM, "Assets");
			m_mgr.AddPane(m_inspector, wxRIGHT, "Inspector");
			m_mgr.Update();
		}


		void GameEditor::_SetupViewport() {
			m_viewport->Setup();

			DENG::Renderer* rend = m_viewport->GetRenderer();

			m_camera = new EditorCamera(NULL, "__GameEditor__", *rend, EditorCameraConfiguration());
			m_grid = new GridGenerator(NULL, "__Grid__", *rend, 50.f, 50.f, 0.4f, 0.4f, m_camera->GetId());
			m_camera->BindScript<EditorCameraController>(m_viewport);

			Registry* reg = Registry::GetInstance();
			reg->AttachEntities();
			rend->LoadShaders();
			m_viewport->Refresh();
			m_viewport->Update();
		}


		void GameEditor::_LoadProject() {
			// set the title accordingly
			SetTitle(m_project.GetXMLGame().meta.name + " | DENG game editor");

			// setup levels

		}


		void GameEditor::_OnNewProjectWizardClose(wxCloseEvent& _ev) {
			NewProjectWizard* wiz = (NewProjectWizard*)wxWindow::FindWindowById(ID_NEW_PROJECT_WIZARD);
			if (wiz->Success()) {
				_LoadProject();
				wiz->Destroy();
			} else {
				wxMessageBox(std::string("Could not create a new project in " + m_project.GetProjectPath() + "."), "Error", wxICON_ERROR | wxOK);
			}
		}


		void GameEditor::_OnNewClick(wxCommandEvent& _ev) {
			NewProjectWizard* wiz = new NewProjectWizard(this, &m_project);
			wiz->Bind(wxEVT_CLOSE_WINDOW, &GameEditor::_OnNewProjectWizardClose, this);
			wiz->Show();
			_ev.Skip();
		}


		void GameEditor::_OnOpenClick(wxCommandEvent& _ev) {
			wxFileDialog dir_dialog = wxFileDialog(this, "Choose a project file", wxEmptyString, wxEmptyString, "game.xml");
			if (dir_dialog.ShowModal() == wxID_OK) {
				auto path = dir_dialog.GetPath().ToStdString();
				if (m_project.LoadProject(path)) {
					_LoadProject();
				}
			}
		}


		void GameEditor::_OnSaveClick(wxCommandEvent& _ev) {

		}


		void GameEditor::_OnDasImportClick(wxCommandEvent& _ev) {

		}


		void GameEditor::_OnGLTFImportClick(wxCommandEvent& _ev) {

		}


		void GameEditor::_OnWavefrontOBJImportClick(wxCommandEvent &_ev) {

		}


		void GameEditor::_OnSTLImportClick(wxCommandEvent& _ev) {

		}


		void GameEditor::_OnNodeClick(wxCommandEvent& _ev) {

		}


		void GameEditor::_OnSphereMeshClick(wxCommandEvent& _ev) {

		}


		void GameEditor::_OnCubeMeshClick(wxCommandEvent& _ev) {

		}


		void GameEditor::_OnConeMeshClick(wxCommandEvent& _ev) {

		}


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
	}
}