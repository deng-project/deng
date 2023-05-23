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
			GetModuleFileNameW(NULL, sFilePath.data(), static_cast<DWORD>(sFilePath.capacity()));
			result = GetLastError();
		}

		m_sParentDirectory = filesystem::path(sFilePath).parent_path().u8string();

#if !defined(DENG_WINDOWS_PACKAGE_CONFIGURATION) && !defined(DENG_PORTABLE) && !defined(_DEBUG)
		m_sParentDirectory += "..\\..";
#elif defined(_DEBUG)
		m_sParentDirectory += "..";
#endif
	}

	bool ProgramFilesManager::ExistsFile(const string& _sPath) {
		const string sAbsolutePath = m_sParentDirectory + '\\' + _sPath;
		return filesystem::exists(sAbsolutePath) && filesystem::is_regular_file(sAbsolutePath);
	}


	size_t ProgramFilesManager::FileSize(const string& _sPath) {
		const string sAbsolutePath = m_sParentDirectory + '\\' + _sPath;
		size_t size = 0; 
		try {
			size = static_cast<size_t>(filesystem::file_size(sAbsolutePath));
		}
		catch (const filesystem::filesystem_error& e) {
			throw IOException(e.what());
			return 0;
		}

		return size;
	}


	time_t ProgramFilesManager::GetFileTimestamp(const string& _sPath) {
		const string sAbsolutePath = m_sParentDirectory + '\\' + _sPath;
		filesystem::file_time_type tTimestamp; 
		
		try {
			tTimestamp = filesystem::last_write_time(sAbsolutePath);
		}
		catch (const filesystem::filesystem_error& e) {
			throw IOException(e.what());
			return 0;
		}

		return tTimestamp.time_since_epoch().count();
	}


	vector<char> ProgramFilesManager::GetProgramFileContent(const string& _sPath) {
		vector<char> output;
		const std::string sAbsolutePath = m_sParentDirectory + '\\' + _sPath;

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
		const filesystem::path parentPath = filesystem::path(sAbsolutePath).parent_path();

		if (!filesystem::exists(parentPath)) {
			try {
				filesystem::create_directories(parentPath);
			}
			catch (const filesystem::filesystem_error& e) {
				throw IOException(e.what());
				return;
			}
		}

		ofstream stream(sAbsolutePath, ios_base::binary);
		stream.write(_data.data(), _data.size());
		stream.close();
	}


	void ProgramFilesManager::WriteProgramFile(const char* _pBytes, size_t _uByteCount, const string& _sFilePath) {
		const string sAbsolutePath = m_sParentDirectory + '\\' + _sFilePath;
		const filesystem::path parentPath = filesystem::path(sAbsolutePath).parent_path();

		if (!filesystem::exists(parentPath)) {
			try {
				filesystem::create_directories(parentPath);
			}
			catch (const filesystem::filesystem_error& e) {
				throw IOException(e.what());
			}
		}

		ofstream stream(sAbsolutePath, ios_base::binary);
		stream.write(_pBytes, _uByteCount);
		stream.close();
	}
}