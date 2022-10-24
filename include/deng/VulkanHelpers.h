// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanHelpers.h - Vulkan helper functions header
// author: Karl-Mihkel Ott


#ifndef VULKAN_HELPERS_H
#define VULKAN_HELPERS_H

#ifdef VULKAN_HELPERS_CPP
    #include <cstdint>
    #include <cstring>
#ifdef __DEBUG
    #include <iostream>
#endif
    #include <vulkan/vulkan.h>
    #include "deng/ErrorDefinitions.h"
#endif

namespace DENG {
    namespace Vulkan {

        struct TextureData {
            VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
            VkImage image = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;
            VkImageView image_view = VK_NULL_HANDLE;
            VkSampler sampler = VK_NULL_HANDLE;
            
            uint32_t width;
            uint32_t height;
            uint32_t depth;
        };


        struct HardwareInfo {
            uint32_t driver_version;
            uint32_t api_version;
            char gpu_name[512] = {};
        };
        

        uint32_t _FindMemoryType(VkPhysicalDevice _gpu, uint32_t _filter, VkMemoryPropertyFlags _props);


        //////////////////////////////////////////////
        // ***** Allocators and buffer helper ***** //
        //////////////////////////////////////////////
        

        void _AllocateMemory(VkDevice _device, VkPhysicalDevice _gpu, VkDeviceSize _size, VkDeviceMemory &_mem, uint32_t _type, VkMemoryPropertyFlags _props);
        VkImageViewCreateInfo _GetImageViewInfo(
            VkImage _img, 
            VkFormat _format,
            VkImageViewType _type,
            VkImageAspectFlags _flags, 
            uint32_t _mip_l, 
            uint32_t _array_count);
        VkMemoryRequirements _CreateBuffer(VkDevice _dev, VkDeviceSize _size, VkBufferUsageFlags _flags, VkBuffer &_buffer);
        VkMemoryRequirements _CreateImage(
            VkDevice _dev, 
            VkImage &_img, 
            uint32_t _width, 
            uint32_t _height, 
            uint32_t _mip_l,
            uint32_t _array_layers,
            VkFormat _format, 
            VkImageTiling _tiling, 
            VkImageUsageFlags _usage,
            VkSampleCountFlagBits _sample_c,
            VkImageCreateFlags _flags);

        void _TransitionImageLayout(
            VkDevice _dev, 
            VkImage _img, 
            VkCommandPool _cmd_pool, 
            VkQueue _graphics_q, 
            VkImageLayout _old, 
            VkImageLayout _new, 
            uint32_t _mip_l,
            uint32_t _array_count);

        void _CopyBufferToImage(
            VkDevice _dev,
            VkCommandPool _cmd_pool, 
            VkQueue _graphics_queue, 
            VkBuffer _src, 
            VkImage _dst, 
            uint32_t _width, 
            uint32_t _height,
            uint32_t _array_count);

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
