// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanInstanceCreator.cpp - Vulkan instance creator class implementation
// author: Karl-Mihkel Ott

#define VULKAN_INSTANCE_CREATOR_CPP
#include "deng/VulkanInstanceCreator.h"

namespace DENG {

    namespace Vulkan {

        InstanceCreator::InstanceCreator(IWindowContext& _window) {
            // needed for swapchain creation later
            try {
                _CreateInstance(_window);
                m_hSurface = static_cast<VkSurfaceKHR>(_window.CreateVulkanSurface(m_hInstance));
#ifdef DENG_DEBUG
                _CreateDebugMessenger();
#endif
                _PickPhysicalDevice();
                _CreateLogicalDeviceHandle();
            }
            catch (const HardwareException& e) {
                DISPATCH_ERROR_MESSAGE("HardwareException", e.what(), ErrorSeverity::CRITICAL);
            }
            catch (const RendererException& e) {
                DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL);
            }
        }


        InstanceCreator::~InstanceCreator() {
            vkDestroyDevice(m_hDevice, nullptr);
            vkDestroySurfaceKHR(m_hInstance, m_hSurface, nullptr);
#ifdef DENG_DEBUG
            PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = 
                (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_hInstance, "vkDestroyDebugUtilsMessengerEXT");
            vkDestroyDebugUtilsMessengerEXT(m_hInstance, m_hDebugUtilsMessenger, nullptr);
#endif
            vkDestroyInstance(m_hInstance, nullptr);
        }


        void InstanceCreator::_CreateInstance(IWindowContext& _window) {
            // Set up Vulkan application info
            VkApplicationInfo appinfo = {};
            appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appinfo.pApplicationName = _window.GetTitle().c_str();
            appinfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
            appinfo.pEngineName = "DENG";
            appinfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
            appinfo.apiVersion = VK_API_VERSION_1_0;

            // Set up instance create info
            VkInstanceCreateInfo instanceCreateInfo = {}; 
            instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCreateInfo.pApplicationInfo = &appinfo;


            // Log all extensions to console
            std::vector<const char*> enabledExtensions = _window.QueryRequiredVulkanExtensions();
#ifdef DENG_DEBUG
            // check if VK_EXT_debug_utils is present
            // if not then add it to required extension vector
            {
                bool bIsDebugUtils = false;
                for (const char* pExtension : enabledExtensions) {
                    if (pExtension == "VK_EXT_debug_utils") {
                        bIsDebugUtils = true;
                        break;
                    }
                }

                if (!bIsDebugUtils) {
                    enabledExtensions.push_back("VK_EXT_debug_utils");
                }
            }
#endif


            for (const char *ext : enabledExtensions) {
                LOG("Required extension: " << ext);
            }

            instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
            instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
            
            // Check for validatation layer support
#ifdef DENG_DEBUG
            VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};

            if(!_CheckValidationLayerSupport()) {
                throw RendererException("Validation layer usage is specified, but none are available");
            } else {
                // Set up instance info to support validation layers
                instanceCreateInfo.enabledLayerCount = 1;
                instanceCreateInfo.ppEnabledLayerNames = &m_szValidationLayerName;
                instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugMessengerCreateInfo;

                // Set up debug messenger createinfo
                debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
                debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
                debugMessengerCreateInfo.pfnUserCallback = InstanceCreator::_DebugCallback;
                debugMessengerCreateInfo.pUserData = nullptr;
            }
#else
            // Set up instance info to not support validation layers
            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.pNext = nullptr;
#endif
            
            // Create new Vulkan instance
            if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_hInstance) != VK_SUCCESS) {
                throw RendererException("vkCreateInstace() could not create a new Vulkan instance");
            }
        }


#ifdef DENG_DEBUG
        bool InstanceCreator::_CheckValidationLayerSupport() {
            uint32_t uLayerCount;
            vkEnumerateInstanceLayerProperties(&uLayerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(uLayerCount);
            vkEnumerateInstanceLayerProperties(&uLayerCount, availableLayers.data());

            for(const VkLayerProperties &properties : availableLayers) {
                if(std::string(m_szValidationLayerName) == properties.layerName)
                    return true;
            }

            return false;
        }


        void InstanceCreator::_CreateDebugMessenger() {
            VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
            debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugMessengerCreateInfo.messageSeverity = 
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
            debugMessengerCreateInfo.messageType = 
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
            debugMessengerCreateInfo.pfnUserCallback = InstanceCreator::_DebugCallback;
            debugMessengerCreateInfo.pUserData = nullptr;
            
            PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_hInstance, "vkCreateDebugUtilsMessengerEXT");

            if (!vkCreateDebugUtilsMessengerEXT) {
                throw RendererException("Could not load vkCreateDebugUtilsMessengerEXT");
            }

            if (vkCreateDebugUtilsMessengerEXT(m_hInstance, &debugMessengerCreateInfo, nullptr, &m_hDebugUtilsMessenger))
                throw RendererException("vkCreateDebugUtilsMessengerEXT() failed to create debug utils messenger");
        }


        VKAPI_ATTR VkBool32 VKAPI_CALL InstanceCreator::_DebugCallback(SeverityFlags _severity, MessageTypeFlags _type, const CallbackData* _callback_data, void* _user) {
            HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
            if (_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
                SetConsoleTextAttribute(console, FOREGROUND_RED);
                std::cout << "[ValidationLayerError] " << _callback_data->pMessage << std::endl;
            }
            else if (_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
                SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN);
                std::cout << "[ValidationLayerWarning] " << _callback_data->pMessage << std::endl;
            }
            else if (_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
                std::cout << "[ValidationLayerInfo] " << _callback_data->pMessage << std::endl;
            }
            else if (_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
                std::cout << "[ValidationLayerVerbose] " << _callback_data->pMessage << std::endl;
            }

            SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            return VK_FALSE;
        }
#endif


        void InstanceCreator::_FindPhysicalDeviceSurfaceProperties(VkPhysicalDevice _hPhysicalDevice) {
            // Find device capabilities and formats
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_hPhysicalDevice, m_hSurface, &m_surfaceCapabilities);
            uint32_t uFormatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(_hPhysicalDevice, m_hSurface, &uFormatCount, nullptr);

            if (!uFormatCount) {
                std::stringstream ss;
                ss << "No Vulkan surface formats are available for physical device 0x" <<
                    std::setfill('0') << std::setw(sizeof(VkPhysicalDevice)) << std::hex << _hPhysicalDevice;

                throw HardwareException(ss.str());
            }

            m_surfaceFormats.clear();
            m_surfaceFormats.resize(uFormatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(_hPhysicalDevice, m_hSurface, &uFormatCount, m_surfaceFormats.data());
            
            uint32_t uPresentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(_hPhysicalDevice, m_hSurface, &uPresentModeCount, nullptr);

            // Check if present modes are available
            if (!uPresentModeCount) {
                std::stringstream ss;
                ss << "No Vulkan presentation modes are available for physical device 0x" <<
                    std::setfill('0') << std::setw(sizeof(VkPhysicalDevice)) << std::hex << _hPhysicalDevice;
                throw HardwareException(ss.str());
            }

            m_presentModes.clear();
            m_presentModes.resize(uPresentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(_hPhysicalDevice, m_hSurface, &uPresentModeCount, m_presentModes.data());
        }


        void InstanceCreator::_PickPhysicalDevice() {
            uint32_t uDeviceCount;
            uint32_t uDeviceScore = 0;
            vkEnumeratePhysicalDevices(m_hInstance, &uDeviceCount, nullptr);

            if (uDeviceCount == 0)
                throw HardwareException("Could not find any Vulkan capable graphics devices");

            std::vector<VkPhysicalDevice> devices(uDeviceCount);
            std::multimap<uint32_t, VkPhysicalDevice> deviceCandidates;
            std::unordered_map<VkPhysicalDevice, std::vector<VkPresentModeKHR>> presentModes;
 
            VkResult result = vkEnumeratePhysicalDevices(m_hInstance, &uDeviceCount, devices.data());
            
            if (result != VK_SUCCESS)
                throw HardwareException("Could not get any information about Vulkan capable graphics devices");

            // Iterate through every potential gpu device
            for(uint32_t i = 0; i < uDeviceCount; i++) {
                uDeviceScore = _ScoreDevice(devices[i]);

                try {
                    _FindPhysicalDeviceSurfaceProperties(devices[i]);
                    deviceCandidates.insert(std::make_pair(uDeviceScore, devices[i]));
                    presentModes[devices[i]] = std::move(m_presentModes);
                }
                catch (const HardwareException& e) {
                    DISPATCH_ERROR_MESSAGE("HardwareException", e.what(), ErrorSeverity::WARNING);
                }
            }

            if (deviceCandidates.empty() || deviceCandidates.rbegin()->first == 0)
                throw HardwareException("No suitable graphics device found.\nUpdate your drivers or try with another graphics card.");

            // this is now our algorithm picked graphics card
            m_hPhysicalDevice = deviceCandidates.rbegin()->second; 
            _FindPhysicalDeviceSurfaceProperties(m_hPhysicalDevice); 

            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(m_hPhysicalDevice, &deviceProperties);
            m_physicalDeviceInformation.sPhysicalDeviceName = deviceProperties.deviceName;

            m_physicalDeviceInformation.uApiVersionMajor = VK_API_VERSION_MAJOR(deviceProperties.apiVersion);
            m_physicalDeviceInformation.uApiVersionMinor = VK_API_VERSION_MINOR(deviceProperties.apiVersion);
            m_physicalDeviceInformation.uApiVersionPatch = VK_API_VERSION_PATCH(deviceProperties.apiVersion);
            m_physicalDeviceInformation.uApiVersionVariant = VK_API_VERSION_VARIANT(deviceProperties.apiVersion);

            m_physicalDeviceInformation.uVendorId = deviceProperties.vendorID;
            m_physicalDeviceInformation.uDeviceId = deviceProperties.deviceID;

            m_physicalDeviceInformation.uMinimalUniformBufferAlignment =
                static_cast<uint32_t>(deviceProperties.limits.minUniformBufferOffsetAlignment);
            m_physicalDeviceInformation.fMaxSamplerAnisotropy =
                deviceProperties.limits.maxSamplerAnisotropy;

            switch(deviceProperties.deviceType) {
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    m_physicalDeviceInformation.eDeviceType = PhysicalDeviceType::INTEGRATED_GPU;
                    break;

                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    m_physicalDeviceInformation.eDeviceType = PhysicalDeviceType::DISCREATE_GPU;
                    break;

                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    m_physicalDeviceInformation.eDeviceType = PhysicalDeviceType::VIRTUAL_GPU;
                    break;

                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    m_physicalDeviceInformation.eDeviceType = PhysicalDeviceType::CPU;
                    break;

                default:
                    m_physicalDeviceInformation.eDeviceType = PhysicalDeviceType::OTHER;
                    break;
            };

            m_presentModes = presentModes[m_hPhysicalDevice];
        }


        bool InstanceCreator::_FindQueueFamilies() {
            // Get the total amount of queue families
            uint32_t uQueueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(m_hPhysicalDevice, &uQueueFamilyCount, nullptr);
            
            // Get properties of all queue families
            std::vector<VkQueueFamilyProperties> queueFamilyProperties(uQueueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(m_hPhysicalDevice, &uQueueFamilyCount, queueFamilyProperties.data());

            // Check which family is graphics family
            for(uint32_t i = 0; i < uQueueFamilyCount; i++) {
                if(m_uPresentationQueueFamilyIndex != UINT32_MAX && m_uGraphicsQueueFamilyIndex != UINT32_MAX)
                    break;

                if(queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    m_uGraphicsQueueFamilyIndex = i;

                // check if presentation is supported
                VkBool32 bPresenetationSupported;
                vkGetPhysicalDeviceSurfaceSupportKHR(m_hPhysicalDevice, i, m_hSurface, &bPresenetationSupported);
                if(bPresenetationSupported) 
                    m_uPresentationQueueFamilyIndex = i;
            }

            return m_uPresentationQueueFamilyIndex != UINT32_MAX && m_uGraphicsQueueFamilyIndex != UINT32_MAX;
        }


        void InstanceCreator::_CreateLogicalDeviceHandle() {
            if (!_FindQueueFamilies())
                throw HardwareException("Vulkan queue supporting graphics device not found");

            uint32_t uUniqueQueueCount = 0;
            VkDeviceQueueCreateInfo queue_create_infos[2] = {};
            std::array<VkDeviceQueueCreateInfo, 2> queueCreateInfos = {};
            std::array<uint32_t, 2> queueFamilyIndices = { m_uGraphicsQueueFamilyIndex, m_uPresentationQueueFamilyIndex };
            
            const float fQueuePriority = 1.0f;
            queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfos[0].queueFamilyIndex = queueFamilyIndices[0];
            queueCreateInfos[0].queueCount = 1;
            queueCreateInfos[0].pQueuePriorities = &fQueuePriority;

            queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfos[1].queueFamilyIndex = queueFamilyIndices[1];
            queueCreateInfos[1].queueCount = 1;
            queueCreateInfos[1].pQueuePriorities = &fQueuePriority;

            if(m_uGraphicsQueueFamilyIndex != m_uPresentationQueueFamilyIndex) {
                uUniqueQueueCount = 2;
            } else {
                uUniqueQueueCount = 1;
            }

            VkPhysicalDeviceFeatures deviceFeatures = {};
            deviceFeatures.samplerAnisotropy = VK_TRUE;
            deviceFeatures.geometryShader = VK_TRUE;

            // Create device createinfo
            VkDeviceCreateInfo logicalDeviceCreateInfo = {};
            logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            logicalDeviceCreateInfo.queueCreateInfoCount = uUniqueQueueCount;
            logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
            logicalDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;

            // construct a temporary vector object holding required extension names as pointers
            logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_requiredExtensions.size());
            logicalDeviceCreateInfo.ppEnabledExtensionNames = m_requiredExtensions.data();

#ifdef DENG_DEBUG
            logicalDeviceCreateInfo.enabledLayerCount = 1;
            logicalDeviceCreateInfo.ppEnabledLayerNames = &m_szValidationLayerName;
#else
            logicalDeviceCreateInfo.enabledLayerCount = 0;
#endif

            if (vkCreateDevice(m_hPhysicalDevice, &logicalDeviceCreateInfo, nullptr, &m_hDevice) != VK_SUCCESS)
                throw RendererException("Could not create a logical device");

            vkGetDeviceQueue(m_hDevice, m_uGraphicsQueueFamilyIndex, 0, &m_hGraphicsQueue);
            vkGetDeviceQueue(m_hDevice, m_uPresentationQueueFamilyIndex, 0, &m_hPresentationQueue);
        }


        bool InstanceCreator::_CheckExtensionSupport(VkPhysicalDevice _hPhysicalDevice, const std::string &_sRequiredExtension) {
            uint32_t uSupportedExtensionCount;
            vkEnumerateDeviceExtensionProperties(_hPhysicalDevice, nullptr, &uSupportedExtensionCount, nullptr);
            
            // Get extensions by names
            std::vector<VkExtensionProperties> allExtensions(uSupportedExtensionCount);
            vkEnumerateDeviceExtensionProperties(_hPhysicalDevice, nullptr, &uSupportedExtensionCount, allExtensions.data());
            
            // Iterate through all extensions to find matching one
            for(VkExtensionProperties &extension : allExtensions) {
                const char *szCurrentExtensionName = extension.extensionName;
                
                if(std::string(szCurrentExtensionName) == std::string(_sRequiredExtension))
                    return true;
            }

            return false;
        }


        uint32_t InstanceCreator::_ScoreDevice(VkPhysicalDevice _gpu) {
            uint32_t uScore = 0;
            VkPhysicalDeviceFeatures deviceFeatures;
            VkPhysicalDeviceProperties deviceProperties;

            vkGetPhysicalDeviceProperties(_gpu, &deviceProperties); 
            vkGetPhysicalDeviceFeatures(_gpu, &deviceFeatures);

            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) 
                uScore += 1000;
            if (deviceFeatures.fillModeNonSolid) 
                uScore += 500;
            if (deviceFeatures.geometryShader)
                uScore += 500;

            uScore += deviceProperties.limits.maxImageDimension2D;
            uScore += deviceProperties.limits.maxImageDimension3D;
            uScore += deviceProperties.limits.maxMemoryAllocationCount;
            uScore += deviceProperties.limits.maxVertexOutputComponents;

            if(!deviceFeatures.geometryShader) 
                uScore += 500;
            if(!deviceFeatures.samplerAnisotropy) 
                return 0;

            // Check if all the extension are supported by the device  
            for(const std::string &extension_name : m_requiredExtensions) {
                if(!_CheckExtensionSupport(_gpu, extension_name)) {
                    WARNME("Required extension: " + std::string(extension_name) + " is not supported!");
                    return 0;
                }
            }

            return uScore;
        }
    }
}
