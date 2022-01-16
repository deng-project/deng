// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanHelpers.h - Vulkan helper functions header
// author: Karl-Mihkel Ott


#ifndef VULKAN_HELPERS_H
#define VULKAN_HELPERS_H

#ifdef VULKAN_HELPERS_CPP
    #include <cstdint>
    #include <cstring>
    #include <vulkan/vulkan.h>
    #include <ErrorDefinitions.h>
#endif

namespace DENG {
    namespace Vulkan {

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
        VkImageViewCreateInfo _GetImageViewInfo(VkImage _img, VkFormat _format, VkImageAspectFlags _flags, uint32_t _mip_l);
        VkMemoryRequirements _CreateBuffer(VkDevice _dev, VkDeviceSize _size, VkBufferUsageFlags _flags, VkBuffer &_buffer);
        VkMemoryRequirements _CreateImage(VkDevice _dev, VkImage &_img, uint32_t _width, uint32_t _height, uint32_t _mip_l, VkFormat _format, 
                                          VkImageTiling _tiling, VkImageUsageFlags _usage, VkSampleCountFlagBits _sample_c);
        void _TransitionImageLayout(VkDevice _dev, VkImage _img, VkCommandPool _cmd_pool, VkQueue _graphics_q, VkFormat _format, VkImageLayout _old, 
                                    VkImageLayout _new, uint32_t _mip_l);
        void _CopyBufferToImage(VkDevice _dev, VkCommandPool _cmd_pool, VkQueue _graphics_queue, VkBuffer _src, VkImage _dst, uint32_t _width, uint32_t _height);
        void _CopyToBufferMemory(VkDevice _dev, VkDeviceSize _size, const void *_src, VkDeviceMemory _dst, VkDeviceSize _offset);
        void _CopyBufferToBuffer(VkDevice _dev, VkCommandPool _cmd_pool, VkQueue _graphics_q, VkBuffer _src, VkBuffer _dst, VkDeviceSize _size, 
                                 VkDeviceSize _src_offset, VkDeviceSize _dst_offset);

        /////////////////////////////////////////////////////////////
        // ***** Universal temporary commandbuffer recording ***** //
        /////////////////////////////////////////////////////////////

        void _BeginCommandBufferSingleCommand(VkDevice _dev, VkCommandPool _cmd_pool, VkCommandBuffer &_cmd_buf);
        void _EndCommandBufferSingleCommand(VkDevice _dev, VkQueue _graphics_q, VkCommandPool _cmd_pool, VkCommandBuffer &_cmd_buf);


        inline uint32_t _AlignData(uint32_t _req_mem, uint32_t _align) {
            return _req_mem + (_req_mem % _align > 0 ? _align - (_req_mem % _align) : 0);
        }
    }
}

#endif