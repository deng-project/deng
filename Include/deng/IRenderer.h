// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: IRenderer.h - renderer interface class header
// author: Karl-Mihkel Ott

#ifndef IRENDERER_H
#define IRENDERER_H

#include <queue>
#include <iostream>
#include <vector>
#include <list>
#include <Windows.h>
#include <vulkan/vulkan.h>
#include <SDL2/SDL_opengl.h>

#include "deng/Api.h"
#include "deng/IWindowContext.h"
#include "deng/IFramebuffer.h"
#include "deng/VulkanHelpers.h"
#include "deng/GPUMemoryAllocator.h"
#include "deng/ShaderDefinitions.h"
#include "deng/Components.h"
#include "deng/Missing.h"

#ifndef MAX_FRAMES_IN_FLIGHT
#define MAX_FRAMES_IN_FLIGHT 2
#endif

// hasher helper
namespace std {
	template<>
	struct hash<list<DENG::PipelineModule>::iterator> {
		size_t operator()(const list<DENG::PipelineModule>::iterator& it) const {
			return hash<DENG::PipelineModule*>()(&*it);
		}
	};
}

namespace DENG {

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
		uint32_t uWidth = 0;
		uint32_t uHeight = 0;
		unsigned char uBitDepth = 0;

		TextureResourceType eResourceType = TEXTURE_RESOURCE_UNKNOWN;
		TextureResourceLoadType eLoadType = TEXTURE_RESOURCE_LOAD_TYPE_UNKNOWN;

		std::variant<Vulkan::TextureData, GLuint> apiTextureHandles;

		// this data needs to be deleted manually
		char* pRGBAData = nullptr;
		bool bHeapAllocationFlag = false;
	};

    enum BufferDataType { VERTEX, INDEX, UNIFORM };

    class DENG_API IRenderer {
        protected:
			static uint32_t m_uImageCounter;

			IWindowContext* m_pWindowContext = nullptr;
            GPUMemoryAllocator m_gpuMemoryAllocator;
            std::list<PipelineModule> m_pipelineModules;
            std::unordered_map<uint32_t, TextureResource> m_textureRegistry;
			std::queue<uint32_t> m_addedTextureResourceQueue;
			std::queue<uint32_t> m_deletedTextureResourceQueue;
            std::vector<IFramebuffer*> m_framebuffers;

            uint32_t m_uMissing2DTextureId = 0;
			uint32_t m_uMissing3DTextureId = 0;

        public:
            IRenderer() = default;
			virtual ~IRenderer() {};

			virtual uint32_t AddTextureResource(const TextureResource& _resource) = 0;
			
			inline void DeleteTextureResource(uint32_t _uTextureId) {
				m_deletedTextureResourceQueue.push(_uTextureId);
			}

			virtual std::list<PipelineModule>::iterator CreatePipeline(const PipelineModule& _module) = 0;
            virtual void DestroyPipeline(std::list<PipelineModule>::iterator _identifier) = 0;
            virtual IFramebuffer* CreateFramebuffer(uint32_t _uWidth, uint32_t _uHeight) = 0;
            virtual IFramebuffer* CreateContext(IWindowContext* _pWindow) = 0;
            virtual size_t AllocateMemory(size_t _uSize, BufferDataType _eType) = 0;
			virtual void DeallocateMemory(size_t _uOffset) = 0;
            virtual void UpdateBuffer(const void* _pData, size_t _uSize, size_t _uOffset) = 0;
            virtual void DrawMesh(const MeshComponent& _mesh, uint32_t _uMeshId, IFramebuffer* _pFramebuffer, const std::vector<uint32_t>& _textureIds = {}) = 0;
    };
}

#endif
