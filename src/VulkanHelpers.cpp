// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanHelpers.cpp - Vulkan helper functions implementation
// author: Karl-Mihkel Ott

#define VULKAN_HELPERS_CPP
#include "deng/VulkanHelpers.h"

namespace DENG {

    namespace Vulkan {

        
        uint32_t _FindMemoryType(VkPhysicalDevice _gpu, uint32_t _filter, VkMemoryPropertyFlags _props) {
            VkPhysicalDeviceMemoryProperties mem_props;
            vkGetPhysicalDeviceMemoryProperties(_gpu, &mem_props);

            for(uint32_t i = 0; i < mem_props.memoryTypeCount; i++) {
                if((_filter & (1 << i)) && (mem_props.memoryTypes[i].propertyFlags & _props))
                    return i;
            }

            VK_BUFFER_ERR("failed to find suitable memory type");
        }


        ////////////////////////////
        // ***** Allocators ***** //
        ////////////////////////////
        

        void _AllocateMemory(VkDevice _device, VkPhysicalDevice _gpu, VkDeviceSize _size, VkDeviceMemory &_mem, uint32_t _type, VkMemoryPropertyFlags _props) {
            VkMemoryAllocateInfo alloc_info = {};
            alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            alloc_info.allocationSize = _size;
            
            alloc_info.memoryTypeIndex = _FindMemoryType(_gpu, _type, _props);

            if(vkAllocateMemory(_device, &alloc_info, nullptr, &_mem) != VK_SUCCESS)
                VK_BUFFER_ERR("failed to allocate buffer memory!");
        }


        VkImageViewCreateInfo _GetImageViewInfo(
            VkImage _img, 
            VkFormat _format, 
            VkImageViewType _type,
            VkImageAspectFlags _flags, 
            uint32_t _mip_l, 
            uint32_t _array_count) 
        {
            VkImageViewCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = _img;
            create_info.viewType = _type;
            create_info.format = _format;

            create_info.subresourceRange.aspectMask = _flags;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = _mip_l;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = _array_count;
            return create_info;
        }


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
            VkImageCreateFlags _flags) 
        {
            // Set up image createinfo
            VkImageCreateInfo image_createinfo = {};
            image_createinfo.sType  = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_createinfo.imageType = VK_IMAGE_TYPE_2D;
            image_createinfo.extent.width = _width;
            image_createinfo.extent.height = _height;
            image_createinfo.extent.depth = 1;
            image_createinfo.mipLevels = _mip_l;
            image_createinfo.arrayLayers = _array_layers;
            image_createinfo.format = _format;
            image_createinfo.tiling = _tiling;
            image_createinfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            image_createinfo.usage = _usage;
            image_createinfo.samples = _sample_c;
            image_createinfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            image_createinfo.flags = _flags;

            // Create image
            if(vkCreateImage(_dev, &image_createinfo, nullptr, &_img) != VK_SUCCESS) 
                VK_BUFFER_ERR("failed to create image!");

            // Get memory requirements for this image
            VkMemoryRequirements memory_requirement;
            vkGetImageMemoryRequirements(_dev, _img, &memory_requirement);
            
            return memory_requirement;
        }


        void _TransitionImageLayout(
            VkDevice _dev, 
            VkImage _img, 
            VkCommandPool _cmd_pool, 
            VkQueue _graphics_q, 
            VkImageLayout _old,
            VkImageLayout _new,
            uint32_t _mip_l,
            uint32_t _array_count)
        {
            // Begin recording cmd_buf
            VkCommandBuffer tmp_cmd_buf;
            _BeginCommandBufferSingleCommand(_dev, _cmd_pool, tmp_cmd_buf);

            // Set up image memory barrier struct
            VkImageMemoryBarrier memory_barrier = {};
            memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            memory_barrier.oldLayout = _old;
            memory_barrier.newLayout = _new;
            memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            memory_barrier.image = _img;
            memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            memory_barrier.subresourceRange.baseMipLevel = 0;
            memory_barrier.subresourceRange.levelCount = _mip_l;
            memory_barrier.subresourceRange.baseArrayLayer = 0;
            memory_barrier.subresourceRange.layerCount = _array_count;

            // Set up pipeline stage flags
            VkPipelineStageFlags src_stage = 0;
            VkPipelineStageFlags dst_stage = 0;
                    
            if(_old == VK_IMAGE_LAYOUT_UNDEFINED && _new == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                memory_barrier.srcAccessMask = 0;
                memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            } else if(_old == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _new == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            } else {
                VK_BUFFER_ERR("invalid image layout transition");
            }
        
            // Change image layout
            vkCmdPipelineBarrier(tmp_cmd_buf, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &memory_barrier);
            _EndCommandBufferSingleCommand(_dev, _graphics_q, _cmd_pool, tmp_cmd_buf);
        }


        void _CopyBufferToImage(
            VkDevice _dev, 
            VkCommandPool _cmd_pool, 
            VkQueue _graphics_queue, 
            VkBuffer _src, 
            VkImage _dst, 
            uint32_t _width, 
            uint32_t _height,
            uint32_t _array_count) 
        {
            // Begin recording cmd_buf
            VkCommandBuffer tmp_cmd_buf;
            _BeginCommandBufferSingleCommand(_dev, _cmd_pool, tmp_cmd_buf);

            // Set up buffer image copy struct
            VkBufferImageCopy copy_region{};
            copy_region.bufferOffset = 0;
            copy_region.bufferRowLength = 0;
            copy_region.bufferImageHeight = 0;
            copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copy_region.imageSubresource.mipLevel = 0;
            copy_region.imageSubresource.baseArrayLayer = 0;
            copy_region.imageSubresource.layerCount = _array_count;
            copy_region.imageOffset = { 0, 0, 0 };
            copy_region.imageExtent = { _width, _height, 1 };
            
            // Call Vulkan buffer to image copy handler
            vkCmdCopyBufferToImage(tmp_cmd_buf, _src, _dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

            // End recording cmd_buf
            _EndCommandBufferSingleCommand(_dev, _graphics_queue, _cmd_pool, tmp_cmd_buf);
        }


        VkSampler _CreateTextureSampler(VkDevice _dev, float _max_sampler_anisotropy, uint32_t _mip_level) {
            VkSamplerCreateInfo sampler_info = {};
            sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            sampler_info.magFilter = VK_FILTER_LINEAR;
            sampler_info.minFilter = VK_FILTER_LINEAR;
            sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampler_info.anisotropyEnable = VK_TRUE;
            sampler_info.maxAnisotropy = _max_sampler_anisotropy;
            sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            sampler_info.unnormalizedCoordinates = VK_FALSE;
            sampler_info.compareEnable = VK_FALSE;
            sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
            sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            sampler_info.minLod = 0.0f;
            sampler_info.maxLod = static_cast<float>(_mip_level);
            sampler_info.mipLodBias = 0.0f;

            VkSampler sampler = VK_NULL_HANDLE;
            if (vkCreateSampler(_dev, &sampler_info, nullptr, &sampler) != VK_SUCCESS)
                VK_RES_ERR("Failed to create a texture sampler");

            return sampler;
        }


        VkMemoryRequirements _CreateBuffer(VkDevice _dev, VkDeviceSize _size, VkBufferUsageFlags _usage, VkBuffer &_buffer) {
            // Set up buffer createinfo struct 
            VkBufferCreateInfo buffer_createinfo{};
            buffer_createinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_createinfo.size = _size;
            buffer_createinfo.usage = _usage;
            buffer_createinfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            // Call Vulkan buffer creation handler 
            if(vkCreateBuffer(_dev, &buffer_createinfo, nullptr, &_buffer) != VK_SUCCESS)
                VK_BUFFER_ERR("Failed to create a buffer!");

            // Get memory requirements for the buffer
            VkMemoryRequirements memory_requirement;
            vkGetBufferMemoryRequirements(_dev, _buffer, &memory_requirement);

            return memory_requirement;
        }


        void _CopyToBufferMemory(VkDevice _dev, VkDeviceSize _size, const void *_src, VkDeviceMemory _dst, VkDeviceSize _offset) {
            void *buf;
            vkMapMemory(_dev, _dst, _offset, _size, 0, &buf);
                std::memcpy(buf, _src, _size);
            vkUnmapMemory(_dev, _dst);
        }


        void *_CopyToDeviceMemory(VkDevice _dev, VkDeviceSize _size, VkDeviceMemory _src, VkDeviceSize _offset) {
            void *device_data = std::malloc(_size);
            if (!device_data) {
                DENG_ASSERT(false);
                return nullptr;
            }
            void *data = nullptr;

            vkMapMemory(_dev, _src, _offset, _size, 0, &data);
                std::memcpy(device_data, data, _size);
            vkUnmapMemory(_dev, _src);

            return device_data;
        }


        void _CopyBufferToBuffer(VkDevice _dev, VkCommandPool _cmd_pool, VkQueue _graphics_q, VkBuffer _src, VkBuffer _dst, VkDeviceSize _size, 
                                 VkDeviceSize _src_offset, VkDeviceSize _dst_offset)
        {
            // Begin recording cmd_buf
            VkCommandBuffer tmp_cmd_buf = {};
            _BeginCommandBufferSingleCommand(_dev, _cmd_pool, tmp_cmd_buf);

            // Set up copy region 
            VkBufferCopy copy_region{};
            copy_region.srcOffset = _src_offset;
            copy_region.dstOffset = _dst_offset;
            copy_region.size = _size;
            
            // Call Vulkan buffer copy handler
            vkCmdCopyBuffer(tmp_cmd_buf, _src, _dst, 1, &copy_region);

            // End recording cmd_buf
            _EndCommandBufferSingleCommand(_dev, _graphics_q, _cmd_pool, tmp_cmd_buf);
        }


        void _ImplicitDataToBufferCopy(VkDevice _dev, VkPhysicalDevice _gpu, VkCommandPool _cmd_pool, VkQueue _graphics_queue, VkDeviceSize _size, const void *_src, 
                                       VkBuffer _dst, VkDeviceSize _offset) 
        {
            // create a staging buffer to hold all data in
            VkBuffer staging_buffer = VK_NULL_HANDLE;
            VkDeviceMemory staging_buffer_memory = VK_NULL_HANDLE;

            VkMemoryRequirements mem_req = _CreateBuffer(_dev, _size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, staging_buffer);
            _AllocateMemory(_dev, _gpu, mem_req.size, staging_buffer_memory, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            vkBindBufferMemory(_dev, staging_buffer, staging_buffer_memory, 0);

            // FIRST: copy data to staging buffer memory
            // SECOND: copy data from staging buffer memory into main memory buffer
            _CopyToBufferMemory(_dev, _size, _src, staging_buffer_memory, 0);
            _CopyBufferToBuffer(_dev, _cmd_pool, _graphics_queue, staging_buffer, _dst, _size, 0, _offset);

            // destroy staging buffer instances
            vkFreeMemory(_dev, staging_buffer_memory, nullptr);
            vkDestroyBuffer(_dev, staging_buffer, nullptr);
        }


        void _BeginCommandBufferSingleCommand(VkDevice _dev, VkCommandPool _cmd_pool, VkCommandBuffer &_cmd_buf) {
            // Set up cmd_buf allocation info
            VkCommandBufferAllocateInfo cmd_buf_allocinfo = {};
            cmd_buf_allocinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmd_buf_allocinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmd_buf_allocinfo.commandPool = _cmd_pool;
            cmd_buf_allocinfo.commandBufferCount = 1;

            // Allocate cmd_bufs
            vkAllocateCommandBuffers(_dev, &cmd_buf_allocinfo, &_cmd_buf);
            
            // Set up command buffer begin info
            VkCommandBufferBeginInfo cmd_buf_begininfo{};
            cmd_buf_begininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cmd_buf_begininfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            // Begin cmd_bufs
            if(vkBeginCommandBuffer(_cmd_buf, &cmd_buf_begininfo) != VK_SUCCESS)
                VK_RES_ERR("failed to start command recording buffer!");
        }


        void _EndCommandBufferSingleCommand(VkDevice _dev, VkQueue _graphics_q, VkCommandPool _cmd_pool, VkCommandBuffer &_cmd_buf) {
            // Call Vulkan command buffer end handler
            vkEndCommandBuffer(_cmd_buf);

            // Set up submit info 
            VkSubmitInfo submitinfo{};
            submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitinfo.commandBufferCount = 1;
            submitinfo.pCommandBuffers = &_cmd_buf;
            
            // Submit to the graphics queue
            vkQueueSubmit(_graphics_q, 1, &submitinfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(_graphics_q);

            // Cleanup
            vkFreeCommandBuffers(_dev, _cmd_pool, 1, &_cmd_buf);
        }
    }
}
