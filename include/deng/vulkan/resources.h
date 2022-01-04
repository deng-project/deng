/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: resources.h - Vulkan resource allocator static classes header file
/// author: Karl-Mihkel Ott


#ifndef __VK_RESOURCES_H
#define __VK_RESOURCES_H

#define DENG_MAX_GPU_NAME_SIZE 256

#ifdef __VK_RESOURCES_CPP
    #include <vector>
    #include <string>
    #include <string.h>
    #include <stdexcept>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <common/hashmap.h>
    #include <data/assets.h>
    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/vulkan/sd.h>
    #include <deng/vulkan/qm.h>
#endif


namespace deng {
    namespace vulkan {

        /// Store information about graphics device
        /// This struct __vk_is used to store information about graphics device properties
        struct __vk_HwInfo {
            deng_ui32_t driver_version;
            deng_ui32_t vk_api_version;
            char *gpu_name;
        };


        /// Get information about graphics hardware
        /// This struct __vk_contains methods to get information about graphics hardware
        struct __vk_HardwareSpecs {
            /// Check if requested extension is supported
            /// This method gets information about supported Vulkan extensions and checks  
            /// if requested extension is one of them
            static deng_bool_t getExtensionSupport (
                const VkPhysicalDevice &gpu, 
                const char *ext_name,
                void *udata
            );


            /// Find device memory type
            /// This method finds appropriate memory type by VkMemoryPropertyFlags
            static deng_ui32_t getMemoryType (
                const VkPhysicalDevice &gpu, 
                deng_ui32_t type_filter, 
                VkMemoryPropertyFlags properties,
                void *udata
            );


            /// Find device score Rate logical device based on its capabilities
            static deng_ui32_t getDeviceScore (
                const VkPhysicalDevice &device, 
                std::vector<const char*> &exts,
                void *udata
            );


            /// Find information about graphics device
            /// This method gets information about graphics device name, Vulkan api
            /// version and driver version
            static __vk_HwInfo getGpuInfo(const VkPhysicalDevice &gpu, void *udata);
        };


        /// Struct for storing buffers This struct __vk_contains all buffers and their memory objects for
        /// vulkan renderer
        struct __vk_BufferData {
            VkBuffer staging_buffer;
            VkDeviceMemory staging_buffer_memory;

            // Device memory for mainly storing texture image data
            VkDeviceMemory img_memory;

            // Main buffer hosts all vertices' and indices' data for assets and ui elements 
            // NOTE: Asset data is always kept on lower memory addresses than UI data, this means that once one of these spaces is filled, 
            // a potential reallocation is triggered
            // NOTE: When offsetting keep in mind these conditions: asset_cap + ui_cap = buffer_cap
            //                                                      ui_offset == asset_cap
            //                                                      asset_offset == 0
            //                                                      asset_size < asset_cap && ui_size < ui_cap
            VkBuffer main_buffer;
            VkDeviceMemory main_buffer_memory;


            // Memory alignment for uniform data looks like that 
            // where n is swapchain image count and m is the amount of texture images
            // n * (sizeof(UniformTransformation) + sizeof(UniformTransformation2D) + sizeof(LightingUniform)) bytes -- 
            // -- All transformation and lighting ubo data
            // n * m * sizeof(__vk_UniformColorData) bytes -- All asset specific color ubo data
            VkBuffer uniform_buffer;
            VkDeviceMemory uniform_buffer_mem;
        };

        
        /// Contains method for memory allocation 
        /// This struct __vk_is inherited to ImageCreator and BufferCreator structs
        struct __vk_MemoryAllocator {
            /// Allocate graphics memory using Vulkan
            /// This method is used to allocate memory for VkBuffer and VkImage objects
            static void allocateMemory (
                const VkDevice &device, 
                const VkPhysicalDevice &gpu,  
                VkDeviceSize size, 
                VkDeviceMemory &memory,
                deng_ui32_t mem_type_bits, 
                VkMemoryPropertyFlags properties,
                void *udata
            );
        };


        /// Container for all VkImage handling methods
        struct __vk_ImageCreator : __vk_MemoryAllocator {
            /// Create new VkImageCreateInfo instance
            /// This method is used to simplify VkImageViewCreateInfo creation
            static VkImageViewCreateInfo getImageViewInfo (
                const VkImage &image, 
                VkFormat format, 
                VkImageAspectFlags aspect_flags,
                deng_ui32_t mip_levels,
                void *udata
            );

                
            /// Create new VkImage instance
            /// This method creates new VkImage instance and returns 
            /// memory allocation requirements for this image
            static VkMemoryRequirements makeImage (
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
                void *udata
            );

            /// Transition VkImage from one layout to another
            /// This method uses VkImageMemoryBarrier to transition image layout to new_layout
            static void transitionImageLayout (
                const VkDevice &device, 
                const VkImage &image, 
                const VkCommandPool &commandpool, 
                const VkQueue &g_queue, 
                VkFormat format, 
                VkImageLayout old_layout, 
                VkImageLayout new_layout,
                deng_ui32_t mip_levels,
                void *udata
            ); 

            /// Copy VkBuffer to VkImage instance
            /// This method copies data from src_buffer to dst_image
            static void cpyBufferToImage (
                const VkDevice &device, 
                const VkCommandPool &commandpool, 
                const VkQueue &g_queue, 
                const VkBuffer &src_buffer, 
                const VkImage &dst_image, 
                deng_ui32_t width, 
                deng_ui32_t height,
                void *udata
            );
        };

        
        /// Container for buffer handling methods
        struct __vk_BufferCreator : __vk_MemoryAllocator {
            /// Create new VkBuffer instance
            /// This method creates new VkImage instance and returns 
            /// memory allocation requirements for this image
            static VkMemoryRequirements makeBuffer (
                const VkDevice &device, 
                const VkPhysicalDevice &gpu, 
                VkDeviceSize size, 
                VkBufferUsageFlags usage, 
                VkBuffer &buffer,
                void *udata
            );
            
            /// Copy data to graphics memory
            /// This method copies size bytes of data from src_data to buf_mem using vkMapMemory
            static void cpyToBufferMem (
                const VkDevice &device, 
                VkDeviceSize size, 
                void *src_data, 
                const VkDeviceMemory &buf_mem, 
                VkDeviceSize offset,
                void *udata
            );


            /// Copy buffer to other buffer
            /// This method copies data from src_buffer to dst_buffer
            static void cpyBufferToBuffer (
                const VkDevice &device, 
                const VkCommandPool &commandpool, 
                const VkQueue &g_queue, 
                const VkBuffer &src_buffer, 
                const VkBuffer &dst_buffer, 
                VkDeviceSize size, 
                VkDeviceSize src_offset,
                VkDeviceSize dst_offset,
                void *udata
            );
        };


        /// Container for single use command buffers' allocation and deallocation
        struct __vk_CommandBufferRecorder {
            /// Start recording commandbuffer
            /// This method allocates and begins recording commandbuffers meant for single use
            static void beginCommandBufferSingleCommand (
                VkDevice device, 
                VkCommandPool commandpool, 
                VkCommandBuffer *p_commandbuffer,
                void *udata
            );

            /// Finish recording commandbuffer
            /// This method end commandbuffer recording, submits it into graphics queue
            /// and frees commandbuffers
            static void endCommandBufferSingleCommand (
                VkDevice device, 
                VkQueue graphics_queue, 
                VkCommandPool commandpool, 
                VkCommandBuffer *p_commandBuffer,
                void *udata
            );
        };
    }
}


#ifdef __VK_RESOURCES_CPP
    #include <deng/vulkan/rend_infos.h>
#endif

#endif
