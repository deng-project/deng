// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanInstanceCreator.h - Vulkan instance creator class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_INSTANCE_CREATOR_H
#define VULKAN_INSTANCE_CREATOR_H


#ifdef VULKAN_INSTANCE_CREATOR_CPP
    #include <iostream>
    #include <string>
    #include <array>
    #include <vector>
    #include <map>
    #include <queue>
    #include <variant>
    #include <sstream>
    #include <iomanip>
    #include <unordered_map>
    #include <vulkan/vulkan.h>
#ifdef _WIN32
    #include <wtypes.h>
    #include <vulkan/vulkan_win32.h>
#else
    #include <vulkan/vulkan_xlib.h>
#endif

    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Points.h"
    #include "trs/Quaternion.h"

    #include "das/Api.h"
    #include "das/DasStructures.h"
    #include "das/TextureReader.h"

    #include "deng/Api.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/Exceptions.h"
    #include "deng/ShaderComponent.h"
    #include "deng/IWindowContext.h"
    #include "deng/VulkanHelpers.h"
#endif

namespace DENG {
    namespace Vulkan {

        class InstanceCreator {
            private:
                // vulkan stuff
                VkDevice m_hDevice = VK_NULL_HANDLE;
                VkPhysicalDevice m_hPhysicalDevice = VK_NULL_HANDLE;
                VkInstance m_hInstance = VK_NULL_HANDLE;
                VkSurfaceKHR m_hSurface = VK_NULL_HANDLE;
                VkSurfaceCapabilitiesKHR m_surfaceCapabilities = {};
                std::vector<const char*> m_requiredExtensions = {
                    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                    "VK_KHR_maintenance1"
                };

                PhysicalDeviceInformation m_physicalDeviceInformation = {};

                // queue family indices
                uint32_t m_uGraphicsQueueFamilyIndex = UINT32_MAX;
                uint32_t m_uPresentationQueueFamilyIndex = UINT32_MAX;

                // vulkan queue handles
                VkQueue m_hGraphicsQueue = VK_NULL_HANDLE;
                VkQueue m_hPresentationQueue = VK_NULL_HANDLE;
                
                const char *m_szValidationLayerName = "VK_LAYER_KHRONOS_validation";
                VkDebugUtilsMessengerEXT m_hDebugUtilsMessenger = {};

                // details needed for swapchain creation
                std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
                std::vector<VkPresentModeKHR> m_presentModes;

            private:
                void _CreateInstance(IWindowContext& _window);
#ifdef _DEBUG
                bool _CheckValidationLayerSupport();
                void _CreateDebugMessenger();

                // callback function for validation layer
                typedef VkDebugUtilsMessageSeverityFlagBitsEXT SeverityFlags;
                typedef VkDebugUtilsMessageTypeFlagsEXT MessageTypeFlags;
                typedef VkDebugUtilsMessengerCallbackDataEXT CallbackData;
                static VKAPI_ATTR VkBool32 VKAPI_CALL _DebugCallback(SeverityFlags, MessageTypeFlags, const CallbackData*, void*);
#endif
                void _FindPhysicalDeviceSurfaceProperties(VkPhysicalDevice _hPhysicalDevice);
                void _PickPhysicalDevice();

                // returns true if necessary queue families were found, false otherwise
                bool _FindQueueFamilies();
                void _CreateLogicalDeviceHandle();
                bool _CheckExtensionSupport(VkPhysicalDevice _gpu, const std::string &_required_ext);
                uint32_t _ScoreDevice(VkPhysicalDevice _gpu);

            public:
                InstanceCreator(IWindowContext& _window);
                ~InstanceCreator();

                inline VkDevice GetDevice() const { return m_hDevice; }
                inline VkPhysicalDevice GetPhysicalDevice() const { return m_hPhysicalDevice; }
                inline VkSurfaceKHR GetSurface() const { return m_hSurface; }
                inline uint32_t GetGraphicsFamilyIndex() const { return m_uGraphicsQueueFamilyIndex; };
                inline uint32_t GetPresentationFamilyIndex() const { return m_uPresentationQueueFamilyIndex; }
                inline VkQueue GetGraphicsQueue() const { return m_hGraphicsQueue; }
                inline VkQueue GetPresentationQueue() const { return m_hPresentationQueue; }
                inline const std::vector<VkSurfaceFormatKHR>& GetSurfaceFormats() const { return m_surfaceFormats; }
                inline const std::vector<VkPresentModeKHR>& GetPresentationModes() const { return m_presentModes; }
                inline void UpdateSurfaceProperties() { _FindPhysicalDeviceSurfaceProperties(m_hPhysicalDevice); }
                inline const VkSurfaceCapabilitiesKHR& GetSurfaceCapabilities() const { return m_surfaceCapabilities; }
                inline const PhysicalDeviceInformation& GetPhysicalDeviceInformation() const { return m_physicalDeviceInformation; }
        };
    }
}
#endif
