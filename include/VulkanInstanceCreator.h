/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: VulkanInstanceCreator.h - Vulkan instance creator class header
/// author: Karl-Mihkel Ott


#ifndef VULKAN_INSTANCE_CREATOR_H
#define VULKAN_INSTANCE_CREATOR_H

#ifdef VULKAN_INSTANCE_CREATOR_CPP
    #include <vector>
    #include <array>   
	#include <string>
    #include <string.h>
    #include <map>

    #include <vulkan/vulkan.h>
    #include <common/base_types.h>
    #include <common/hashmap.h>
    #include <common/uuid.h>
    #include <common/err_def.h>

    #include <data/assets.h>
    #include <math/deng_math.h>
    #include <deng/window.h>

    #include <deng/vulkan/resources.h>
    #include <deng/vulkan/sd.h>
    #include <deng/vulkan/qm.h>
    #include <deng/vulkan/rend_infos.h>

#ifdef __DEBUG
    #define PUDATA  &m_udata
#else
    #define PUDATA  NULL
#endif
#endif

namespace DENG {
    namespace Vulkan {

        class InstanceCreator : private __vk_InstanceInfo, private __vk_DeviceInfo {
        private:
            // Supported device properties flags
            deng_bool_t m_tex_linear_filtering_support;

            // Required vulkan extensions
            std::vector<const char*> m_required_extension_names; 
            VkSampleCountFlagBits m_max_sample_count = {};
            VkPhysicalDeviceLimits m_dev_limits = {};
            VkSurfaceCapabilitiesKHR m_surface_capabilities = {};
            const char *m_p_validation_layer = "VK_LAYER_KHRONOS_validation";
#ifdef __DEBUG
            __vk_DebugUserData m_udata;
#endif


        private:
            void __mkInstance(const deng_bool_t enable_vl);
            const deng_bool_t __checkValidationLayerSupport();
            void __findSupportedProperties();
            void __mkDebugMessenger();
            void __selectPhysicalDevice();
            void __mkLogicalDevice(const deng_bool_t enable_vl);
            void __mkWindowSurface();

            
            // Debug messenger
            VkResult __mkDebugMessenger (
                const VkDebugUtilsMessengerCreateInfoEXT *p_msg_createinfo
            );

            static VKAPI_ATTR VkBool32 VKAPI_CALL __debugCallback (
                VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, 
                VkDebugUtilsMessageTypeFlagsEXT message_type, 
                const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data, 
                void *p_user_data
            );

        public:
            __vk_InstanceCreator(Window &win, const deng_bool_t enable_vl );
            static void destroyDebugUtils(VkInstance instance, VkDebugUtilsMessengerEXT messenger);

        public:
            deng_bool_t getLFSupport();
            VkInstance getIns();
            VkDevice getDev();
            VkSampleCountFlagBits getMaxSampleCount();
            VkPhysicalDevice getGpu();
            VkSurfaceKHR getSu();
            VkSurfaceCapabilitiesKHR getSurfaceCapabilities();
            __vk_QueueManager getQFF();
            const VkPhysicalDeviceLimits &getGpuLimits();
            VkDebugUtilsMessengerEXT getDMEXT();
            void *getUserData();
        };
    }
}
#endif
