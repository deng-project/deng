// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanImage.h - Vulkan image loader class header
// author: Karl-Mihkel Ott

#pragma once

#include <vulkan/vulkan.h>
#include "deng/IGraphicsMemory.h"
#include "deng/VulkanManagedBuffer.h"

namespace DENG
{
	namespace Vulkan 
	{
		class DENG_API Image : public IGPUImage
		{
			private:
				VkDevice m_hDevice = VK_NULL_HANDLE;
				VkPhysicalDevice m_hPhysicalDevice = VK_NULL_HANDLE;
				VkQueue m_hGraphicsQueue = VK_NULL_HANDLE;
				VkCommandPool m_hCommandPool = VK_NULL_HANDLE;

				VkImage m_hImage = VK_NULL_HANDLE;
				VkImageView m_hImageView = VK_NULL_HANDLE;
				VkSampler m_hSampler = VK_NULL_HANDLE;
				VkDeviceMemory m_hMemory = VK_NULL_HANDLE;

				const VkMemoryType* m_pMemoryTypes = nullptr;
				uint32_t m_uMemoryTypeCount = 0;
				VkFormatFeatureFlags m_optimalTilingFeatures = (VkFormatFeatureFlagBits)0;
				float m_fMaxSamplerAnisotropy = 0.f;

			private:
				void _CreateImageHandle(uint32_t _uWidth, uint32_t _uHeight, uint32_t _uMipLevels, uint32_t _uArrayCount, VkFormat _eFormat, VkImageCreateFlagBits _bImageBits = (VkImageCreateFlagBits)0);
				void _CreateImageViewHandle(uint32_t _uMipLevels, uint32_t _uArrayCount, VkImageViewType _imageViewType, VkFormat _eFormat);
				void _CreateSamplerHandle(uint32_t _uMipLevels);
				void _GenerateMipMaps(uint32_t _uWidth, uint32_t _uHeight, uint32_t _uMipLevels);
				VkFormat _PickFormat(uint32_t _uBitDepth);

			public:
				Image(
					VkDevice _hDevice,
					VkPhysicalDevice _hPhysicalDevice,
					VkQueue _hGraphicsQueue,
					VkCommandPool _hCommandPool,
					const VkMemoryType* pMemoryTypes,
					uint32_t uMemoryTypeCount,
					VkFormatFeatureFlags _optimalTilingFeatures,
					float _fMaxSamplerAnisotropy);
				~Image();

				virtual void CopyFrom(IGPUManagedBuffer* _pBuffer, size_t _uOffset, uint32_t _uWidth, uint32_t _uHeight, uint32_t _uArrayCount) override;

				virtual void CreateDummy(uint32_t _uWidth, uint32_t _uHeight, uint32_t _uBitDepth, TextureType _eTextureType, uint32_t _uColor = 0, bool _bCreateMipmaps = 0) override;
				virtual void LoadTexture1DFromMemory(const void* _pData, size_t _uLength, uint32_t _uBitDepth) override;
				virtual void LoadTexture2DFromMemory(const void* _pData, uint32_t _uWidth, uint32_t _uHeight, uint32_t _uBitDepth, bool _bCreateMipMaps = false) override;
				virtual void LoadTexture3DFromMemory(
					uint32_t _uWidth, uint32_t _uHeight, uint32_t _uBitDepth,
					const void* _pPosXData,
					const void* _pNegXData,
					const void* _pPosYData,
					const void* _pNegYData,
					const void* _pPosZData,
					const void* _pNegZData,
					bool _bCreateMipMaps = false
				) override;

				inline VkImage GetImage() { return m_hImage; }
				inline VkImageView GetImageView() { return m_hImageView; }
				inline VkSampler GetSampler() { return m_hSampler; }
		};
	}
}