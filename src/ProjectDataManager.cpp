// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProjectDataManager.h - Project data management abstraction class implementation
// author: Karl-Mihkel Ott

#define PROJECT_DATA_MANAGER_CPP
#include "deng/ProjectDataManager.h"

namespace DENG {

	ProjectDataManager::ProjectDataManager(const std::string& _path, const std::string& _name, DXML::Configuration::Backend _backend) :
		m_project_path(_path)
	{
		m_xml_game.meta.name = _name;
		m_xml_game.cfg.default_backend = _backend;
	}


	bool ProjectDataManager::CreateEmptyProject() {
		DENG_ASSERT(m_project_path.size());

		// check if the project path exists and if it doesn't create it
		if (!std::filesystem::exists(m_project_path)) {
			std::filesystem::create_directories(m_project_path);

			// create directories for sfx, models, models/static, models/dynamic, scripts
			std::filesystem::create_directory(m_project_path + "/sfx");
			std::filesystem::create_directory(m_project_path + "/models");
			std::filesystem::create_directory(m_project_path + "/models/static");
			std::filesystem::create_directory(m_project_path + "/models/dynamic");
			std::filesystem::create_directory(m_project_path + "/scripts");
			std::filesystem::create_directory(m_project_path + "/materials");

			const std::string parent = std::filesystem::path(m_project_path).filename().u8string();
			m_xml_game.cfg.runtime_executable = parent;
#ifdef _WIN32
			std::filesystem::copy_file(Libdas::Algorithm::GetProgramPath() + "\\deng-complete.dll", m_project_path + "\\deng-complete.dll");
			std::filesystem::copy_file(Libdas::Algorithm::GetProgramPath() + "\\rt.exe", m_project_path + "\\" + parent + ".exe");
			std::filesystem::copy_file(Libdas::Algorithm::GetProgramPath() + "\\libcrypto-1_1.dll", m_project_path + "\\libcrypto-1_1.dll");
			std::filesystem::copy_file(Libdas::Algorithm::GetProgramPath() + "\\libssl-1_1.dll", m_project_path + "\\libssl-1_1.dll");
			std::filesystem::copy_file(Libdas::Algorithm::GetProgramPath() + "\\libffi-7.dll", m_project_path + "\\libffi-7.dll");
			std::filesystem::copy(Libdas::Algorithm::GetProgramPath() + "\\python3.10", m_project_path + "\\python3.10", std::filesystem::copy_options::recursive);
#ifdef __DEBUG
			std::filesystem::copy_file(Libdas::Algorithm::GetProgramPath() + "\\python310_d.dll", m_project_path + "\\python310_d.dll");
#else
			std::filesystem::copy_file(Libdas::Algorithm::GetProgramPath() + "\\python310.dll", m_project_path + "\\python310.dll");
#endif
#else
#error "Project creation is not implemented in GNU/Linux"
#endif
			DXML::GameConfigurationWriter cfg_writer(m_project_path + "/game.xml");
			cfg_writer.Write(m_xml_game);
			return true;
		}

		return false;
	}


	bool ProjectDataManager::LoadProject(const std::string &_game_xml_path) {
		DXML::GameConfigurationReader reader(_game_xml_path);
		reader.Parse();
		m_xml_game = reader.GetXMLGame();
		return true;
	}
}