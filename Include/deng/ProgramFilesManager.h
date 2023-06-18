// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProgramFilesManager.h - program file data manager class header
// author: Karl-Mihkel Ott

#ifndef PROGRAM_FILES_MANAGER_H
#define PROGRAM_FILES_MANAGER_H

#include <string>
#include <cstdint>
#include <vector>

#ifdef PROGRAM_FILES_MANAGER_CPP
	#include <filesystem>
	#include <fstream>

	#include <Windows.h>

	#include "deng/Exceptions.h"
#endif

namespace DENG {

	class ProgramFilesManager {
		private:
			std::string m_sParentDirectory;

		public:
			ProgramFilesManager();
			bool ExistsFile(const std::string& _sPath) const;
			size_t FileSize(const std::string& _sPath) const;
			time_t GetFileTimestamp(const std::string& _sPath) const;
			std::vector<char> GetProgramFileContent(const std::string &_sPath) const;
			void WriteProgramFile(const std::vector<char>& _bytes, const std::string& _sFilePath) const;
			void WriteProgramFile(const char* _pBytes, size_t _uByteCount, const std::string& _sFilePath) const;
	};
}

#endif