// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanHelpers.cpp - Vulkan helper functions implementation
// author: Karl-Mihkel Ott

#define VULKAN_HELPERS_CPP
#include "deng/Exceptions.h"
#include "deng/VulkanHelpers.h"
#include "deng/FastTrigo.h"

namespace DENG 
{
    namespace Vulkan 
    {
        uint32_t _FindMemoryTypeIndex(const VkMemoryType* _pMemoryTypes, uint32_t _uMemoryTypeCount, uint32_t _uMemoryTypeBits, VkMemoryPropertyFlags _memoryPropertyFlags) 
        {
            for(uint32_t i = 0; i < _uMemoryTypeCount; i++) 
            {
                if ((_uMemoryTypeBits & (1 << i)) && (_pMemoryTypes[i].propertyFlags & _memoryPropertyFlags))
                {
                    return i;
                }
            }

            throw RendererException("Failed to find suitable memory type (Vulkan)!");
        }


        void _TransitionImageLayout(
            VkDevice _hDevice, 
            VkImage _hImage, 
            VkCommandPool _hCommandPool, 
            VkQueue _hGraphicsQueue, 
            VkImageLayout _oldLayout,
            VkImageLayout _newLayout,
            uint32_t _uMipLevels,
            uint32_t _uArrayCount)
        {
            // Begin recording cmd_buf
            VkCommandBuffer hCommandBuffer;
            _BeginCommandBufferSingleCommand(_hDevice, _hCommandPool, hCommandBuffer);

            // Set up image memory barrier struct
            VkImageMemoryBarrier memoryBarrier = {};
            memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            memoryBarrier.oldLayout = _oldLayout;
            memoryBarrier.newLayout = _newLayout;
            memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            memoryBarrier.image = _hImage;
            memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            memoryBarrier.subresourceRange.baseMipLevel = 0;
            memoryBarrier.subresourceRange.levelCount = _uMipLevels;
            memoryBarrier.subresourceRange.baseArrayLayer = 0;
            memoryBarrier.subresourceRange.layerCount = _uArrayCount;

            // Set up pipeline stage flags
            VkPipelineStageFlags srcStage = 0;
            VkPipelineStageFlags dstStage = 0;
                    
            if(_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
            {
                memoryBarrier.srcAccessMask = 0;
                memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            } 
            else if(_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
            {
                memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            } 
            else 
            {
                throw RendererException("Invalid image layout translation!");
            }
        
            // Change image layout
            vkCmdPipelineBarrier(hCommandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);
            _EndCommandBufferSingleCommand(_hDevice, _hGraphicsQueue, _hCommandPool, hCommandBuffer);
        }


        void _BeginCommandBufferSingleCommand(VkDevice _hDevice, VkCommandPool _hCommandPool, VkCommandBuffer& _hCommandBuffer) 
        {
            // Set up cmd_buf allocation info
            VkCommandBufferAllocateInfo cmd_buf_allocinfo = {};
            cmd_buf_allocinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmd_buf_allocinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmd_buf_allocinfo.commandPool = _hCommandPool;
            cmd_buf_allocinfo.commandBufferCount = 1;

            // Allocate cmd_bufs
            vkAllocateCommandBuffers(_hDevice, &cmd_buf_allocinfo, &_hCommandBuffer);
            
            // Set up command buffer begin info
            VkCommandBufferBeginInfo cmd_buf_begininfo{};
            cmd_buf_begininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cmd_buf_begininfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            // Begin cmd_bufs
            if (vkBeginCommandBuffer(_hCommandBuffer, &cmd_buf_begininfo) != VK_SUCCESS)
            {
                throw RendererException("Failed to start command buffer recording (vkBeginCommandBuffer)!");
            }
        }


        void _EndCommandBufferSingleCommand(VkDevice _hDevice, VkQueue _hGraphicsQueue, VkCommandPool _hCommandPool, VkCommandBuffer& _hCommandBuffer) 
        {
            // Call Vulkan command buffer end handler
            vkEndCommandBuffer(_hCommandBuffer);

            // Set up submit info 
            VkSubmitInfo submitinfo{};
            submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitinfo.commandBufferCount = 1;
            submitinfo.pCommandBuffers = &_hCommandBuffer;
            
            // Submit to the graphics queue
            vkQueueSubmit(_hGraphicsQueue, 1, &submitinfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(_hGraphicsQueue);

            // Cleanup
            vkFreeCommandBuffers(_hDevice, _hCommandPool, 1, &_hCommandBuffer);
        }
    }
}
