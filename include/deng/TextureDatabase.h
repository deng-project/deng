// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: TextureDatabase.h - Texture database singleton class header
// author: Karl-Mihkel Ott

#ifndef TEXTURE_DATABASE_H
#define TEXTURE_DATABASE_H

#ifdef TEXTURE_DATABASE_CPP
	#include <cstdint>
	#include <string>
	#include <array>
	#include <vector>
	#include <fstream>
	#include <queue>
	#include <variant>
	#include <stdexcept>
	#include <vulkan/vulkan.h>

	#include <nwin/glad/glad.h>

	#include "das/stb_image.h"
	#include "deng/Api.h"
#endif

namespace DENG {

	namespace Vulkan {
		struct TextureData {
			VkImage image = VK_NULL_HANDLE;
			VkDeviceMemory memory = VK_NULL_HANDLE;
			VkImageView image_view = VK_NULL_HANDLE;
			VkSampler sampler = VK_NULL_HANDLE;
		};
	};

	enum TextureResourceType {
		TEXTURE_RESOURCE_UNKNOWN,
		TEXTURE_RESOURCE_1D_IMAGE,
		TEXTURE_RESOURCE_2D_IMAGE,
		TEXTURE_RESOURCE_3D_IMAGE,
		TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_2D_IMAGE,
		TEXTURE_RESOURCE_INTERNAL_FRAMEBUFFER_3D_IMAGE,
		TEXTURE_RESOURCE_SWAPCHAIN_IMAGE,
	};

	enum TextureResourceLoadType {
		TEXTURE_RESOURCE_LOAD_TYPE_UNKNOWN = 0x00,
		TEXTURE_RESOURCE_LOAD_TYPE_EMBEDDED = 0x01,
		TEXTURE_RESOURCE_LOAD_TYPE_EXTERNAL_JPEG = 0x02,
		TEXTURE_RESOURCE_LOAD_TYPE_EXTERNAL_PNG = 0x04,
		TEXTURE_RESOURCE_LOAD_TYPE_EXTERNAL_TIFF = 0x08,
		TEXTURE_RESOURCE_LOAD_TYPE_EXTERNAL_BMP = 0x10,
		TEXTURE_RESOURCE_LOAD_TYPE_EXTERNAL_TGA = 0x20,
		TEXTURE_RESOURCE_LOAD_TYPE_EXTERNAL_GIF = 0x40
	};

	struct TextureResource {
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t bit_depth = 0;

		TextureResourceType resource_type = TEXTURE_RESOURCE_UNKNOWN;
		TextureResourceLoadType load_type = TEXTURE_RESOURCE_LOAD_TYPE_UNKNOWN;

		// this data needs to be deleted manually
		char* rgba_data = nullptr;
		std::variant<Vulkan::TextureData, GLuint> vendor;
	};

	class DENG_API TextureDatabase {
		private:
			static TextureDatabase* m_instance;
			std::vector<TextureResource*> m_resources;
			std::queue<uint32_t> m_added_event_queue;
			std::queue<TextureResource> m_deleted_event_queue;

		private:
			TextureDatabase() = default;
			TextureResourceLoadType GetResourceType(const std::string& _file_name);
		
		public:
			static TextureDatabase* GetInstance();
			static void DeleteInstance();

			// add an existing resource
			inline uint32_t AddResource(const TextureResource& _res) {
				m_resources.emplace_back(new TextureResource(_res));
				m_added_event_queue.push(static_cast<uint32_t>(m_resources.size() - 1));

				return static_cast<uint32_t>(m_resources.size() - 1);
			}

			// adds a 2D image resource from file
			uint32_t AddResource(const std::string& _file_name, uint32_t desired_depth = 4);
			// adds a 2D image resource from memory
			uint32_t AddResource(const char* _data, uint32_t _width, uint32_t _height, uint32_t _bit_depth);
			// adds a 3D image resource from files
			uint32_t AddResource(const std::string &_right, 
								 const std::string &_left,
								 const std::string &_top,
								 const std::string &_bottom,
								 const std::string &_front,
								 const std::string &_back,
								 uint32_t _desired_depth = 4);
			// add a 3D image resource from memory
			uint32_t AddResource(uint32_t _width, uint32_t _height, uint32_t _bit_depth,
								 const char* _right,
								 const char* _left,
								 const char* _top,
								 const char* _bottom,
								 const char* _front,
								 const char* _back);
			
			inline void DeleteResource(uint32_t _id) {
				if (_id < static_cast<uint32_t>(m_resources.size()))
					throw std::runtime_error("Invalid texture resource id (index out of bounds)");
				else if (m_resources[_id] == nullptr)
					throw std::runtime_error("Texture resource with id " + std::to_string(_id) + " is already deleted");

				m_deleted_event_queue.push(*m_resources[_id]);
				delete m_resources[_id];
				m_resources[_id] = nullptr;
			}

			inline TextureResource& GetResource(uint32_t _id) {
				if (_id < static_cast<uint32_t>(m_resources.size()))
					throw std::runtime_error("Invalid texture resource id (index out of bounds)");
				else if (m_resources[_id] == nullptr)
					throw std::runtime_error("Cannot retrieve deleted texture with id " + std::to_string(_id));

				return *m_resources[_id];
			}

			inline std::vector<TextureResource*>& GetAllResources() {
				return m_resources;
			}

			void DeleteAll();

			// queue events
			inline std::queue<uint32_t>& GetAddedEventQueue() {
				return m_added_event_queue;
			}

			inline std::queue<TextureResource>& GetDeletedEventQueue() {
				return m_deleted_event_queue;
			}
	};
};

#endif