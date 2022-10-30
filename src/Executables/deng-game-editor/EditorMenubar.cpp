// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: EditorMenubar.h - DENG game editor menubar class header
// author: Karl-Mihkel Ott

#define EDITOR_MENUBAR_CPP
#include "deng/Executables/deng-game-editor/EditorMenubar.h"

namespace DENG {
	namespace Editor {

		wxBEGIN_EVENT_TABLE(EditorMenubar, wxMenuBar)
			EVT_MENU(ID_EDITOR_MENUBAR_FILE_NEW, EditorMenubar::_OnFile_NewClick)
			EVT_MENU(ID_EDITOR_MENUBAR_FILE_OPEN, EditorMenubar::_OnFile_OpenClick)
			EVT_MENU(ID_EDITOR_MENUBAR_FILE_SAVE, EditorMenubar::_OnFile_SaveClick)
			EVT_MENU(ID_EDITOR_MENUBAR_FILE_IMPORT_DAS, EditorMenubar::_OnFile_Import_DASClick)
			EVT_MENU(ID_EDITOR_MENUBAR_FILE_IMPORT_GLTF, EditorMenubar::_OnFile_Import_GLTFClick)
			EVT_MENU(ID_EDITOR_MENUBAR_FILE_IMPORT_OBJ, EditorMenubar::_OnFile_Import_WavefrontOBJClick)
			EVT_MENU(ID_EDITOR_MENUBAR_FILE_IMPORT_STL, EditorMenubar::_OnFile_Import_STLClick)

			EVT_MENU(ID_EDITOR_MENUBAR_ADD_SKYBOX, EditorMenubar::_OnAdd_SkyboxClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_NODE, EditorMenubar::_OnAdd_NodeClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_MESH_CUBE, EditorMenubar::_OnAdd_Mesh_CubeClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_MESH_SPHERE, EditorMenubar::_OnAdd_Mesh_SphereClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_MESH_CAPSULE, EditorMenubar::_OnAdd_Mesh_CapsuleClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_LIGHT_DIRECTIONAL_LIGHT, EditorMenubar::_OnAdd_Light_DirectionalLightClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_LIGHT_POINT_LIGHT, EditorMenubar::_OnAdd_Light_PointLightClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_LIGHT_AMBIENT_LIGHT, EditorMenubar::_OnAdd_Light_AmbientLightClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_CAMERA_FIRST_PERSON_CAMERA, EditorMenubar::_OnAdd_Camera_FirstPersonCameraClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_CAMERA_THIRD_PERSON_CAMERA, EditorMenubar::_OnAdd_Camera_ThirdPersonCameraClick)
			EVT_MENU(ID_EDITOR_MENUBAR_ADD_CAMERA_EDITOR_CAMERA, EditorMenubar::_OnAdd_Camera_EditorCameraClick)

			EVT_MENU(ID_EDITOR_MENUBAR_VIEW_GRID, EditorMenubar::_OnView_GridCheck)
		wxEND_EVENT_TABLE()

		EditorMenubar::EditorMenubar() : wxMenuBar() {
			m_file = new wxMenu;
			m_file->Append(ID_EDITOR_MENUBAR_FILE_NEW, wxT("&New"), "Create a new deng project");
			m_file->Append(ID_EDITOR_MENUBAR_FILE_OPEN, wxT("&Open"), "Open existing project");
			m_file->Append(ID_EDITOR_MENUBAR_FILE_SAVE, wxT("&Save"), "Save the project");

			m_file_import = new wxMenu;
			m_file_import->Append(ID_EDITOR_MENUBAR_FILE_IMPORT_DAS, wxT("&DAS"), "Import a DENG native das model");
			m_file_import->Append(ID_EDITOR_MENUBAR_FILE_IMPORT_GLTF, wxT("&GLTF"), "Import a GLTF model");
			m_file_import->Append(ID_EDITOR_MENUBAR_FILE_IMPORT_OBJ, wxT("&OBJ"), "Import a Wavefront OBJ model");
			m_file_import->Append(ID_EDITOR_MENUBAR_FILE_IMPORT_STL, wxT("&STL"), "Import an STL model");

			m_file->Append(ID_EDITOR_MENUBAR_FILE_IMPORT, wxT("&Import"), m_file_import, "Import a 3D model");
			m_file->Append(wxID_EXIT, wxT("&Quit"), "Quit the application");
			Append(m_file, wxT("&File"));

			m_view = new wxMenu;
			wxMenuItem* view_grid = new wxMenuItem(m_view, ID_EDITOR_MENUBAR_VIEW_GRID, wxT("&Grid"), wxT("Toggle grid in editor"), wxITEM_CHECK);
			m_view->Append(view_grid);
			view_grid->Check(true);
			Append(m_view, wxT("&View"));

			m_add = new wxMenu;
			m_add->Append(ID_EDITOR_MENUBAR_ADD_SKYBOX, wxT("&Skybox"), "Add a skybox to the scene");
			m_add->Append(ID_EDITOR_MENUBAR_ADD_NODE, wxT("&Node"), "Add a new node to the scene");
		
			m_add_mesh = new wxMenu;
			m_add_mesh->Append(ID_EDITOR_MENUBAR_ADD_MESH_SPHERE, wxT("&Sphere"), "Add a sphere");
			m_add_mesh->Append(ID_EDITOR_MENUBAR_ADD_MESH_CUBE, wxT("&Cube"), "Add a cube");
			m_add_mesh->Append(ID_EDITOR_MENUBAR_ADD_MESH_CAPSULE, wxT("&Capsule"), "Add a capsule");
			m_add->Append(ID_EDITOR_MENUBAR_ADD_MESH, wxT("&Mesh"), m_add_mesh, "Add a mesh");
			
			m_add_light = new wxMenu;
			m_add_light->Append(ID_EDITOR_MENUBAR_ADD_LIGHT_DIRECTIONAL_LIGHT, wxT("&Directional light"), "Add a directional light");
			m_add_light->Append(ID_EDITOR_MENUBAR_ADD_LIGHT_POINT_LIGHT, wxT("&Point light"), "Add a point light");
			m_add_light->Append(ID_EDITOR_MENUBAR_ADD_LIGHT_AMBIENT_LIGHT, wxT("&Ambient light"), "Add an ambient light");
			m_add->Append(ID_EDITOR_MENUBAR_ADD_LIGHT, wxT("&Light"), m_add_light, "Add a light source");

			m_add_camera = new wxMenu;
			m_add_camera->Append(ID_EDITOR_MENUBAR_ADD_CAMERA_FIRST_PERSON_CAMERA, wxT("&First person camera"), "Add a first person camera");
			m_add_camera->Append(ID_EDITOR_MENUBAR_ADD_CAMERA_THIRD_PERSON_CAMERA, wxT("&Third person camera"), "Add a third person camera");
			m_add_camera->Append(ID_EDITOR_MENUBAR_ADD_CAMERA_EDITOR_CAMERA, wxT("&Editor camera"), "Add an editor camera");
			m_add->Append(ID_EDITOR_MENUBAR_ADD_CAMERA, wxT("&Camera"), m_add_camera, "Add a camera");
			Append(m_add, wxT("&Add"));
		}


		void EditorMenubar::_OnFile_NewClick(wxCommandEvent &_evt) {
			std::cout << "[EditorMenubar] 0x" << reinterpret_cast<void*>(GetParent()) << std::endl;
		}

		void EditorMenubar::_OnFile_OpenClick(wxCommandEvent& _evt) {
		}

		void EditorMenubar::_OnFile_SaveClick(wxCommandEvent& _evt) {
		}

		void EditorMenubar::_OnFile_Import_DASClick(wxCommandEvent& _evt) {
			wxFileDialog dialog = wxFileDialog(GetParent(), "Choose a das file", wxEmptyString, wxEmptyString, "DAS model | *.das");
			if (dialog.ShowModal() == wxID_OK) {
				GameEditor* editor = (GameEditor*)GetParent();
				auto path = dialog.GetPath().ToStdString();
				Libdas::DasParser parser(path);
				parser.Parse();

				editor->GetModelLoaders().emplace_back(nullptr, parser.GetModel(), *editor->GetViewport()->GetRenderer(), editor->GetCamera()->GetId());
				Registry* reg = Registry::GetInstance();
				reg->AttachEntities();
				editor->AppendModelToHierarchy();
				editor->GetViewport()->GetRenderer()->LoadShaders();
			}
		}

		void EditorMenubar::_OnFile_Import_GLTFClick(wxCommandEvent& _evt) {
		}

		void EditorMenubar::_OnFile_Import_WavefrontOBJClick(wxCommandEvent& _evt) {
		}

		void EditorMenubar::_OnFile_Import_STLClick(wxCommandEvent& _evt) {
		}

		void EditorMenubar::_OnAdd_SkyboxClick(wxCommandEvent& _evt) {
			SkyboxGeneratorDialog dialog(GetParent());

			GameEditor* editor = (GameEditor*)GetParent();
			if (dialog.ShowModal() == wxID_OK) {
				const auto& tx = dialog.GetTextures();
				editor->SetSkybox(new Skybox(nullptr, *editor->GetViewport()->GetRenderer(),
											 editor->GetCamera()->GetId(),
											 tx[3], tx[1], tx[0], tx[4], tx[5], tx[2]));
				Registry* reg = Registry::GetInstance();
				reg->AttachEntities();
				editor->GetSkybox()->SetScale(dialog.GetScale());
				editor->GetViewport()->GetRenderer()->LoadShaders();
			}
		}

		void EditorMenubar::_OnAdd_NodeClick(wxCommandEvent& _evt) {
		}

		void EditorMenubar::_OnAdd_Mesh_CubeClick(wxCommandEvent& _evt) {
			CubeGeneratorDialog dialog(GetParent());
			
			GameEditor* editor = (GameEditor*)GetParent();
			if (dialog.ShowModal() == wxID_OK) {
				CubeGenerator gen = dialog.ConstructCube();
				editor->GetModelLoaders().emplace_back(std::move(gen.ToModelLoader(nullptr, *editor->GetViewport()->GetRenderer(), 
																 editor->GetCamera()->GetId())));
				Registry* reg = Registry::GetInstance();
				reg->AttachEntities();
				editor->AppendModelToHierarchy();
				editor->GetViewport()->GetRenderer()->LoadShaders();
			}
		}

		void EditorMenubar::_OnAdd_Mesh_SphereClick(wxCommandEvent& _evt) {
			SphereGeneratorDialog dialog(GetParent());
			
			GameEditor* editor = (GameEditor*)GetParent();
			if (dialog.ShowModal() == wxID_OK) {
				SphereGenerator gen(std::move(dialog.ConstructSphere()));
				editor->GetModelLoaders().emplace_back(std::move(gen.ToModelLoader(nullptr, *editor->GetViewport()->GetRenderer(), 
																 editor->GetCamera()->GetId())));
				Registry* reg = Registry::GetInstance();
				reg->AttachEntities();
				editor->AppendModelToHierarchy();
				editor->GetViewport()->GetRenderer()->LoadShaders();
			}
		}

		void EditorMenubar::_OnAdd_Mesh_CapsuleClick(wxCommandEvent& _evt) {
		}

		void EditorMenubar::_OnAdd_Light_DirectionalLightClick(wxCommandEvent& _evt) {
		}

		void EditorMenubar::_OnAdd_Light_PointLightClick(wxCommandEvent& _evt) {
		}

		void EditorMenubar::_OnAdd_Light_AmbientLightClick(wxCommandEvent& _evt) {
		}

		void EditorMenubar::_OnAdd_Camera_FirstPersonCameraClick(wxCommandEvent& _evt) {
		}

		void EditorMenubar::_OnAdd_Camera_ThirdPersonCameraClick(wxCommandEvent& _evt) {
		}

		void EditorMenubar::_OnAdd_Camera_EditorCameraClick(wxCommandEvent& _evt) {
		}

		void EditorMenubar::_OnView_GridCheck(wxCommandEvent& _evt) {
			Registry* reg = Registry::GetInstance();
			GridGenerator* grid = (GridGenerator*)reg->GetFirstEntityByType(ENTITY_TYPE_GRID_MESH);

			if (grid) {
				grid->Show(_evt.IsChecked());
			} else {
				wxMessageBox("Could not find ENTITY_TYPE_GRID_MESH entity", "Error", wxICON_ERROR | wxOK, GetParent());
			}
		}

	}
}