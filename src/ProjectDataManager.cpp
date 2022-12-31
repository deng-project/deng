// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ProjectDataManager.h - Project data management abstraction class implementation
// author: Karl-Mihkel Ott

#define PROJECT_DATA_MANAGER_CPP
#include "deng/ProjectDataManager.h"

namespace DENG {

	ProjectDataManager::ProjectDataManager(const std::string& _path, bool _read_bit, const std::string& _data_file_name) :
		m_read_bit(_read_bit),
		m_file_name(_data_file_name),
		m_project_path(_path)
	{
		if (m_file_name == "" && std::filesystem::exists(m_project_path + "\\game.xml")) {
			m_file_name = "game.xml";
		}
		else if (m_file_name == "" && std::filesystem::exists(m_project_path + "\\game.dat")) {
			m_file_name = "game.dat";
		}
		else {
			m_fail_bit = true;
			return;
		}

		// is read bit specified
		if (m_read_bit) {
			Load();
		} else {
			_CreateNewProject();
		}
	}


	void ProjectDataManager::_CreateNewProject() {
		// create the parent directory
		if (!std::filesystem::exists(m_project_path))
			std::filesystem::create_directories(std::filesystem::path(m_project_path));

		// Game\Assets
		std::filesystem::create_directory(std::filesystem::path(m_project_path + "\\Assets"));
		// Game\Scripts
		std::filesystem::create_directory(std::filesystem::path(m_project_path + "\\Scripts"));
		// Game\Shaders
		std::filesystem::create_directory(std::filesystem::path(m_project_path + "\\Shaders"));
		// Game\SoundEffects
		std::filesystem::create_directory(std::filesystem::path(m_project_path + "\\SoundEffects"));
		// Game\Textures
		std::filesystem::create_directory(std::filesystem::path(m_project_path + "\\Textures"));
		// Game\Videos
		std::filesystem::create_directory(std::filesystem::path(m_project_path + "\\Videos"));
		// Game\Python
		std::filesystem::copy(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\Python"),
			std::filesystem::path(m_project_path + "\\Python"),
			std::filesystem::copy_options::recursive);

		// Game\deng-complete.dll
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\deng-complete.dll"),
			std::filesystem::path(m_project_path + "\\deng-complete.dll"));

		// Game\drt.exe
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\drt.exe"),
			std::filesystem::path(m_project_path + "\\drt.exe"));

#ifdef _DEBUG
		// Game\python310_d.dll
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\python310_d.dll"),
			std::filesystem::path(m_project_path + "\\python310_d.dll"));

		// Game\zlibd.dll
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\zlibd1.dll"),
			std::filesystem::path(m_project_path + "\\zlibd1.dll"));
#else
		// Game\python310.dll
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\python310.dll"),
			std::filesystem::path(m_project_path + "\\python310.dll"));

		// Game\zlib.dll
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\zlib1.dll"),
			std::filesystem::path(m_project_path + "\\zlib1.dll"));
#endif

		// Game\vcruntime140.dll
		std::filesystem::copy_file(
			std::filesystem::path("C:\\Windows\\System32\\vcruntime140.dll"),
			std::filesystem::path(m_project_path + "\\vcruntime140.dll"));

		// create a game data file
		std::filesystem::path pth(m_file_name);
		if (pth.extension().u8string() == ".xml") {
			DXML::GameXMLWriter writer(m_project_path + "\\" + m_file_name);
			writer.Write(DXML::SerializedGame());
		}
		else {
			DXML::SerializedGame game;
			std::ofstream stream(m_project_path + "\\" + m_file_name, std::ios::binary);
			game.Serialize(stream);
			stream.close();
		}
	}


	std::string ProjectDataManager::_CalculateChecksum() {
		CryptoPP::Weak::MD5 hash;

		// calculate hash from all files
		for (const auto& entry : std::filesystem::recursive_directory_iterator(m_project_path)) {
			if (entry.is_regular_file() && entry.path().filename().u8string() != "CHECKSUM") {
				std::ifstream stream(entry.path(), std::ios::binary);
				stream.seekg(0, std::ios_base::end);
				size_t size = stream.tellg();
				stream.seekg(0, std::ios_base::beg);

				char* data = new char[size];
				stream.read(data, size);
				hash.Update((CryptoPP::byte*)data, size);
				delete[] data;
			}
		}

		std::string digest;
		std::string str_digest;
		digest.resize(hash.DigestSize());
		hash.Final((CryptoPP::byte*)digest.data());

		CryptoPP::StringSource src(digest, true, new CryptoPP::HashFilter(hash,
												  new CryptoPP::HexEncoder(
												   new CryptoPP::StringSink(str_digest))));


		return str_digest;
	}


	void ProjectDataManager::Load() {
		// check if the file is in XML format
		std::filesystem::path pth(m_project_path + "\\" + m_file_name);
		if (pth.extension() == ".xml") {
			DXML::GameXMLReader reader(m_project_path + "\\" + m_file_name);
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
		} else if (pth.extension() == ".dat") {
			std::ifstream stream(m_project_path + "\\" + m_file_name);

			if (!stream.is_open()) {
				throw std::runtime_error("Failed to open file " + m_project_path + "/" + m_file_name);
			}

			m_game.Unserialize(stream);
			stream.close();
		}
	}


	void ProjectDataManager::Repair() {
		// Game\Python
		std::filesystem::copy(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\Python"),
			std::filesystem::path(m_project_path + "\\Python"),
			std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);

		// Game\deng-complete.dll
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\deng-complete.dll"),
			std::filesystem::path(m_project_path + "\\deng-complete.dll"),
			std::filesystem::copy_options::overwrite_existing);

		// Game\drt.exe
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\drt.exe"),
			std::filesystem::path(m_project_path + "\\drt.exe"),
			std::filesystem::copy_options::overwrite_existing);

#ifdef _DEBUG
		// Game\python310_d.dll
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\python310_d.dll"),
			std::filesystem::path(m_project_path + "\\python310_d.dll"),
			std::filesystem::copy_options::overwrite_existing);

		// Game\zlibd.dll
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\zlibd1.dll"),
			std::filesystem::path(m_project_path + "\\zlibd1.dll"),
			std::filesystem::copy_options::overwrite_existing);
#else
		// Game\python310.dll
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\python310.dll"),
			std::filesystem::path(m_project_path + "\\python310.dll"),
			std::filesystem::copy_options::overwrite_existing);

		// Game\zlib.dll
		std::filesystem::copy_file(
			std::filesystem::path(Libdas::Algorithm::GetProgramPath() + "\\zlib1.dll"),
			std::filesystem::path(m_project_path + "\\zlib1.dll"),
			std::filesystem::copy_options::overwrite_existing);
#endif

		// Game\vcruntime140.dll
		std::filesystem::copy_file(
			std::filesystem::path("C:\\Windows\\System32\\vcruntime140.dll"),
			std::filesystem::path(m_project_path + "\\vcruntime140.dll"),
			std::filesystem::copy_options::overwrite_existing);
	}


	void ProjectDataManager::Save() {
		// either serialize or write an xml configuration
		if (std::filesystem::path(m_file_name).extension().u8string() == ".xml") {
			DXML::GameXMLWriter writer(m_project_path + "\\" + m_file_name);
			writer.Write(m_game);
		} else {
			std::ofstream stream(m_project_path + '\\' + m_file_name);
			m_game.Serialize(stream);
			stream.close();
		}
	}

	void ProjectDataManager::VerifyFiles() {
		// check if CHECKSUM file exists
		if (std::filesystem::exists(m_project_path + "\\CHECKSUM")) {
			std::ifstream stream(m_project_path + "\\CHECKSUM");
			stream.seekg(0, std::ios_base::end);
			size_t size = stream.tellg();
			stream.seekg(0, std::ios_base::beg);

			std::string expected_sum;
			expected_sum.resize(size);
			stream.read((char*)expected_sum.data(), expected_sum.size());

			std::string given_sum = _CalculateChecksum();

			if (given_sum == expected_sum) {
				m_checksum_flag = 0;
				return;
			}

			m_checksum_flag = 1;
		}

		m_checksum_flag = 2;
	}


	void ProjectDataManager::CalculateAndWriteChecksum() {
		// calculate and write checksum file
		auto sum = _CalculateChecksum();
		std::ofstream stream(m_project_path + "\\CHECKSUM");
		stream.write((char*)sum.data(), sum.size());
		stream.close();
	}
}