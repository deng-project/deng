// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanQueueManager.h - Vulkan graphics and present queue wrapper class header
// author: Karl-Mihkel Ott

#ifndef VULKAN_QUEUE_MANAGER_H
#define VULKAN_QUEUE_MANAGER_H

#ifdef VULKAN_QUEUE_MANAGER_CPP
    #include <vector>
    #include <vulkan/vulkan.h>
    #include <common/base_types.h>
#endif

namespace DENG {
    namespace Vulkan {

        /*
         * Contains information about queues and their family indices
         */
        class QueueManager {
        private:
            uint32_t m_graphics_family_index;
            uint32_t m_present_family_index;
        
        public:
            VkQueue graphics_queue;
            VkQueue present_queue;

        public:
            /*
             * Search for queue family that supports graphics queues
             * This method checks for all queue families available on given physical 
             * device and searches for one that supports graphics queues.
             */
			bool findGraphicsFamily(VkPhysicalDevice gpu);

            /*
             * Search for queue family that supports present queues
             * This method checks for all queue families available on given physical 
             * device and searches for one that supports present queues
             */
            bool findPresentSupportFamily(const VkPhysicalDevice &gpu, const VkSurfaceKHR &surface);

            uint32_t getGraphicsQFIndex() const;
            uint32_t getPresentQFIndex() const;
        };
    }
}

#endif
