// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: RuntimeExecutable.cpp - Main runtime binary implementation file
// author: Karl-Mihkel Ott

#define RUNTIME_EXECUTABLE_CPP
#include "deng/Executables/rt/RuntimeExecutable.h"

std::string GetProgramPath(bool _no_filename) {
	std::vector<char> path_buf;
	DWORD copied = 0;

	do {
		path_buf.resize(path_buf.size()+_MAX_PATH);
		copied = GetModuleFileNameA(NULL, path_buf.data(), static_cast<DWORD>(path_buf.size()));
	} while (copied == path_buf.size());

	std::string path;
	for (auto c : path_buf) {
		if (c != 0)
			path += c;
		else break;
	}

	if (!_no_filename) {
		for (int i = (int)path.size() - 1; i >= 0; i--) {
			char c = path[i];
			path.resize(path.size() - 1);

			if (c == '\\')
				break;
		}
	}

	return path;
}


void SetupSysPath() {
	py::module_ sys = py::module_::import("sys");
	py::print(sys.attr("path"));

	std::string root = GetProgramPath();

	py::list list;
	list.append(root + "\\Python\\DLLs");
	list.append(root + "\\Python\\Lib");
	list.append(root + "\\Python");
	list.append(root);

	sys.attr("path") = list;
}


// Runtime class
Runtime::Runtime() {
	m_data = new DENG::ProjectDataManager(GetProgramPath(), true);

	auto& conf = m_data->GetConfiguration();
	auto& meta = m_data->GetMetadata();

	// check executable integrity
	const std::filesystem::path pth = GetProgramPath(true);
	if (pth.filename().u8string() != conf.executable) {
		MessageBoxA(NULL, "Error game datafile executable name does not match the real executable name", "Error", MB_ICONERROR | MB_OK);
		std::exit(0);
	}

	// create the renderer
	DENG::RendererConfig rend_config;
	rend_config.title = meta.name;
	rend_config.clear_color = { 0.0f, 0.f, 0.f, 1.f };
	rend_config.enable_vsync = false;

	m_window = new DENG::Window(
		(int32_t)conf.window_size[0],
		(int32_t)conf.window_size[1],
		NEKO_HINT_RESIZEABLE,
		meta.name.c_str());

	rend_config.win32_hwnd = m_window->GetWin32HWND();
	rend_config.win32_instance = m_window->GetWin32Instance();

	// create a renderer instance with specified backend
	switch (conf.graphicsBackend) {
		case DXML::GRAPHICS_BACKEND_OPENGL:
			DENG::OpenGL::Initialise(*m_window);
			m_renderer = new DENG::OpenGLRenderer(rend_config);
			break;

		case DXML::GRAPHICS_BACKEND_VULKAN:
			DENG::Vulkan::Initialise();
			m_renderer = new DENG::VulkanRenderer(rend_config);
			break;

		default:
			MessageBoxA(rend_config.win32_hwnd, "Invalid graphics backend in data file", "Error", MB_ICONERROR | MB_OK);
			std::exit(0);
			break;
	}

	_LoadLevel(conf.level);
}

Runtime::~Runtime() {
	delete m_renderer;
	delete m_window;
	delete m_data;

	// delete level data
	delete m_level.player;
	delete m_level.skybox;
}


void Runtime::_LoadLevel(uint32_t _id) {
	// check if level id is correct
	if (_id >= (uint32_t)m_data->GetLevelCount()) {
		const std::string msg = "Invalid level with id " + std::to_string(_id);
		MessageBoxA(m_window->GetWin32HWND(), msg.c_str(), "Error", MB_ICONERROR | MB_OK);
		std::exit(0);
	}

	// delete all previous level data
	delete m_level.player;
	delete m_level.skybox;

	m_level.player = nullptr;
	m_level.skybox = nullptr;
	while (!m_level.cameras.empty())
		m_level.cameras.pop_back();
	while (!m_level.models.empty())
		m_level.cameras.pop_back();

	m_level.id = _id;

	Libdas::DasParser parser(m_data->GetLevels()[m_level.id].player.source);
	parser.Parse();

	// implement player model loading here ...
	//for (auto it = m_data->GetLevels()[_id].rigidBodies.begin(); it != m_data->GetLevels()[_id].rigidBodies.end(); it++) {
	//	m_level.models.emplace_back();
	//}
}


int main(void) {
	py::scoped_interpreter guard{};
	SetupSysPath();


	// now project data manager should be fully loaded
	// we can proceed to loading the game
	Runtime rt;
	//rt.Run();

	return 0;
}