// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProgramFilesManager.h - program file data manager class header
// author: Karl-Mihkel Ott

#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <fstream>
#include <filesystem>

#ifdef PROGRAM_FILES_MANAGER_CPP
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
			std::vector<char> GetProgramFileContent(const std::string& _sPath) const;

			template <typename T>
			std::vector<T> GetProgramFileContentAs(const std::string& _sPath)
			{
				std::vector<T> output;
				const std::string sAbsolutePath = m_sParentDirectory + '\\' + _sPath;

				if (std::filesystem::exists(sAbsolutePath) && std::filesystem::is_regular_file(sAbsolutePath))
				{
					std::ifstream stream(sAbsolutePath, std::ios_base::binary);
					stream.seekg(0, std::ios_base::end);
					size_t uFileSize = stream.tellg();
					stream.seekg(0, std::ios_base::beg);

					// check if file size is multiple of sizeof(T)
					if (uFileSize % sizeof(T) != 0)
					{
						throw IOException("GetProgramFileContent<T>(): Read file size must be multiple of " + std::to_string(sizeof(T)));
					}

					output.resize(uFileSize / sizeof(T));
					stream.read(reinterpret_cast<char*>(output.data()), uFileSize);
					stream.close();
				}

				return output;
			}

			void WriteProgramFile(const std::vector<char>& _bytes, const std::string& _sFilePath) const;
			void WriteProgramFile(const char* _pBytes, size_t _uByteCount, const std::string& _sFilePath) const;
	};
}