// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanHelpers.h - Vulkan helper functions header
// author: Karl-Mihkel Ott

#pragma once

#include <vulkan/vulkan.h>
#define VK_FORMAT_DEFAULT_IMAGE VK_FORMAT_R8G8B8A8_UNORM

namespace DENG 
{
    namespace Vulkan 
    {

        uint32_t _FindMemoryTypeIndex(const VkMemoryType* pMemoryTypes, uint32_t _uMemoryTypeCount, uint32_t _uMemoryTypeBits, VkMemoryPropertyFlags _memoryPropertyFlags);

        void _TransitionImageLayout(
            VkDevice _hDevice, 
            VkImage _hImage, 
            VkCommandPool _hCommandPool, 
            VkQueue _hGraphicsQueue, 
            VkImageLayout _oldLayout, 
            VkImageLayout _newLayout, 
            uint32_t _uMipLevels,
            uint32_t _uArrayCount);

        void _BeginCommandBufferSingleCommand(VkDevice _hDevice, VkCommandPool _hCommandPool, VkCommandBuffer& _hCommandBuffer);
        void _EndCommandBufferSingleCommand(VkDevice _hDevice, VkQueue _hGraphicsQueue, VkCommandPool _hCommandPool, VkCommandBuffer& _hCommandBuffer);
    }
}