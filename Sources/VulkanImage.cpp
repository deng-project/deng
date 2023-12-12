// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanImage.h - Vulkan image loader class header
// author: Karl-Mihkel Ott

#include "deng/ErrorDefinitions.h"
#include "deng/Exceptions.h"
#include "deng/VulkanImage.h"
#include "deng/VulkanHelpers.h"
#include <cmath>

namespace DENG
{
	namespace Vulkan
	{
		Image::Image(
			VkDevice _hDevice,
			VkPhysicalDevice _hPhysicalDevice,
			VkQueue _hGraphicsQueue,
			VkCommandPool _hCommandPool,
			const VkMemoryType* _pMemoryTypes,
			uint32_t _uMemoryTypeCount,
			VkFormatFeatureFlags _optimalTilingFeatures) :
			m_hDevice(_hDevice),
			m_hPhysicalDevice(_hPhysicalDevice),
			m_hGraphicsQueue(_hGraphicsQueue),
			m_hCommandPool(_hCommandPool),
			m_pMemoryTypes(_pMemoryTypes),
			m_uMemoryTypeCount(_uMemoryTypeCount),
			m_optimalTilingFeatures(_optimalTilingFeatures)
		{

		}

		void Image::_CreateImageHandle(
			uint32_t _uWidth,
			uint32_t _uHeight,
			uint32_t _uMipLevels,
			uint32_t _uArrayCount,
			VkFormat _eFormat)
		{
			VkImageCreateInfo imageCreateInfo = {};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			imageCreateInfo.extent.width = _uWidth;
			imageCreateInfo.extent.height = _uHeight;
			imageCreateInfo.extent.depth = 1;
			imageCreateInfo.mipLevels = _uMipLevels;
			imageCreateInfo.arrayLayers = _uArrayCount;
			imageCreateInfo.format = _eFormat;
			imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageCreateInfo.flags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

			if (vkCreateImage(m_hDevice, &imageCreateInfo, nullptr, &m_hImage) != VK_SUCCESS)
				throw RendererException("Failed to create image (vkCreateImage)!");

			VkMemoryRequirements memoryRequirements = {};
			vkGetImageMemoryRequirements(m_hDevice, m_hImage, &memoryRequirements);

			// allocate memory for the image
			VkMemoryAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocateInfo.allocationSize = memoryRequirements.size;
			allocateInfo.memoryTypeIndex = _FindMemoryTypeIndex(m_pMemoryTypes, m_uMemoryTypeCount, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			if (vkAllocateMemory(m_hDevice, &allocateInfo, nullptr, &m_hMemory) != VK_SUCCESS)
				throw RendererException("Failed to allocate image memory (vkAllocateMemory)!");

			if (vkBindImageMemory(m_hDevice, m_hImage, m_hMemory, 0) != VK_SUCCESS)
				throw RendererException("Failed to bind image memory with handle (vkBindImageMemory)!");
		}
		
		
		void Image::_CreateImageViewHandle(uint32_t _uMipLevels, uint32_t _uArrayCount, VkImageViewType _imageViewType, VkFormat _eFormat)
		{
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = m_hImage;
			imageViewCreateInfo.viewType = _imageViewType;
			imageViewCreateInfo.format = _eFormat;
			imageViewCreateInfo.components =
			{
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A,
			};
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = _uMipLevels;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = _uArrayCount;

			if (vkCreateImageView(m_hDevice, &imageViewCreateInfo, nullptr, &m_hImageView) != VK_SUCCESS)
			{
				throw RendererException("Cannot create VkImageView object (vkCreateImageView)!");
			}
		}
		
		
		void Image::_CreateSamplerHandle()
		{
		}
		
		
		void Image::_GenerateMipMaps(uint32_t _uWidth, uint32_t _uHeight, uint32_t _uMipLevels)
		{
			if (!(m_optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
			{
				throw RendererException("Texture image format does not support linear blitting. Current implementation does not allow mipmapping when this feature is not available.");
			}

			VkCommandBuffer hCommandBuffer;
			_BeginCommandBufferSingleCommand(m_hDevice, m_hCommandPool, hCommandBuffer);

			VkImageMemoryBarrier barrier = {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = m_hImage;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.subresourceRange.levelCount = 1;

			uint32_t uMipWidth = _uWidth;
			uint32_t uMipHeight = _uHeight;
			for (uint32_t i = 1; i < _uMipLevels; i++) 
			{
				barrier.subresourceRange.baseMipLevel = i - 1;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				vkCmdPipelineBarrier(hCommandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				VkImageBlit blit = {};
				blit.srcOffsets[0] = { 0, 0, 0 };
				blit.srcOffsets[1] = { static_cast<int32_t>(uMipWidth), static_cast<int32_t>(uMipHeight), 1 };
				blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.srcSubresource.mipLevel = i - 1;
				blit.srcSubresource.baseArrayLayer = 0;
				blit.srcSubresource.layerCount = 1;
				blit.dstOffsets[0] = { 0, 0, 0 };
				blit.dstOffsets[1] = { std::max(static_cast<int32_t>(uMipWidth / 2), 1), std::max(static_cast<int32_t>(uMipHeight / 2), 1), 1 };
				blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.dstSubresource.mipLevel = i;
				blit.dstSubresource.baseArrayLayer = 0;
				blit.dstSubresource.layerCount = 1;

				vkCmdBlitImage(hCommandBuffer,
					m_hImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					m_hImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1, &blit,
					VK_FILTER_LINEAR);

				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(hCommandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
					0, nullptr,
					0, nullptr,
					1, &barrier);

				if (uMipWidth > 1) uMipWidth /= 2;
				if (uMipHeight > 1) uMipHeight /= 2;
			}

			barrier.subresourceRange.baseMipLevel = _uMipLevels - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			_EndCommandBufferSingleCommand(m_hDevice, m_hGraphicsQueue, m_hCommandPool, hCommandBuffer);
		}


		void Image::CreateDummy(uint32_t _uWidth, uint32_t _uHeight, uint32_t _uBitDepth, TextureType _eTextureType, uint32_t _uColor, bool _bCreateMipMaps)
		{
			// find the texture size and array count
			VkDeviceSize uSize = 0;
			VkImageCreateFlagBits bImageBits = static_cast<VkImageCreateFlagBits>(0);
			VkImageViewType imageViewType = static_cast<VkImageViewType>(0);
			uint32_t uArrayCount = 0;
			uint32_t uMipLevels = 1;

			if (_bCreateMipMaps)
			{
				uMipLevels = static_cast<uint32_t>(std::log2((double)std::max(_uWidth, _uHeight))) + 1;
			}

			switch (_eTextureType)
			{
				case TextureType::Image_1D:
					uSize = static_cast<VkDeviceSize>(_uWidth * _uBitDepth);
					uArrayCount = 1;
					imageViewType = VK_IMAGE_VIEW_TYPE_1D;
					break;

				case TextureType::Image_2D:
					uSize = static_cast<VkDeviceSize>(_uWidth * _uHeight * _uBitDepth);
					uArrayCount = 1;
					imageViewType = VK_IMAGE_VIEW_TYPE_2D;
					break;

				case TextureType::Image_3D:
				case TextureType::Image_3D_Array:
					uSize = static_cast<VkDeviceSize>(_uWidth * _uHeight * _uBitDepth * 6);
					uArrayCount = 6;
					imageViewType = VK_IMAGE_VIEW_TYPE_CUBE;
					bImageBits = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
					break;

				default:
					DENG_ASSERT(false);
					break;
			}

			VkFormat eFormat = VK_FORMAT_UNDEFINED;
			switch (_uBitDepth)
			{
				case 1:
					eFormat = VK_FORMAT_R8_UNORM;
					break;

				case 2:
					eFormat = VK_FORMAT_R8G8_UNORM;
					break;

				case 3:
					eFormat = VK_FORMAT_R8G8B8_UNORM;
					break;

				case 4:
					eFormat = VK_FORMAT_R8G8B8A8_UNORM;
					break;

				default:
					DENG_ASSERT(false);
			}

			ManagedBuffer stagingBuffer(
				m_hDevice,
				m_hPhysicalDevice,
				m_hGraphicsQueue,
				m_hCommandPool,
				(1 << _uBitDepth) & ~(_uBitDepth),
				MemoryPropertyFlags::CPUVisible,
				BufferUsageFlagBit_CopySource | BufferUsageFlagBit_CopyDestination);
			size_t uStagingBufferOffset = stagingBuffer.SubAllocate(uSize, 1);

			{
				void* pData = stagingBuffer.MapMemory(uStagingBufferOffset, uSize);
					for (uint32_t i = 0; i < _uWidth * _uHeight; i++) 
					{
						for (uint32_t j = 0; j < _uBitDepth; j++)
						{
							static_cast<unsigned char*>(pData)[i * _uBitDepth + j] = static_cast<unsigned char>((_uColor >> 8*j) % 256);
						}
					}
				stagingBuffer.UnmapMemory();
			}

			// create the image handle
			_CreateImageHandle(_uWidth, _uHeight, uMipLevels, uArrayCount, eFormat);

			// copy from staging buffer to image
			_TransitionImageLayout(m_hDevice, m_hImage, m_hCommandPool, m_hGraphicsQueue, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, uMipLevels, uArrayCount);
			this->CopyFrom(&stagingBuffer, static_cast<size_t>(uStagingBufferOffset));

			if (_bCreateMipMaps)
			{
				try
				{
					_GenerateMipMaps(_uWidth, _uHeight, uMipLevels);
				}
				catch (const RendererException& e)
				{
					DISPATCH_ERROR_MESSAGE("[RendererException]", e.what(), ErrorSeverity::NON_CRITICAL);
					WARNME("Mipmapping is skipped");
					_TransitionImageLayout(m_hDevice, m_hImage, m_hCommandPool, m_hGraphicsQueue, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL, uMipLevels, uArrayCount);
				}
			}
			else
			{
				_TransitionImageLayout(m_hDevice, m_hImage, m_hCommandPool, m_hGraphicsQueue, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL, uMipLevels, uArrayCount);
			}

			// create an image view
			_CreateImageViewHandle(uMipLevels, uArrayCount, imageViewType, eFormat);
		}
	}
}