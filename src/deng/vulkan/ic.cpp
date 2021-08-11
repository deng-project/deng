/// DENG: dynamic engine - small but powerful 3D game engine
/// licence: Apache, see LICENCE file
/// file: ic.cpp - Vulkan instance creator class implementation
/// author: Karl-Mihkel Ott


#define __VK_IC_CPP
#include <deng/vulkan/ic.h>

namespace deng {
    namespace vulkan {
        /***********************************************/
        /************ __vk_InstanceCreator *************/
        /***********************************************/

        __vk_InstanceCreator::__vk_InstanceCreator (
            deng::Window &win, 
            const deng_bool_t enable_vl
        ) : __vk_DeviceInfo(win) {
            m_required_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

            __mkInstance(enable_vl);
            __mkWindowSurface();
            if(enable_vl) __mkDebugMessenger();
            
            __selectPhysicalDevice();
            __findSupportedProperties();
            __mkLogicalDevice(enable_vl);
        }


        /// Create new vulkan instance for renderer 
        void __vk_InstanceCreator::__mkInstance(const deng_bool_t enable_vl) {
            // Set up Vulkan application info
            VkApplicationInfo appinfo{};
            appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appinfo.pApplicationName = m_win.getTitle().c_str();
            appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appinfo.pEngineName = "DENG";
            appinfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
            appinfo.apiVersion = VK_API_VERSION_1_0;

            // Set up instance create info
            VkInstanceCreateInfo instance_createinfo{}; 
            instance_createinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instance_createinfo.pApplicationInfo = &appinfo;

            // Get all required extensions
            size_t ext_c;
            const char **extensions = (const char**) m_win.findVulkanSurfaceExtensions(&ext_c, enable_vl);

            // Log all extensions to console
            for(size_t i = 0; i < ext_c; i++)
                LOG("Require extension: " + std::string(extensions[i]));

            instance_createinfo.enabledExtensionCount = ext_c;
            instance_createinfo.ppEnabledExtensionNames = extensions;
            LOG("Required extensions count is: " + std::to_string(instance_createinfo.enabledExtensionCount));

            VkDebugUtilsMessengerCreateInfoEXT debug_createinfo = {};
            
            // Check for validatation layer support
            if(enable_vl && !__checkValidationLayerSupport())
                VK_INSTANCE_ERR("validation layers usage specified, but none are available!");

            else if(enable_vl && __checkValidationLayerSupport()) {
                // Set up instance info to support validation layers
                instance_createinfo.enabledLayerCount = 1;
                instance_createinfo.ppEnabledLayerNames = &m_p_validation_layer;
                instance_createinfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_createinfo;

                // Set up debug messenger createinfo
                debug_createinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
                debug_createinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                debug_createinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
                debug_createinfo.pfnUserCallback = __vk_InstanceCreator::__debugCallback;
            }

            else if(!enable_vl) {
                // Set up instance info to not support validation layers
                LOG("Vulkan validation layers are disabled");
                instance_createinfo.enabledLayerCount = 0;
                instance_createinfo.pNext = NULL;
            }
            
            // Create new Vulkan instance
            if(vkCreateInstance(&instance_createinfo, NULL, &m_instance) != VK_SUCCESS)
                VK_INSTANCE_ERR("failed to create an instance!");
        }


        /// Check if Vulkan validation layers are available 
        const deng_bool_t __vk_InstanceCreator::__checkValidationLayerSupport() {
            deng_ui32_t layer_count;
            vkEnumerateInstanceLayerProperties(&layer_count, NULL);

            std::vector<VkLayerProperties> available_layers(layer_count);
            vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
            deng_bool_t is_layer = false;

            for(const VkLayerProperties &properties : available_layers) {
                if(!strcmp(m_p_validation_layer, properties.layerName)) {
                    is_layer = true;
                    break;
                }
            }

            return is_layer;
        }


        /// Make debug messenger for Vulkan validation layer 
        void __vk_InstanceCreator::__mkDebugMessenger() {
            VkDebugUtilsMessengerCreateInfoEXT messenger_createinfo{};
            messenger_createinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            messenger_createinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            messenger_createinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            messenger_createinfo.pfnUserCallback = __vk_InstanceCreator::__debugCallback;
            
            auto debugUtilsMessengerCreator_fun = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");

            if(!debugUtilsMessengerCreator_fun)
                VK_GEN_ERR("could not create debug messenger!");

            debugUtilsMessengerCreator_fun (
                m_instance, 
                &messenger_createinfo, 
                NULL, 
                &m_debug_mes
            );
        }


        /// Callback method for Vulkan validation layer debugging 
        VKAPI_ATTR VkBool32 VKAPI_CALL __vk_InstanceCreator::__debugCallback (
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, 
            VkDebugUtilsMessageTypeFlagsEXT message_type, 
            const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data, 
            void *p_user_data
        ) {
            VK_VAL_LAYER(p_callback_data->pMessage);
            return VK_FALSE;
        }

        
        /// Select appropriate graphics device 
        void __vk_InstanceCreator::__selectPhysicalDevice() {
            deng_ui32_t device_count;
            deng_ui32_t score;
            vkEnumeratePhysicalDevices (
                m_instance, 
                &device_count, 
                NULL
            );

            if(device_count == 0)
                VK_INSTANCE_ERR("failed to find graphics cards!");

            std::vector<VkPhysicalDevice> devices(device_count);
            std::multimap<deng_ui32_t, VkPhysicalDevice> device_candidates;
            VkResult result = vkEnumeratePhysicalDevices (
                m_instance, 
                &device_count, 
                devices.data()
            );
            
            if(result != VK_SUCCESS) 
                VK_INSTANCE_ERR("no physical devices found!");

            // Iterate through every potential gpu device
            for(deng_ui32_t i = 0; i < device_count; i++) {
                score = __vk_HardwareSpecs::getDeviceScore (
                    devices[i], 
                    m_required_extension_names
                );
                __vk_SwapChainDetails swapchain_details (
                    devices[i], 
                    m_surface
                );
                
                if(!swapchain_details.getFormats().empty() && !swapchain_details.getPresentModes().empty())
                    device_candidates.insert(std::make_pair(score, devices[i]));
            }

            if(!device_candidates.empty() && device_candidates.rbegin()->first > 0)
                m_gpu = device_candidates.rbegin()->second;

            else 
                VK_INSTANCE_ERR("failed to find suitable GPU!");

            LOG("Found suitable physical device");
        }

        
        /// Create logical Vulkan device 
        void __vk_InstanceCreator::__mkLogicalDevice(const deng_bool_t enable_vl) {
            if (
                !m_qff.findGraphicsFamily(m_gpu) || 
                !m_qff.findPresentSupportFamily(m_gpu, m_surface)
            ) VK_INSTANCE_ERR("queue supporting GPU not found!");

            std::array<VkDeviceQueueCreateInfo, 2> queue_createinfos;
            std::array<deng_ui32_t, 2> queue_families_indexes = {
                m_qff.getGraphicsQFIndex(), 
                m_qff.getPresentQFIndex()
            };

            deng_vec_t queue_priority = 1.0f;

            // Create device queue creatinfos for present and graphics queues
            for(deng_ui32_t i = 0; i < 2; i++) {
                VkDeviceQueueCreateInfo dev_queue_createinfo{};
                dev_queue_createinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                dev_queue_createinfo.queueFamilyIndex = queue_families_indexes[i];
                dev_queue_createinfo.queueCount = 1;
                dev_queue_createinfo.pQueuePriorities = &queue_priority;
                queue_createinfos[i] = dev_queue_createinfo;                
            } 

            VkPhysicalDeviceFeatures device_features{};
            device_features.samplerAnisotropy = VK_TRUE;

            // Create device createinfo
            VkDeviceCreateInfo logical_device_createinfo{};
            logical_device_createinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            logical_device_createinfo.queueCreateInfoCount = static_cast<deng_ui32_t>(queue_createinfos.size());
            logical_device_createinfo.pQueueCreateInfos = queue_createinfos.data();
            logical_device_createinfo.pEnabledFeatures = &device_features;
            logical_device_createinfo.enabledExtensionCount = static_cast<deng_ui32_t>(m_required_extension_names.size());
            logical_device_createinfo.ppEnabledExtensionNames = m_required_extension_names.data();

            if(enable_vl) {
                logical_device_createinfo.enabledLayerCount = 1;
                logical_device_createinfo.ppEnabledLayerNames = &m_p_validation_layer;
            }

            else logical_device_createinfo.enabledLayerCount = 0;

            if(vkCreateDevice(m_gpu, &logical_device_createinfo, NULL, &m_device) != VK_SUCCESS)
                VK_INSTANCE_ERR("failed to create logical device!");

            vkGetDeviceQueue(m_device, m_qff.getGraphicsQFIndex(), 0, &m_qff.graphics_queue );
            vkGetDeviceQueue(m_device, m_qff.getPresentQFIndex(), 0, &m_qff.present_queue );
        }

        
        /// Create window surface with deng surface library 
        void __vk_InstanceCreator::__mkWindowSurface() {
            if(m_win.initVkSurface(m_instance, m_surface) != VK_SUCCESS)
                VK_INSTANCE_ERR("failed to create window surface!");
        }


        /// Find the maximum supported sample count for anti-aliasing (MSAA) 
        void __vk_InstanceCreator::__findSupportedProperties() {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(m_gpu, &props);
            m_dev_limits = props.limits;

            // Find maximum multisample count
            VkSampleCountFlags counts = 
            props.limits.framebufferColorSampleCounts &
            props.limits.framebufferDepthSampleCounts;

            if(counts & VK_SAMPLE_COUNT_64_BIT)
                m_max_sample_count = VK_SAMPLE_COUNT_64_BIT;
            else if(counts & VK_SAMPLE_COUNT_32_BIT)
                m_max_sample_count = VK_SAMPLE_COUNT_32_BIT;
            else if(counts & VK_SAMPLE_COUNT_16_BIT)
                m_max_sample_count = VK_SAMPLE_COUNT_16_BIT;
            else if(counts & VK_SAMPLE_COUNT_8_BIT)
                m_max_sample_count = VK_SAMPLE_COUNT_8_BIT;
            else if(counts & VK_SAMPLE_COUNT_4_BIT)
                m_max_sample_count = VK_SAMPLE_COUNT_4_BIT;
            else if(counts & VK_SAMPLE_COUNT_2_BIT) 
                m_max_sample_count = VK_SAMPLE_COUNT_2_BIT;
            else if(counts & VK_SAMPLE_COUNT_1_BIT)
                m_max_sample_count = VK_SAMPLE_COUNT_1_BIT;
            else m_max_sample_count = VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;

            // Find linear filtering support needed for mipmapping
            VkFormatProperties format_props;
            vkGetPhysicalDeviceFormatProperties (
                m_gpu,
                VK_FORMAT_B8G8R8A8_SRGB,
                &format_props
            );

            if(!(format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
                m_tex_linear_filtering_support = false;
                LOG("Texture linear filtering is not supported by hardware");
            }
            else {
                m_tex_linear_filtering_support = true;
                LOG("Texture linear filtering is supported by hardware");
            }
        }   

        
        /// Destroy all debugging utilities
        void __vk_InstanceCreator::destroyDebugUtils (
            VkInstance instance,
            VkDebugUtilsMessengerEXT messenger
        ) {
            auto messengerDestroyer = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr (
                instance,
                "vkDestroyDebugUtilsMessengerEXT"
            );

            if(messengerDestroyer)
                messengerDestroyer (
                    instance,
                    messenger,
                    NULL
                );
            else
                WARNME("Failed to destroy Vulkan debug utils");
        }



        /// __vk_InstanceCreator getter methods 
        VkInstance __vk_InstanceCreator::getIns() { return m_instance; }
        VkDevice __vk_InstanceCreator::getDev() { return m_device; }
        VkPhysicalDevice __vk_InstanceCreator::getGpu() { return m_gpu; }
        VkSurfaceKHR __vk_InstanceCreator::getSu() { return m_surface; }
        __vk_QueueManager __vk_InstanceCreator::getQFF() { return m_qff; } 
        const VkPhysicalDeviceLimits &__vk_InstanceCreator::getGpuLimits() { return m_dev_limits; } 
        VkDebugUtilsMessengerEXT __vk_InstanceCreator::getDMEXT() { return m_debug_mes; }
        deng_bool_t __vk_InstanceCreator::getLFSupport() { return m_tex_linear_filtering_support; }
        VkSampleCountFlagBits __vk_InstanceCreator::getMaxSampleCount() { return m_max_sample_count; }
    }
}
