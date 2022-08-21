// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProjectDataManager.h - Project data management abstraction class implementation
// author: Karl-Mihkel Ott

#define PROJECT_DATA_MANAGER_CPP
#include "deng/ProjectDataManager.h"

namespace DENG {

	ProjectDataManager::ProjectDataManager(const std::string& _path, const std::string& _name) :
		m_project_path(_path)
	{
		m_xml_game.meta.name = _name;
	}


	void ProjectDataManager::CreateEmptyProject(const std::string &_path, const std::string &_name) {
		
	}
}