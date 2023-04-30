// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProgramFilesManager.h - program file data manager class header
// author: Karl-Mihkel Ott

#ifndef PROGRAM_FILES_MANAGER_H
#define PROGRAM_FILES_MANAGER_H

#ifdef PROGRAM_FILES_MANAGER_CPP
	#include <string>
	#include <vector>
	#include <filesystem>
	#include <fstream>

	#include <Windows.h>
#endif

namespace DENG {

	class ProgramFilesManager {
		private:
			std::string m_sParentDirectory;

		public:
			ProgramFilesManager();
			std::vector<char> GetProgramFileContent(const std::string &_sPath);
			void WriteProgramFile(const std::vector<char>& _bytes, const std::string& _sFilePath);
			void WriteProgramFile(const char* _pBytes, size_t _uByteCount, const std::string& _sFilePath);
	};
}

#endif