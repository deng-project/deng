// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanHelpers.h - Vulkan helper functions header
// author: Karl-Mihkel Ott


#ifndef VULKAN_HELPERS_H
#define VULKAN_HELPERS_H

#ifdef VULKAN_HELPERS_CPP
    #include "deng/ErrorDefinitions.h"
#endif

#include <vulkan/vulkan.h>
#include "deng/IRenderer.h"
#define VK_FORMAT_DEFAULT_IMAGE VK_FORMAT_R8G8B8A8_UNORM

namespace DENG {
    namespace Vulkan {

        uint32_t _FindMemoryTypeIndex(const VkMemoryType* pMemoryTypes, uint32_t _uMemoryTypeCount, uint32_t _uMemoryTypeBits, VkMemoryPropertyFlags _memoryPropertyFlags);


        //////////////////////////////////////////////
        // ***** Allocators and buffer helper ***** //
        //////////////////////////////////////////////
        
        void _TransitionImageLayout(
            VkDevice _hDevice, 
            VkImage _hImage, 
            VkCommandPool _hCommandPool, 
            VkQueue _hGraphicsQueue, 
            VkImageLayout _oldLayout, 
            VkImageLayout _newLayout, 
            uint32_t _uMipLevels,
            uint32_t _uArrayCount);

        void _CopyBufferToImage(
            VkDevice _hDevice,
            VkCommandPool _cmd_pool, 
            VkQueue _graphics_queue, 
            VkBuffer _src, 
            VkImage _dst, 
            uint32_t _width, 
            uint32_t _height,
            uint32_t _array_count);
        VkSampler _CreateTextureSampler(VkDevice _dev, float _max_sampler_anisotropy, uint32_t _mip_levels);

        void _CopyToBufferMemory(VkDevice _dev, VkDeviceSize _size, const void *_src, VkDeviceMemory _dst, VkDeviceSize _offset);
        // using malloc
        void *_CopyToDeviceMemory(VkDevice _dev, VkDeviceSize _size, VkDeviceMemory _src, VkDeviceSize _offset);
        void _CopyBufferToBuffer(
            VkDevice _dev, 
            VkCommandPool _cmd_pool, 
            VkQueue _graphics_q, 
            VkBuffer _src, 
            VkBuffer _dst, 
            VkDeviceSize _size, 
            VkDeviceSize _src_offset, 
            VkDeviceSize _dst_offset);
        
        // Similar _CopyToBufferMemory(), except using staging buffer for intermediate data storage
        void _ImplicitDataToBufferCopy(
            VkDevice _dev, 
            VkPhysicalDevice _gpu, 
            VkCommandPool _cmd_pool, 
            VkQueue _graphics_queue, 
            VkDeviceSize _size, 
            const void *_src, 
            VkBuffer _dst, 
            VkDeviceSize _offset);

        /////////////////////////////////////////////////////////////
        // ***** Universal temporary commandbuffer recording ***** //
        /////////////////////////////////////////////////////////////

        void _BeginCommandBufferSingleCommand(VkDevice _dev, VkCommandPool _cmd_pool, VkCommandBuffer &_cmd_buf);
        void _EndCommandBufferSingleCommand(VkDevice _dev, VkQueue _graphics_q, VkCommandPool _cmd_pool, VkCommandBuffer& _cmd_buf);
    }
}

#endif
