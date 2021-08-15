/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: resources.cpp - Vulkan resource allocator static classes implementation file
/// author: Karl-Mihkel Ott


#define __VK_RESOURCES_CPP
#include <deng/vulkan/resources.h>

namespace deng {
    namespace vulkan {
        /*************************************/
        /******** __vk_HardwareSpecs *********/
        /*************************************/

        /// Check if requested extension is supported
        /// This method gets information about supported Vulkan extensions and checks  
        /// if requested extension is one of them
        deng_bool_t __vk_HardwareSpecs::getExtensionSupport (
            const VkPhysicalDevice &gpu, 
            const char *ext_name,
            void *m_udata
        ) {
            // Count total amount of supported extensions  
            LOG("Finding support for extension " + std::string(ext_name));
            deng_ui32_t extension_count;
            SET_UDATA("vkEnumerateDeviceExtensionProperties");
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extension_count, nullptr);
            
            // Get extensions by names
            std::vector<VkExtensionProperties> all_extensions(extension_count);
            SET_UDATA("vkEnumerateDeviceExtensionProperties");
            vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extension_count, all_extensions.data());
            
            // Iterate through all extension to find matching extension
            for(VkExtensionProperties &extension : all_extensions) {
                const char *current_extenstion_name = extension.extensionName;
                
                if(std::string(current_extenstion_name) == std::string(ext_name)) {
                    LOG("Detected support for " + std::string(current_extenstion_name));
                    return true;
                }
            }

            return false;
        }


        /// Find device memory type
        /// This method finds appropriate memory type by VkMemoryPropertyFlags
        deng_ui32_t __vk_HardwareSpecs::getMemoryType (
            const VkPhysicalDevice &gpu, 
            deng_ui32_t type_filter, 
            VkMemoryPropertyFlags properties,
            void *m_udata
        ) {
            // Get all device memory properties
            VkPhysicalDeviceMemoryProperties memory_properties;
            
            SET_UDATA("vkGetPhysicalDeviceMemoryProperties");
            vkGetPhysicalDeviceMemoryProperties(gpu, &memory_properties);

            for(deng_ui32_t index = 0; index < memory_properties.memoryTypeCount; index++) {
                if(type_filter & (1 << index) && (memory_properties.memoryTypes[index].propertyFlags & properties))
                    return index;
            }

            VK_BUFFER_ERR("failed to find suitable memory type");
        }


        /// Find device score
        /// Rate logical device based on its capabilities
        deng_ui32_t __vk_HardwareSpecs::getDeviceScore (
            const VkPhysicalDevice &gpu, 
            std::vector<const char*> &required_extenstions,
            void *m_udata
        ) {
            deng_ui32_t score = 0;
            VkPhysicalDeviceFeatures device_features;
            VkPhysicalDeviceProperties device_properties;

            SET_UDATA("vkGetPhysicalDeviceProperties");
            vkGetPhysicalDeviceProperties(gpu, &device_properties); 
            SET_UDATA("vkGetPhysicalDeviceFeatures");
            vkGetPhysicalDeviceFeatures(gpu, &device_features);

            if(device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
            
            if(device_features.fillModeNonSolid) score += 500;

            score += device_properties.limits.maxImageDimension2D;
            score += device_properties.limits.maxImageDimension3D;
            score += device_properties.limits.maxMemoryAllocationCount;
            score += device_properties.limits.maxVertexOutputComponents;

            if(!device_features.geometryShader) 
                return 0;
            if(!device_features.samplerAnisotropy) 
                return 0;

            // Check if all the extension are supported by the device  
            for(const char* extension_name : required_extenstions) {
                if(!__vk_HardwareSpecs::getExtensionSupport(gpu, extension_name, m_udata)) {
                    LOG("Required extension: " + std::string(extension_name) + " is not supported!");
                    return 0;
                }
            }

            return score;
        }


        /// Find information about graphics device
        /// This method gets information about graphics device name, Vulkan api
        /// version and driver version
        __vk_HwInfo __vk_HardwareSpecs::getGpuInfo(const VkPhysicalDevice &gpu, void *m_udata) {
            __vk_HwInfo dev_info;
            VkPhysicalDeviceProperties gpu_props = {};
            SET_UDATA("vkGetPhysicalDeviceProperties");
            vkGetPhysicalDeviceProperties(gpu, &gpu_props);

            dev_info.driver_version = gpu_props.driverVersion;
            dev_info.vk_api_version = gpu_props.apiVersion;

            
            dev_info.gpu_name = (char*) calloc(DENG_MAX_GPU_NAME_SIZE, sizeof(char));
            strcpy(dev_info.gpu_name, gpu_props.deviceName);

            return dev_info;
        }


        /***************************************/
        /******** __vk_MemoryAllocator *********/
        /***************************************/
        
        /// Allocate graphics memory using Vulkan
        /// This method is used to allocate memory for VkBuffer and VkImage objects
        void __vk_MemoryAllocator::allocateMemory (
            const VkDevice &device, 
            const VkPhysicalDevice &gpu, 
            VkDeviceSize size,
            VkDeviceMemory &memory,  
            deng_ui32_t mem_type_bits, 
            VkMemoryPropertyFlags properties, 
            void *m_udata
        ) {
            VkMemoryAllocateInfo allocinfo{};
            allocinfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocinfo.allocationSize = size;
            
            allocinfo.memoryTypeIndex = __vk_HardwareSpecs::getMemoryType(gpu, mem_type_bits, properties, m_udata);

            SET_UDATA("vkAllocateMemory");
            if(vkAllocateMemory(device, &allocinfo, nullptr, &memory) != VK_SUCCESS)
                VK_BUFFER_ERR("failed to allocate buffer memory!");
        }


        /***************************************/
        /********** __vk_ImageCreator **********/
        /***************************************/

        
        /// Create new VkImageCreateInfo instance
        /// This method is used to simplify VkImageViewCreateInfo creation
        VkImageViewCreateInfo __vk_ImageCreator::getImageViewInfo (
            const VkImage &image, 
            VkFormat format, 
            VkImageAspectFlags aspect_flags,
            deng_ui32_t mip_levels,
            void *m_udata
        ) {
            VkImageViewCreateInfo createinfo{};
            createinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createinfo.image = image;
            createinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createinfo.format = format;

            createinfo.subresourceRange.aspectMask = aspect_flags;
            createinfo.subresourceRange.baseMipLevel = 0;
            createinfo.subresourceRange.levelCount = mip_levels;
            createinfo.subresourceRange.baseArrayLayer = 0;
            createinfo.subresourceRange.layerCount = 1;
            return createinfo;
        }



        /// Create new VkImage instance
        /// This method creates new VkImage instance and returns VkMemoryRequirements for this image
        VkMemoryRequirements __vk_ImageCreator::makeImage (
            const VkDevice &device, 
            const VkPhysicalDevice &gpu, 
            VkImage &image, 
            deng_ui32_t width, 
            deng_ui32_t height, 
            deng_ui32_t mip_levels,
            VkFormat format, 
            VkImageTiling tiling, 
            VkImageUsageFlags usage,
            VkSampleCountFlagBits sample_c,
            void *m_udata
        ) {
            // Set up image createinfo
            VkImageCreateInfo image_createinfo{};
            image_createinfo.sType  = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_createinfo.imageType = VK_IMAGE_TYPE_2D;
            image_createinfo.extent.width = width;
            image_createinfo.extent.height = height;
            image_createinfo.extent.depth = 1;
            image_createinfo.mipLevels = mip_levels;
            image_createinfo.arrayLayers = 1;
            image_createinfo.format = format;
            image_createinfo.tiling = tiling;
            image_createinfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            image_createinfo.usage = usage;
            image_createinfo.samples = sample_c;
            image_createinfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            // Create image
            SET_UDATA("vkCreateImage");
            if(vkCreateImage(device, &image_createinfo, nullptr, &image) != VK_SUCCESS) 
                VK_BUFFER_ERR("failed to create image!");

            // Get memory requirements for this image
            VkMemoryRequirements memory_requirement;
            SET_UDATA("vkGetImageMemoryRequirements");
            vkGetImageMemoryRequirements(device, image, &memory_requirement);
            
            return memory_requirement;
        }


        /// Transition VkImage from one layout to another
        /// This method uses VkImageMemoryBarrier to transition image layout to new_layout
        void __vk_ImageCreator::transitionImageLayout (
            const VkDevice &device, 
            const VkImage &image, 
            const VkCommandPool &cmd_pool, 
            const VkQueue &g_queue, 
            VkFormat format, 
            VkImageLayout old_layout, 
            VkImageLayout new_layout,
            deng_ui32_t mip_levels,
            void *m_udata
        ) {
            // Begin recording cmd_buf
            VkCommandBuffer tmp_cmd_buf;
            __vk_CommandBufferRecorder::beginCommandBufferSingleCommand(device, cmd_pool, &tmp_cmd_buf, m_udata);

            // Set up image memory barrier struct
            VkImageMemoryBarrier memory_barrier{};
            memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            memory_barrier.oldLayout = old_layout;
            memory_barrier.newLayout = new_layout;
            memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            memory_barrier.image = image;
            memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            memory_barrier.subresourceRange.baseMipLevel = 0;
            memory_barrier.subresourceRange.levelCount = mip_levels;
            memory_barrier.subresourceRange.baseArrayLayer = 0;
            memory_barrier.subresourceRange.layerCount = 1;

            // Set up pipeline stage flags
            VkPipelineStageFlags src_stage;
            VkPipelineStageFlags dst_stage;
                    
            if(old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                memory_barrier.srcAccessMask = 0;
                memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            }

            else if(old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            }

            else 
                VK_BUFFER_ERR("invalid image layout transition");
        
            // Change image layout
            SET_UDATA("vkCmdPipelineBarrier");
            vkCmdPipelineBarrier(tmp_cmd_buf, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &memory_barrier);

            __vk_CommandBufferRecorder::endCommandBufferSingleCommand(device, g_queue, cmd_pool, &tmp_cmd_buf, m_udata);
        }


        /// Copy VkBuffer to VkImage instance
        /// This method copies data from src_buffer to dst_image
        void __vk_ImageCreator::cpyBufferToImage (
            const VkDevice &device, 
            const VkCommandPool &cmd_pool, 
            const VkQueue &g_queue, 
            const VkBuffer &src_buffer, 
            const VkImage &dst_image, 
            deng_ui32_t width, 
            deng_ui32_t height,
            void *m_udata
        ) {
            // Begin recording cmd_buf
            VkCommandBuffer tmp_cmd_buf;
            __vk_CommandBufferRecorder::beginCommandBufferSingleCommand(device, cmd_pool, &tmp_cmd_buf, m_udata);

            // Set up buffer image copy struct
            VkBufferImageCopy copy_region{};
            copy_region.bufferOffset = 0;
            copy_region.bufferRowLength = 0;
            copy_region.bufferImageHeight = 0;
            copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copy_region.imageSubresource.mipLevel = 0;
            copy_region.imageSubresource.baseArrayLayer = 0;
            copy_region.imageSubresource.layerCount = 1;
            copy_region.imageOffset = {0, 0, 0};
            copy_region.imageExtent = {width, height, 1};
            
            // Call Vulkan buffer to image copy handler
            SET_UDATA("vkCmdCopyBufferToImage");
            vkCmdCopyBufferToImage(tmp_cmd_buf, src_buffer, dst_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

            // End recording cmd_buf
            __vk_CommandBufferRecorder::endCommandBufferSingleCommand(device, g_queue, cmd_pool, &tmp_cmd_buf, m_udata);
        }


        /***************************************/
        /********* __vk_BufferCreator **********/
        /***************************************/
        
        /*
         * Create new VkBuffer instance
         * This method creates new VkImage instance and returns 
         * memory allocation requirements for this image
         */
        VkMemoryRequirements __vk_BufferCreator::makeBuffer (
            const VkDevice &device, 
            const VkPhysicalDevice &gpu, 
            VkDeviceSize size, 
            VkBufferUsageFlags usage, 
            VkBuffer &buffer,
            void *m_udata
        ) {
            // Set up buffer createinfo struct 
            VkBufferCreateInfo buffer_createinfo{};
            buffer_createinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_createinfo.size = size;
            buffer_createinfo.usage = usage;
            buffer_createinfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            // Call Vulkan buffer creation handler 
            SET_UDATA("vkCreateBuffer");
            if(vkCreateBuffer(device, &buffer_createinfo, nullptr, &buffer) != VK_SUCCESS)
                VK_BUFFER_ERR("Failed to create a buffer!");

            // Get memory requirements for the buffer
            VkMemoryRequirements memory_requirement;
            SET_UDATA("vkGetBufferMemoryRequirements");
            vkGetBufferMemoryRequirements(device, buffer, &memory_requirement);

            return memory_requirement;
        }


        /// Copy data to graphics memory
        /// This method copies size bytes of data from src_data to buf_mem using vkMapMemory
        void __vk_BufferCreator::cpyToBufferMem (
            const VkDevice &device, 
            VkDeviceSize size, 
            void *src_data, 
            const VkDeviceMemory &buf_mem, 
            VkDeviceSize offset,
            void *m_udata
        ) {
            void *buf;
            SET_UDATA("vkMapMemory");
            vkMapMemory(device, buf_mem, offset, size, 0, &buf);
                memcpy(buf, src_data, size);
            SET_UDATA("vkUnmapMemory");
            vkUnmapMemory(device, buf_mem);
        }


        /// Copy buffer to other buffer
        /// This method copies data from src_buffer to dst_buffer
        void __vk_BufferCreator::cpyBufferToBuffer (
            const VkDevice &device, 
            const VkCommandPool &cmd_pool, 
            const VkQueue &g_queue, 
            const VkBuffer &src_buffer, 
            const VkBuffer &dst_buffer, 
            VkDeviceSize size, 
            VkDeviceSize src_offset,
            VkDeviceSize dst_offset,
            void *m_udata
        ) {
            // Begin recording cmd_buf
            VkCommandBuffer tmp_cmd_buf = {};
            __vk_CommandBufferRecorder::beginCommandBufferSingleCommand(device, cmd_pool, &tmp_cmd_buf, m_udata);

            // Set up copy region 
            VkBufferCopy copy_region{};
            copy_region.srcOffset = src_offset;
            copy_region.dstOffset = dst_offset;
            copy_region.size = size;
            
            // Call Vulkan buffer copy handler
            vkCmdCopyBuffer(tmp_cmd_buf, src_buffer, dst_buffer, 1, &copy_region);

            // End recording cmd_buf
            __vk_CommandBufferRecorder::endCommandBufferSingleCommand(device, g_queue, cmd_pool, &tmp_cmd_buf, m_udata);
        }


        /***********************************************/
        /********* __vk_CommandBufferRecorder **********/
        /***********************************************/


        /*
         * Start recording cmd_buf
         * This method allocates and begins recording cmd_bufs meant for single use
         */
        void __vk_CommandBufferRecorder::beginCommandBufferSingleCommand (
            VkDevice device, 
            VkCommandPool cmd_pool, 
            VkCommandBuffer *p_cmd_buf,
            void *m_udata
        ) {
            // Set up cmd_buf allocation info
            VkCommandBufferAllocateInfo cmd_buf_allocinfo{};
            cmd_buf_allocinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmd_buf_allocinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmd_buf_allocinfo.commandPool = cmd_pool;
            cmd_buf_allocinfo.commandBufferCount = 1;

            // Allocate cmd_bufs
            SET_UDATA("vkAllocateCommandBuffers");
            vkAllocateCommandBuffers(device, &cmd_buf_allocinfo, p_cmd_buf);
            
            // Set up command buffer begin info
            VkCommandBufferBeginInfo cmd_buf_begininfo{};
            cmd_buf_begininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cmd_buf_begininfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            // Begin cmd_bufs
            SET_UDATA("vkBeginCommandBuffer");
            if(vkBeginCommandBuffer(*p_cmd_buf, &cmd_buf_begininfo) != VK_SUCCESS)
                VK_RES_ERR("failed to start command recording buffer!");
        }


        /// Finish recording cmd_buf
        /// This method end cmd_buf recording, submits it into graphics queue
        /// and frees cmd_bufs
        void __vk_CommandBufferRecorder::endCommandBufferSingleCommand (
            VkDevice device, 
            VkQueue g_queue, 
            VkCommandPool cmd_pool, 
            VkCommandBuffer *p_cmd_buf,
            void *m_udata
        ) {
            // Call Vulkan command buffer end handler
            SET_UDATA("vkEndCommandBuffer");
            vkEndCommandBuffer(*p_cmd_buf);

            // Set up submit info 
            VkSubmitInfo submitinfo{};
            submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitinfo.commandBufferCount = 1;
            submitinfo.pCommandBuffers = p_cmd_buf;
            
            // Submit to the graphics queue
            SET_UDATA("vkQueueSubmit");
            vkQueueSubmit(g_queue, 1, &submitinfo, VK_NULL_HANDLE);
            SET_UDATA("vkQueueWaitIdle");
            vkQueueWaitIdle(g_queue);

            // Cleanup
            SET_UDATA("vkFreeCommandBuffers");
            vkFreeCommandBuffers(device, cmd_pool, 1, p_cmd_buf);
        }
    }
}
