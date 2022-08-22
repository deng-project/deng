// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GameEditor.cpp - Main game editor gui implementation
// author: Karl-Mihkel Ott

#define GAME_EDITOR_CPP
#include "deng/Executables/deng-game-editor/GameEditor.h"

const static float g_verts[] = {
	-0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
};


const static uint32_t g_indices[] = { 0, 1, 2 };

static const char *g_vertex_shader =
	"#version 450\n"\
	"#extension GL_ARB_separate_shader_objects : enable\n"\
	"layout(location=0) in vec3 in_pos;\n"\
	"void main() {\n"\
	"	gl_Position = vec4(in_pos, 1.0f);\n"\
	"}";

static const char* g_frag_shader =
	"#version 450\n"\
	"#extension GL_ARB_separate_shader_objects : enable\n"\
	"layout(location=0) out vec4 out_color;\n"\
	"void main() {\n"\
	"	out_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"\
	"}";

namespace DENG {
	namespace Editor {

		GameEditor::GameEditor() :
			wxFrame(NULL, wxID_ANY, "Untitled project", wxDefaultPosition, wxSize(1880, 1020)),
			m_mgr(this, wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_RECTANGLE_HINT)
		{
			wxTextCtrl *hierarchy = new wxTextCtrl(this, wxID_ANY, "Hierarchy panel", wxDefaultPosition, wxSize(200, 150), wxNO_BORDER | wxTE_MULTILINE);
			RendererViewport* viewport = new RendererViewport(this, DXML::Configuration::Backend::VULKAN);
			wxTextCtrl *assets = new wxTextCtrl(this, wxID_ANY, "Assets panel", wxDefaultPosition, wxSize(200, 150), wxNO_BORDER | wxTE_MULTILINE);
			wxTextCtrl* inspector = new wxTextCtrl(this, wxID_ANY, "Inspector panel", wxDefaultPosition, wxSize(200, 150), wxNO_BORDER | wxTE_MULTILINE);

			m_mgr.AddPane(hierarchy, wxLEFT, "Scene hierarchy");
			m_mgr.AddPane(viewport, wxCENTER, "Viewport");
			m_mgr.AddPane(assets, wxBOTTOM, "Assets");
			m_mgr.AddPane(inspector, wxRIGHT, "Inspector");

			m_mgr.Update();
			viewport->Setup();

			// DENG::Renderer *rend = viewport->GetRenderer();
			// rend->LoadShaders();

		}


		GameEditor::~GameEditor() {
			m_mgr.UnInit();
		}
	}
}