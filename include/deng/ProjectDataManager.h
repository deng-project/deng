// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProjectDataManager.h - Project data management abstraction class header
// author: Karl-Mihkel Ott

#ifndef PROJECT_DATA_MANAGER_H
#define PROJECT_DATA_MANAGER_H

#ifdef PROJECT_DATA_MANAGER_CPP
	#include <string>
	#include <vector>
	#include <fstream>
	#include <stack>
	#include <array>
	#include <filesystem>
	#include <unordered_map>
    #include <cmath>
	#include <queue>

#ifdef _WIN32
	#include <Windows.h>
#endif

	#include "trs/Vector.h"
	#include "trs/Points.h"
	#include "trs/Quaternion.h"
	#include "trs/Matrix.h"

	#include <python3.10/pyconfig.h>
	#include <pybind11/pybind11.h>
	namespace py = pybind11;

	#include "mar/AsciiStreamReader.h"
	#include "mar/AsciiLineReader.h"
	
	#include "dxml/GameStructs.h"
	#include "dxml/XMLTag.h"
	#include "dxml/XMLWriter.h"
	#include "dxml/XMLParser.h"
	#include "dxml/GameXMLReader.h"

	#include "das/Api.h"
	#include "das/Algorithm.h"
	#include "das/DasStructures.h"
	#include "das/TextureReader.h"

	#include "deng/Api.h"
	#include "deng/ErrorDefinitions.h"
	#include "deng/ShaderDefinitions.h"
	#include "deng/Renderer.h"
#endif

namespace DENG {

	class DENG_API ProjectDataManager {
		private:
			DXML::SerializedGame m_game;
			bool m_save_bit = false;
			const bool m_read_bit;

			// filename and parent directory to consider
			const std::string m_file_name;
			const std::string m_parent_dir;

		private:
			void _CreateNewProject();

		public:
			ProjectDataManager(const std::string &_file_name, const std::string& _parent_dir = ".", bool _read_bit = true);
			ProjectDataManager(const ProjectDataManager& _pdm) = default;

			inline void SetMetadata(const DXML::GameMetadata& _meta) {
				m_game.metadata = _meta;
			}

			inline void SetConfiguration(const DXML::GameConfiguration& _conf) {
				m_game.config = _conf;
			}

			inline uint32_t AddLevel() {
				m_game.levels.emplace_back();
				return static_cast<uint32_t>(m_game.levels.size()-1);
			}

			inline void SetLevelDescriptor(uint32_t _id, DXML::LevelDescriptor& _desc) {
				DENG_ASSERT(_id < m_game.levels.size());
				m_game.levels[_id] = _desc;
			}

			inline void SetEntryScript(const std::string& _script) {
				m_game.entryScript = _script;
			}

			inline const DXML::GameMetadata& GetMetadata() const {
				return m_game.metadata;
			}

			inline const DXML::GameConfiguration& GetConfiguration() const {
				return m_game.config;
			}

			inline const size_t GetLevelCount() const {
				return m_game.levels.size();
			}

			inline const std::vector<DXML::LevelDescriptor>& GetLevels() const {
				return m_game.levels;
			}

			void Load();
			void Save();

			inline bool GetSaveBit() {
				return m_save_bit;
			}
	};
}

#endif
