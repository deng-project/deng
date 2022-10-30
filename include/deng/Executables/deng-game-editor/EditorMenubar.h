// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: EditorMenubar.h - DENG game editor menubar class header
// author: Karl-Mihkel Ott

#ifndef EDITOR_MENUBAR_H
#define EDITOR_MENUBAR_H

#ifdef EDITOR_MENUBAR_CPP
	#include <wx/wx.h>
	#include <wx/valnum.h>
	#include <wx/aui/aui.h>
	#include <wx/treectrl.h>
	#include "deng/deng.h"

	#include "deng/Executables/deng-game-editor/GuiIds.h"
	#include "deng/Executables/deng-game-editor/OpenGLLoader.h"
	#include "deng/Executables/deng-game-editor/RendererViewport.h"
	#include "deng/Executables/deng-game-editor/CubeGeneratorDialog.h"
	#include "deng/Executables/deng-game-editor/SphereGeneratorDialog.h"
	#include "deng/Executables/deng-game-editor/SkyboxGeneratorDialog.h"
	#include "deng/Executables/deng-game-editor/GameEditor.h"
#endif

namespace DENG {
	namespace Editor {

		class EditorMenubar : public wxMenuBar {
			private:
				wxMenu* m_file = nullptr;
				wxMenu* m_file_import = nullptr;

				wxMenu* m_view = nullptr;
			
				wxMenu* m_add = nullptr;
				wxMenu* m_add_mesh = nullptr;
				wxMenu* m_add_light = nullptr;
				wxMenu* m_add_camera = nullptr;


			private:
				void _OnFile_NewClick(wxCommandEvent& _evt);
				void _OnFile_OpenClick(wxCommandEvent& _evt);
				void _OnFile_SaveClick(wxCommandEvent& _evt);
				void _OnFile_Import_DASClick(wxCommandEvent& _evt);
				void _OnFile_Import_GLTFClick(wxCommandEvent& _evt);
				void _OnFile_Import_WavefrontOBJClick(wxCommandEvent& _evt);
				void _OnFile_Import_STLClick(wxCommandEvent& _evt);

				void _OnAdd_SkyboxClick(wxCommandEvent& _evt);
				void _OnAdd_NodeClick(wxCommandEvent& _evt);
				void _OnAdd_Mesh_CubeClick(wxCommandEvent& _evt);
				void _OnAdd_Mesh_SphereClick(wxCommandEvent& _evt);
				void _OnAdd_Mesh_CapsuleClick(wxCommandEvent& _evt);
				void _OnAdd_Light_DirectionalLightClick(wxCommandEvent& _evt);
				void _OnAdd_Light_PointLightClick(wxCommandEvent& _evt);
				void _OnAdd_Light_AmbientLightClick(wxCommandEvent& _evt);
				void _OnAdd_Camera_FirstPersonCameraClick(wxCommandEvent& _evt);
				void _OnAdd_Camera_ThirdPersonCameraClick(wxCommandEvent& _evt);
				void _OnAdd_Camera_EditorCameraClick(wxCommandEvent& _evt);

				void _OnView_GridCheck(wxCommandEvent& _evt);

				wxDECLARE_EVENT_TABLE();

			public:
				EditorMenubar();
		};
	}
}

#endif