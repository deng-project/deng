// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanManagedBuffer.h - Vulkan managed buffer object class header
// author: Karl-Mihkel Ott

#pragma once

#include <vulkan/vulkan.h>
#include "deng/IGraphicsMemory.h"

namespace DENG {
	namespace Vulkan {

		class DENG_API ManagedBuffer : public IGPUManagedBuffer 
		{
			private:
				VkDevice m_hDevice = VK_NULL_HANDLE;
				VkPhysicalDevice m_hPhysicalDevice = VK_NULL_HANDLE;
				VkQueue m_hGraphicsQueue = VK_NULL_HANDLE;
				VkCommandPool m_hCommandPool = VK_NULL_HANDLE;
				VkBuffer m_hBuffer = VK_NULL_HANDLE;
				VkDeviceMemory m_hDeviceMemory = VK_NULL_HANDLE;
				uint32_t m_uMinimalBufferAlignment = 0;
				bool m_bIsMapped = false;

			protected:
				virtual void Allocate(size_t _uLength) override;
				virtual void Free() override;

			public:
				ManagedBuffer(
					VkDevice _hDevice,
					VkPhysicalDevice _hPhysicalDevice,
					VkQueue _hGraphicsQueue,
					VkCommandPool _hCommandPool,
					uint32_t _uMinimalBufferAlignment,
					MemoryPropertyFlags _ePropertyFlags,
					BufferUsageFlagBits _bBufferUsage);
				~ManagedBuffer();

				virtual void* MapMemory(size_t _uOffset, size_t _uSize) override;
				virtual void UnmapMemory() override;

				virtual void Copy(IGPUManagedBuffer* _pOther, size_t _uDstOffset, size_t _uSrcOffset, size_t _uSize) override;
				virtual void Reserve(size_t _uCapacity) override;
				virtual size_t SubAllocate(size_t _uSize, size_t _uAlignment) override;
				virtual void SubDeallocate(size_t _uOffset) override;

				inline VkBuffer GetBuffer() {
					return m_hBuffer;
				}

				inline VkDeviceMemory GetDeviceMemory() {
					return m_hDeviceMemory;
				}
		};
	}
}