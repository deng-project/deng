// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProjectDataManager.h - Project data management abstraction class header
// author: Karl-Mihkel Ott

#ifndef PROJECT_DATA_MANAGER_H
#define PROJECT_DATA_MANAGER_H

#ifdef PROJECT_DATA_MANAGER_CPP
	#include <string>
	#include <vector>
	#include <unordered_map>
    #include <cmath>

	#include "trs/Vector.h"
	#include "trs/Points.h"
	#include "trs/Quaternion.h"
	#include "dxml/GameStructs.h"

	#include "deng/Api.h"
#endif

namespace DENG {

	class DENG_API ProjectDataManager {
		private:
			std::string m_project_path;
			DXML::XMLGame m_xml_game;

		public:
			ProjectDataManager() = default;
			ProjectDataManager(const std::string &_path, const std::string &_name);
			ProjectDataManager(const ProjectDataManager &_pd) = default;
			ProjectDataManager(ProjectDataManager &&_pd) = default;

			void CreateEmptyProject(const std::string &_path = "", const std::string &_name = "");
			void LoadProject(const std::string &_game_xml_path);
			void Move(const std::string& _new_path);

			inline void SetProjectPath(const std::string &_path) {
				m_project_path = _path;
			}

			inline void SetProjectName(const std::string& _name) {
				m_xml_game.meta.name = _name;
			}

			inline const std::string& GetProjectPath() const {
				return m_project_path;
			}

			inline DXML::XMLGame& GetXMLGame() {
				return m_xml_game;
			}
	};
}

#endif
