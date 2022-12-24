// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: TextureDatabase.cpp - Texture database singleton class implementation
// author: Karl-Mihkel Ott

#define TEXTURE_DATABASE_CPP
#include "deng/TextureDatabase.h"

namespace DENG {
	TextureDatabase* TextureDatabase::m_instance = nullptr;
	
	TextureDatabase* TextureDatabase::GetInstance() {
		if (!m_instance)
			m_instance = new TextureDatabase;
		return m_instance;
	}

	void TextureDatabase::DeleteInstance() {
		delete m_instance;
	}

	
	TextureResourceLoadType TextureDatabase::GetResourceType(const std::string& _file_name) {
		char bytes[4]{};
		std::fstream str(_file_name);
		str.read(bytes, 4);

		if (bytes[0] == 0xff && bytes[1] == 0xd8 && bytes[2] == 0xff && bytes[3] == 0xe0)
			return TEXTURE_RESOURCE_LOAD_TYPE_EXTERNAL_JPEG;
		else if (bytes[0] == 0x89 && bytes[1] == 0x50 && bytes[2] == 0x4e && bytes[3] == 0x47)
			return TEXTURE_RESOURCE_LOAD_TYPE_EXTERNAL_PNG;
		else if ((bytes[0] == 0x49 && bytes[1] == 0x49 && bytes[2] == 0x2a && bytes[3] == 0x00) ||
			(bytes[0] == 0x4d && bytes[1] == 0x4d && bytes[2] == 0x00 && bytes[3] == 0x2a))
			return TEXTURE_RESOURCE_LOAD_TYPE_EXTERNAL_TIFF;
		else if (bytes[0] == 0x42 && bytes[1] == 0x4d)
			return TEXTURE_RESOURCE_LOAD_TYPE_EXTERNAL_BMP;
		else if (bytes[0] == 0x47 && bytes[1] == 0x49 && bytes[2] == 0x46 && bytes[3] == 0x38)
			return TEXTURE_RESOURCE_LOAD_TYPE_EXTERNAL_GIF;
		
		return TEXTURE_RESOURCE_LOAD_TYPE_EXTERNAL_TGA;
	}

	
	uint32_t TextureDatabase::AddResource(const std::string& _file_name, uint32_t _desired_depth) {
		TextureResource res;
	
		int x, y, depth;
		char *data = (char*)stbi_load(_file_name.c_str(), &x, &y, &depth, (int)_desired_depth);
		res.width = static_cast<uint32_t>(x);
		res.height = static_cast<uint32_t>(y);
		res.bit_depth = depth;
		res.load_type = GetResourceType(_file_name);
		res.resource_type = TEXTURE_RESOURCE_2D_IMAGE;

		m_resources.emplace_back(res);
		m_added_event_queue.push(static_cast<uint32_t>(m_resources.size() - 1));
		return static_cast<uint32_t>(m_resources.size() - 1);
	}


	uint32_t TextureDatabase::AddResource(const char* _data, uint32_t _width, uint32_t _height, uint32_t _bit_depth) {
		TextureResource res;
		res.width = _width;
		res.height = _height;
		res.bit_depth = _bit_depth;
		res.load_type = TEXTURE_RESOURCE_LOAD_TYPE_EMBEDDED;
		res.resource_type = TEXTURE_RESOURCE_2D_IMAGE;

		m_resources.emplace_back(res);
		m_added_event_queue.push(static_cast<uint32_t>(m_resources.size() - 1));
		return m_added_event_queue.front();
	}


	uint32_t TextureDatabase::AddResource(const std::string& _right,
										  const std::string& _left,
										  const std::string& _top,
										  const std::string& _bottom,
										  const std::string& _front,
										  const std::string& _back,
										  uint32_t _desired_depth)
	{
		TextureResource res;
		std::array<std::string, 6> files = { _right, _left, _top, _bottom, _front, _back };
		std::array<const char*, 6> data_ptrs;

		for (auto it = files.begin(); it != files.end(); it++) {
			int x, y, depth;
			char* data = (char*)stbi_load(it->c_str(), &x, &y, &depth, (int)_desired_depth);

			if ((res.width || res.height) && (res.width != x || res.height != y))
				throw std::runtime_error("Conflicting cube map resolutions (" + std::to_string(res.width) + "x" + std::to_string(res.height) + " and " + std::to_string(x) + "x" + std::to_string(y));
			else {
				res.width = static_cast<uint32_t>(x);
				res.height = static_cast<uint32_t>(y);
			}
			
			res.load_type = static_cast<TextureResourceLoadType>(static_cast<int>(res.load_type) | static_cast<int>(GetResourceType(*it)));
			data_ptrs[it - files.begin()] = data;
		}

		res.resource_type = TEXTURE_RESOURCE_3D_IMAGE;
		res.rgba_data = new char[res.width * res.height * res.bit_depth * 6u]{};
		for (uint32_t i = 0; i < 6; i++)
			std::memcpy(res.rgba_data + i * res.width * res.height * res.bit_depth, data_ptrs[i], res.width * res.height * res.bit_depth);
		
		m_resources.emplace_back(res);
		m_added_event_queue.push(static_cast<uint32_t>(m_resources.size() - 1));
		return m_added_event_queue.front();
	}


	uint32_t TextureDatabase::AddResource(uint32_t _width, uint32_t _height, uint32_t _bit_depth,
										  const char* _right,
										  const char* _left,
										  const char* _top,
										  const char* _bottom,
										  const char* _front,
										  const char* _back)
	{
		TextureResource res;
		res.width = _width;
		res.height = _height;
		res.bit_depth = _bit_depth;
		res.load_type = TEXTURE_RESOURCE_LOAD_TYPE_EMBEDDED;
		res.resource_type = TEXTURE_RESOURCE_3D_IMAGE;
		res.rgba_data = new char[res.width * res.height * res.bit_depth * 6u]{};
		std::array<const char*, 6> data_ptrs = { _right, _left, _top, _bottom, _front, _back };

		for (uint32_t i = 0; i < 6; i++)
			std::memcpy(res.rgba_data + i * res.width * res.height * res.bit_depth, data_ptrs[i], res.width * res.height * res.bit_depth);

		m_resources.emplace_back(res);
		m_added_event_queue.push(static_cast<uint32_t>(m_resources.size() - 1));
		return m_added_event_queue.front();
	}
}