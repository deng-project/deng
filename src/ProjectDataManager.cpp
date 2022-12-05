// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProjectDataManager.h - Project data management abstraction class implementation
// author: Karl-Mihkel Ott

#define PROJECT_DATA_MANAGER_CPP
#include "deng/ProjectDataManager.h"

namespace DENG {
	
	ProjectDataManager::ProjectDataManager(const std::string& _file_name, const std::string& _parent_dir, bool _read_bit) :
		m_read_bit(_read_bit),
		m_file_name(_file_name),
		m_parent_dir(_parent_dir)
	{
		// should the game be read
		if (_read_bit) {
			Load();
		} else {
			_CreateNewProject();
		}
	}


	void ProjectDataManager::_CreateNewProject() {
		// create the parent directory
		if (!std::filesystem::exists(m_parent_dir))
			std::filesystem::create_directories(std::filesystem::path(m_parent_dir));

		// Game\Assets
		std::filesystem::create_directory(std::filesystem::path(m_parent_dir + "\\Assets"));
		// Game\Scripts
		std::filesystem::create_directory(std::filesystem::path(m_parent_dir + "\\Scripts"));
		// Game\Shaders
		std::filesystem::create_directory(std::filesystem::path(m_parent_dir + "\\Shaders"));
		// Game\SoundEffects
		std::filesystem::create_directory(std::filesystem::path(m_parent_dir + "\\SoundEffects"));
		// Game\Textures
		std::filesystem::create_directory(std::filesystem::path(m_parent_dir + "\\Textures"));
		// Game\Videos
		std::filesystem::create_directory(std::filesystem::path(m_parent_dir + "\\Videos"));
		// Game\Python
		std::filesystem::create_directory(std::filesystem::path(m_parent_dir + "\\Python"));

		// Game\deng-complete.dll
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\deng-complete.dll"),
			std::filesystem::path(m_parent_dir + "\\deng-complete.dll"));
		
		// Game\drt.exe
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\drt.exe"),
			std::filesystem::path(m_parent_dir + "\\drt.exe"));

#ifdef _DEBUG
		// Game\python310_d.dll
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\python310_d.dll"),
			std::filesystem::path(m_parent_dir + "\\python310_d.dll"));
#else
		// Game\python310.dll
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\python310.dll"),
			std::filesystem::path(m_parent_dir + "\\python310.dll"));
#endif

		// Game\vcruntime140.dll
		std::filesystem::copy_file(
			std::filesystem::path("C:\\Windows\\System32\\vcruntime140.dll"),
			std::filesystem::path(m_parent_dir + "\\vcruntime140.dll"));
	}


	void ProjectDataManager::Load() {
		// check if the file is in XML format
		std::filesystem::path pth(m_parent_dir + "/" + m_file_name);
		if (pth.extension() == ".xml") {
			DXML::GameXMLReader reader(m_parent_dir + "/" + m_file_name);
			reader.Parse();

			// throw an error if there were any errors present
			if (reader.ErrorStackSize()) {
				std::string errmsg;
				while (reader.ErrorStackSize()) {
					errmsg += reader.PopErrorStack() + "\n";
				}

				throw std::runtime_error(errmsg);
			}

			m_game = reader.GetSerializedGame();
		}
		else if (pth.extension() == ".dat") {
			std::ifstream stream(m_parent_dir + "/" + m_file_name);

			if (!stream.is_open()) {
				throw std::runtime_error("Failed to open file " + m_parent_dir + "/" + m_file_name);
			}

			m_game.Unserialize(stream);
			stream.close();
		}
		else DENG_ASSERT(false);
	}
}