// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanManagedBuffer.cpp - Vulkan managed buffer object class implementation
// author: Karl-Mihkel Ott

#pragma once

#include "deng/ErrorDefinitions.h"
#include "deng/Exceptions.h"
#include "deng/VulkanManagedBuffer.h"

namespace DENG {
	namespace Vulkan {

		ManagedBuffer::ManagedBuffer(
			VkDevice _hDevice,
			VkPhysicalDevice _hPhysicalDevice,
			VkQueue _hGraphicsQueue,
			VkCommandPool _hCommandPool,
			uint32_t _uMinimalBufferAlignment,
			MemoryPropertyFlags _ePropertyFlags,
			BufferUsageFlagBits _bBufferUsage) :
			IGPUManagedBuffer(_ePropertyFlags, _bBufferUsage),
			m_hDevice(_hDevice),
			m_hPhysicalDevice(_hPhysicalDevice),
			m_hGraphicsQueue(_hGraphicsQueue),
			m_hCommandPool(_hCommandPool),
			m_uMinimalBufferAlignment(_uMinimalBufferAlignment) 
		{
		}

		
		VkCommandBuffer ManagedBuffer::_BeginCommandBufferRecording()
		{
			/// Begin temporary command buffer
			VkCommandBuffer tmpCommandBuffer = VK_NULL_HANDLE;

			VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandPool = m_hCommandPool;
			commandBufferAllocateInfo.commandBufferCount = 1;

			// allocate command buffers
			if (vkAllocateCommandBuffers(m_hDevice, &commandBufferAllocateInfo, &tmpCommandBuffer) != VK_SUCCESS)
				throw RendererException("Failed to allocate command buffer (vkAllocateCommandBuffers)!");

			// setup command buffer begin info
			VkCommandBufferBeginInfo commandBufferBeginInfo = {};
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			// begin command buffers
			if (vkBeginCommandBuffer(tmpCommandBuffer, &commandBufferBeginInfo) != VK_SUCCESS)
				throw RendererException("Failed to start command buffer recording!");

			return tmpCommandBuffer;
		}


		void ManagedBuffer::_EndCommandBufferRecording(VkCommandBuffer _hCommandBuffer)
		{
			if (vkEndCommandBuffer(_hCommandBuffer) != VK_SUCCESS)
				throw RendererException("Failed to end command buffer recording (vkEndCommandBuffer)!");

			// setup submit info
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &_hCommandBuffer;

			if (vkQueueSubmit(m_hGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
				throw RendererException("Failed to submit graphics queue with single usage command buffer (vkQueueSubmit)!");
			vkQueueWaitIdle(m_hGraphicsQueue);

			// Cleanup
			vkFreeCommandBuffers(m_hDevice, m_hCommandPool, 1, &_hCommandBuffer);
		}

		void ManagedBuffer::Allocate(size_t _uLength)
		{
			/// Create logical buffer handle
			VkBufferUsageFlags bVkBufferUsage = 0;
			
			// translate buffer usage bits into VkBufferUsageFlags
			if (m_bBufferUsageFlags & BufferUsageFlagBit_CopySource)
				bVkBufferUsage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			if (m_bBufferUsageFlags & BufferUsageFlagBit_CopyDestination)
				bVkBufferUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			if (m_bBufferUsageFlags & BufferUsageFlagBit_Vertices)
				bVkBufferUsage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			if (m_bBufferUsageFlags & BufferUsageFlagBit_Indices)
				bVkBufferUsage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			if (m_bBufferUsageFlags & BufferUsageFlagBit_Uniform)
				bVkBufferUsage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			if (m_bBufferUsageFlags & BufferUsageFlagBit_StorageBuffer)
				bVkBufferUsage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			if (m_bBufferUsageFlags & BufferUsageFlagBit_StorageTexels)
				bVkBufferUsage |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
			if (m_bBufferUsageFlags & BufferUsageFlagBit_IndirectBuffer)
				bVkBufferUsage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = static_cast<VkDeviceSize>(_uLength);
			bufferCreateInfo.usage = bVkBufferUsage;
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			// try to create a vulkan buffer object
			if (vkCreateBuffer(m_hDevice, &bufferCreateInfo, nullptr, &m_hBuffer) != VK_SUCCESS)
				throw RendererException("Failed to create a buffer handle (vkCreateBuffer)");

			VkMemoryRequirements memRequirements = {};
			vkGetBufferMemoryRequirements(m_hDevice, m_hBuffer, &memRequirements);

			/// Actually allocate memory for the buffer
			VkMemoryAllocateInfo memAllocateInfo = {};
			memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memAllocateInfo.allocationSize = memRequirements.size;
			memAllocateInfo.memoryTypeIndex = memRequirements.memoryTypeBits;

			if (vkAllocateMemory(m_hDevice, &memAllocateInfo, nullptr, &m_hDeviceMemory) != VK_SUCCESS)
				throw RendererException("Failed to allocate buffer memory (vkAllocateMemory)!");

			/// Bind buffer with its memory
			vkBindBufferMemory(m_hDevice, m_hBuffer, m_hDeviceMemory, 0);
		}

		
		void ManagedBuffer::Free() 
		{
			DENG_ASSERT(m_uLength);
			vkDestroyBuffer(m_hDevice, m_hBuffer, nullptr);
			vkFreeMemory(m_hDevice, m_hDeviceMemory, nullptr);
		}

		
		void* ManagedBuffer::MapMemory(size_t _uOffset, size_t _uSize)
		{
			DENG_ASSERT(m_ePropertyFlags != MemoryPropertyFlags::GPUOnly);
			DENG_ASSERT(!m_bIsMapped && m_uLength);

			void* pData = nullptr;
			vkMapMemory(m_hDevice, m_hDeviceMemory, _uOffset, _uSize, 0, &pData);
			
			m_bIsMapped = true;
			return pData;
		}

		
		void ManagedBuffer::UnmapMemory()
		{
			DENG_ASSERT(m_bIsMapped);
			vkUnmapMemory(m_hDevice, m_hDeviceMemory);
		}


		void ManagedBuffer::Copy(IGPUManagedBuffer* _pOther, size_t _uDstOffset, size_t _uSrcOffset, size_t _uSize)
		{
			VkCommandBuffer commandBuffer = _BeginCommandBufferRecording();

			// setup buffer copy region
			VkBufferCopy bufferCopy = {};
			bufferCopy.srcOffset = static_cast<VkDeviceSize>(_uSrcOffset);
			bufferCopy.dstOffset = static_cast<VkDeviceSize>(_uDstOffset);
			bufferCopy.size = static_cast<VkDeviceSize>(_uSize);

			// call vulkan buffer copy command
			ManagedBuffer* pOtherVulkan = dynamic_cast<ManagedBuffer*>(_pOther);
			DENG_ASSERT(pOtherVulkan);
			vkCmdCopyBuffer(commandBuffer, m_hBuffer, pOtherVulkan->m_hBuffer, 1, &bufferCopy);

			_EndCommandBufferRecording(commandBuffer);
		}


		void ManagedBuffer::Reserve(size_t _uCapacity) 
		{
			/// Check if buffer is already allocated
			if (this->IsAllocated()) 
			{
				ManagedBuffer other(
					m_hDevice,
					m_hPhysicalDevice,
					m_hGraphicsQueue,
					m_hCommandPool,
					m_uMinimalBufferAlignment,
					m_ePropertyFlags,
					m_bBufferUsageFlags);
				other.Allocate(_uCapacity);
				other.Copy(this, 0, 0, std::min(m_uLength, _uCapacity));
				Free();
				Allocate(_uCapacity);
				this->Copy(&other, 0, 0, _uCapacity);
			}
			else
			{
				Allocate(_uCapacity);
			}
		}
		

		size_t ManagedBuffer::SubAllocate(size_t _uSize, size_t _uAlignment)
		{
			return m_memoryAllocator.RequestMemory(_uSize, _uAlignment).uOffset;
		}


		void ManagedBuffer::SubDeallocate(size_t _uOffset)
		{
			m_memoryAllocator.FreeMemory(_uOffset);
		}
	}
}