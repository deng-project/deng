// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanInstanceCreator.cpp - Vulkan instance creator class implementation
// author: Karl-Mihkel Ott

#define VULKAN_INSTANCE_CREATOR_CPP
#include <VulkanInstanceCreator.h>

namespace DENG {

    namespace Vulkan {

        InstanceCreator::InstanceCreator(const Window &_win) : m_window(_win) {
            // needed for swapchain creation later
            _CreateInstance();
            if(m_window.InitVkSurface(m_instance, m_surface) != VK_SUCCESS)
                VK_INSTANCE_ERR("failed to create a window surface!");

#ifdef _DEBUG
            _CreateDebugMessenger();
#endif
            _PickPhysicalDevice();
            _CreateLogicalDeviceHandle();
        }


        InstanceCreator::~InstanceCreator() {}


        void InstanceCreator::_CreateInstance() {
            // Set up Vulkan application info
            VkApplicationInfo appinfo{};
            appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appinfo.pApplicationName = m_window.GetTitle().c_str();
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
            const char **extensions = (const char**) m_window.FindVulkanSurfaceExtensions(&ext_c);

            // Log all extensions to console
            for(size_t i = 0; i < ext_c; i++)
                LOG("Required extension: " + std::string(extensions[i]));

            instance_createinfo.enabledExtensionCount = ext_c;
            instance_createinfo.ppEnabledExtensionNames = extensions;
            LOG("Required extensions count is: " + std::to_string(instance_createinfo.enabledExtensionCount));

            
            // Check for validatation layer support
#ifdef _DEBUG
            if(!_CheckValidationLayerSupport())
                VK_INSTANCE_ERR("validation layers usage specified, but none are available!");

            else if(_CheckValidationLayerSupport()) {
                VkDebugUtilsMessengerCreateInfoEXT debug_createinfo = {};

                // Set up instance info to support validation layers
                instance_createinfo.enabledLayerCount = 1;
                instance_createinfo.ppEnabledLayerNames = &m_validation_layer;
                instance_createinfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_createinfo;

                // Set up debug messenger createinfo
                debug_createinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
                debug_createinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                debug_createinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
                debug_createinfo.pfnUserCallback = InstanceCreator::_DebugCallback;
                debug_createinfo.pUserData = NULL;
            }
#else
            // Set up instance info to not support validation layers
            instance_createinfo.enabledLayerCount = 0;
            instance_createinfo.pNext = NULL;
#endif
            
            // Create new Vulkan instance
            if(vkCreateInstance(&instance_createinfo, NULL, &m_instance) != VK_SUCCESS)
                VK_INSTANCE_ERR("failed to create an instance!");
        }


#ifdef _DEBUG
        bool InstanceCreator::_CheckValidationLayerSupport() {
            const std::string validation_lname = "VK_LAYER_KHRONOS_validation";
            uint32_t layer_count;
            vkEnumerateInstanceLayerProperties(&layer_count, NULL);

            std::vector<VkLayerProperties> available_layers(layer_count);
            vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

            for(const VkLayerProperties &properties : available_layers) {
                if(validation_lname == properties.layerName)
                    return true;
            }

            return false;
        }


        void InstanceCreator::_CreateDebugMessenger() {
            VkDebugUtilsMessengerCreateInfoEXT messenger_createinfo{};
            messenger_createinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            messenger_createinfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            messenger_createinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            messenger_createinfo.pfnUserCallback = InstanceCreator::_DebugCallback;
            messenger_createinfo.pUserData = NULL;
            
            auto debugUtilsMessengerCreator_fun = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");

            if(!debugUtilsMessengerCreator_fun)
                VK_GEN_ERR("could not create debug messenger!");

            debugUtilsMessengerCreator_fun(m_instance, &messenger_createinfo, NULL, &m_debug_messenger);
        }


        VKAPI_ATTR VkBool32 VKAPI_CALL InstanceCreator::_DebugCallback(SeverityFlags _severity, MessageTypeFlags _type, const CallbackData *_callback_data, void *_user) {
            if(_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT && _type != VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
                VK_VAL_LAYER(_callback_data->pMessage);
                DENG_ASSERT(false);
            }

            return VK_FALSE;
        }
#endif


        void InstanceCreator::_FindPhysicalDeviceSurfaceProperties(VkPhysicalDevice _gpu) {
            // Find device capabilities and formats
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_gpu, m_surface, &m_surface_capabilities);
            uint32_t format_count;
            vkGetPhysicalDeviceSurfaceFormatsKHR(_gpu, m_surface, &format_count, NULL);

            if(!format_count) 
                VK_SWAPCHAIN_ERR("no surface formats available!");

            m_surface_formats.clear();
            m_surface_formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(_gpu, m_surface, &format_count, m_surface_formats.data());
            
            uint32_t present_mode_count;
            vkGetPhysicalDeviceSurfacePresentModesKHR(_gpu, m_surface, &present_mode_count, NULL);

            // Check if present modes are available
            if(!present_mode_count) 
                VK_SWAPCHAIN_ERR("No surface present modes available!");

            m_present_modes.clear();
            m_present_modes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(_gpu, m_surface, &present_mode_count, m_present_modes.data());
        }


        void InstanceCreator::_PickPhysicalDevice() {
            uint32_t device_count;
            uint32_t score;
            vkEnumeratePhysicalDevices(m_instance, &device_count, NULL);

            if(device_count == 0)
                VK_INSTANCE_ERR("failed to find graphics cards!");

            std::vector<VkPhysicalDevice> devices(device_count);
            std::multimap<uint32_t, VkPhysicalDevice> device_candidates;
            std::unordered_map<VkPhysicalDevice, std::vector<VkPresentModeKHR>> present_modes;
 
            VkResult result = vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());
            
            if(result != VK_SUCCESS) 
                VK_INSTANCE_ERR("no physical devices found!");

            // Iterate through every potential gpu device
            for(uint32_t i = 0; i < device_count; i++) {
                score = _ScoreDevice(devices[i]);
                _FindPhysicalDeviceSurfaceProperties(devices[i]);
                
                if(!m_present_modes.empty() && !m_surface_formats.empty()) {
                    device_candidates.insert(std::make_pair(score, devices[i]));
                    present_modes[devices[i]] = std::move(m_present_modes);
                }
            }

            // found suitable device
            if(!device_candidates.empty() && device_candidates.rbegin()->first > 0) {
                m_gpu = device_candidates.rbegin()->second;
                _FindPhysicalDeviceSurfaceProperties(m_gpu);
                vkGetPhysicalDeviceProperties(m_gpu, &m_gpu_properties);
                m_present_modes = present_modes[m_gpu];
            }

            else VK_INSTANCE_ERR("failed to find suitable GPU!");

            LOG("Found suitable physical device");
        }


        bool InstanceCreator::_FindQueueFamilies() {
            // Get the total count of queue families
            uint32_t family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &family_count, nullptr);
            
            // Get properties of all queue families
            std::vector<VkQueueFamilyProperties> queue_family_properties(family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &family_count, queue_family_properties.data());

            // Check which family is graphics family
            for(uint32_t i = 0; i < family_count; i++) {
                if(m_presentation_family_index != UINT32_MAX && m_graphics_family_index != UINT32_MAX)
                    break;

                if(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    m_graphics_family_index = i;

                // check if presentation is supported
                VkBool32 presentation_supported;
                vkGetPhysicalDeviceSurfaceSupportKHR(m_gpu, i, m_surface, &presentation_supported);
                if(presentation_supported) m_presentation_family_index = i;
            }

            return m_presentation_family_index != UINT32_MAX && m_graphics_family_index != UINT32_MAX;
        }


        void InstanceCreator::_CreateLogicalDeviceHandle() {
            if (!_FindQueueFamilies()) 
                VK_INSTANCE_ERR("queue supporting GPU not found!");

            uint32_t unique_queue_c;
            VkDeviceQueueCreateInfo queue_create_infos[2] = {};
            uint32_t queue_family_indices[2] = { m_graphics_family_index, m_presentation_family_index };
            const float queue_priority = 1.0f;

            queue_create_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_infos[0].queueFamilyIndex = queue_family_indices[0];
            queue_create_infos[0].queueCount = 1;
            queue_create_infos[0].pQueuePriorities = &queue_priority;

            queue_create_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_infos[1].queueFamilyIndex = queue_family_indices[1];
            queue_create_infos[1].queueCount = 1;
            queue_create_infos[1].pQueuePriorities = &queue_priority;

            if(m_graphics_family_index != m_presentation_family_index)
                unique_queue_c = 2;
            else unique_queue_c = 1;

            VkPhysicalDeviceFeatures device_features = {};
            device_features.samplerAnisotropy = VK_TRUE;

            // Create device createinfo
            VkDeviceCreateInfo logical_device_createinfo = {};
            logical_device_createinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            logical_device_createinfo.queueCreateInfoCount = unique_queue_c;
            logical_device_createinfo.pQueueCreateInfos = queue_create_infos;
            logical_device_createinfo.pEnabledFeatures = &device_features;

            // construct a temporary vector object holding required extension names as pointers
            std::vector<const char*> p_required_ext;
            for(const std::string &ext : m_required_extensions)
                p_required_ext.push_back(ext.c_str());

            logical_device_createinfo.enabledExtensionCount = static_cast<uint32_t>(p_required_ext.size());
            logical_device_createinfo.ppEnabledExtensionNames = p_required_ext.data();

#ifdef _DEBUG
            logical_device_createinfo.enabledLayerCount = 1;
            logical_device_createinfo.ppEnabledLayerNames = &m_validation_layer;
#else
            logical_device_createinfo.enabledLayerCount = 0;
#endif

            if(vkCreateDevice(m_gpu, &logical_device_createinfo, NULL, &m_device) != VK_SUCCESS)
                VK_INSTANCE_ERR("failed to create logical device!");

            vkGetDeviceQueue(m_device, m_graphics_family_index, 0, &m_graphics_queue);
            vkGetDeviceQueue(m_device, m_presentation_family_index, 0, &m_presentation_queue);
        }


        bool InstanceCreator::_CheckExtensionSupport(VkPhysicalDevice _gpu, const std::string &_required_ext) {
            uint32_t supported_count;
            vkEnumerateDeviceExtensionProperties(_gpu, nullptr, &supported_count, nullptr);
            
            // Get extensions by names
            std::vector<VkExtensionProperties> all_extensions(supported_count);
            vkEnumerateDeviceExtensionProperties(_gpu, nullptr, &supported_count, all_extensions.data());
            
            // Iterate through all extensions to find matching one
            for(VkExtensionProperties &extension : all_extensions) {
                const char *current_extenstion_name = extension.extensionName;
                
                if(std::string(current_extenstion_name) == std::string(_required_ext))
                    return true;
            }

            return false;
        }


        uint32_t InstanceCreator::_ScoreDevice(VkPhysicalDevice _gpu) {
            uint32_t score = 0;
            VkPhysicalDeviceFeatures device_features;
            VkPhysicalDeviceProperties device_properties;

            vkGetPhysicalDeviceProperties(_gpu, &device_properties); 
            vkGetPhysicalDeviceFeatures(_gpu, &device_features);

            if(device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
            else return 0;
            if(device_features.fillModeNonSolid) score += 500;

            score += device_properties.limits.maxImageDimension2D;
            score += device_properties.limits.maxImageDimension3D;
            score += device_properties.limits.maxMemoryAllocationCount;
            score += device_properties.limits.maxVertexOutputComponents;

            if(!device_features.geometryShader) 
                score += 500;
            if(!device_features.samplerAnisotropy) 
                return 0;

            // Check if all the extension are supported by the device  
            for(const std::string &extension_name : m_required_extensions) {
                if(!_CheckExtensionSupport(_gpu, extension_name)) {
                    WARNME("Required extension: " + std::string(extension_name) + " is not supported!");
                    return 0;
                }
            }

            return score;
        }
    }
}
