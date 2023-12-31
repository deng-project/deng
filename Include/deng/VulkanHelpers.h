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

        enum PhysicalDeviceType { OTHER, INTEGRATED_GPU, DISCREATE_GPU, VIRTUAL_GPU, CPU };

        struct PhysicalDeviceInformation {
            uint8_t pipelineCacheUUID[VK_UUID_SIZE] = {};

            std::string sPhysicalDeviceName;
            std::string sDriverVersion;

            uint32_t uApiVersionMajor = 0;
            uint32_t uApiVersionMinor = 0;
            uint32_t uApiVersionPatch = 0;
            uint32_t uApiVersionVariant = 0;

            uint32_t uVendorId = 0;
            uint32_t uDeviceId = 0;

            uint32_t uMinimalUniformBufferAlignment = 0;
            float fMaxSamplerAnisotropy = 0.f;

            PhysicalDeviceType eDeviceType = PhysicalDeviceType::OTHER;
        };

        struct BufferData {
            VkBuffer hBuffer = VK_NULL_HANDLE;
            VkDeviceMemory hMemory = VK_NULL_HANDLE;
            VkDeviceSize uSize = 0;
        };


        struct ShaderDescriptorData {
            VkDescriptorSetLayout hDescriptorSetLayout = VK_NULL_HANDLE;
            std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> descriptorSets;
        };

        struct TextureData {
            VkImage hImage = VK_NULL_HANDLE;
            VkImageView hImageView = VK_NULL_HANDLE;
            VkSampler hSampler = VK_NULL_HANDLE;
            VkDeviceMemory hMemory = VK_NULL_HANDLE;
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
