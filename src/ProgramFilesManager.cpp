// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProgramFilesManager.cpp - program file data manager class implementation
// author: Karl-Mihkel Ott

#define PROGRAM_FILES_MANAGER_CPP
#include "deng/ProgramFilesManager.h"

using namespace std;

namespace DENG {
	
	ProgramFilesManager::ProgramFilesManager() {
		wstring sFilePath;
		sFilePath.reserve(256);

		DWORD result = 0x7a; // ERROR_INSUFFICIENT_BUFFER
		while (result == 0x7a) {
			sFilePath.reserve(sFilePath.capacity() + 256);
			GetModuleFileNameW(NULL, sFilePath.data(), sFilePath.size());
			result = GetLastError();
		}

		m_sParentDirectory = filesystem::path(sFilePath).parent_path().u8string();

#if !defined(DENG_WINDOWS_PACKAGE_CONFIGURATION) && !defined(DENG_PORTABLE)
		m_sParentDirectory += "..\\..";
#endif
	}

	vector<char> ProgramFilesManager::GetProgramFileContent(const string& _path) {
		vector<char> output;
		const std::string sAbsolutePath = m_sParentDirectory + '\\' + _path;

		if (filesystem::exists(sAbsolutePath) && filesystem::is_regular_file(sAbsolutePath)) {
			ifstream stream(sAbsolutePath, ios_base::binary);
			
			stream.seekg(0, ios_base::end);
			size_t uFileSize = stream.tellg();
			stream.seekg(0, ios_base::beg);

			output.resize(uFileSize);
			stream.read(output.data(), uFileSize);
			stream.close();
		}

		return output;
	}


	void ProgramFilesManager::WriteProgramFile(const vector<char>& _data, const string& _sFilePath) {
		const string sAbsolutePath = m_sParentDirectory + '\\' + _sFilePath;

		ofstream stream(sAbsolutePath, ios_base::binary);
		stream.write(_data.data(), _data.size());
		stream.close();
	}


	void ProgramFilesManager::WriteProgramFile(const char* _pBytes, size_t _uByteCount, const string& _sFilePath) {
		const string sAbsolutePath = m_sParentDirectory + '\\' + _sFilePath;

		ofstream stream(sAbsolutePath, ios_base::binary);
		stream.write(_pBytes, _uByteCount);
		stream.close();
	}
}