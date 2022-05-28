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
    #include <unordered_map>
    #include <vulkan/vulkan.h>

    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Points.h>

    #include <Api.h>
    #include <BaseTypes.h>
    #include <ErrorDefinitions.h>
    #include <Window.h>
#endif

namespace DENG {
    namespace Vulkan {

        class InstanceCreator {
            private:
                Window &m_window;

                // vulkan stuff
                VkDevice m_device = {};
                VkPhysicalDevice m_gpu = {};
                VkInstance m_instance = {};
                VkSurfaceKHR m_surface = {};
                VkSurfaceCapabilitiesKHR m_surface_capabilities = {};
                VkPhysicalDeviceProperties m_gpu_properties = {};
                std::vector<std::string> m_required_extensions = {
                    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                    "VK_KHR_maintenance1"
                };

                // details needed for swapchain creation
                std::vector<VkSurfaceFormatKHR> m_surface_formats;
                std::vector<VkPresentModeKHR> m_present_modes;

                // queue family indices
                uint32_t m_graphics_family_index = UINT32_MAX;
                uint32_t m_presentation_family_index = UINT32_MAX;

                // vulkan queue handles
                VkQueue m_graphics_queue;
                VkQueue m_presentation_queue;
#ifdef _DEBUG
                const char *m_validation_layer = "VK_LAYER_KHRONOS_validation";
                VkDebugUtilsMessengerEXT m_debug_messenger = {};
#endif

            private:
                void _CreateInstance();
#ifdef _DEBUG
                bool _CheckValidationLayerSupport();
                void _CreateDebugMessenger();

                // callback function for validation layer
                typedef VkDebugUtilsMessageSeverityFlagBitsEXT SeverityFlags;
                typedef VkDebugUtilsMessageTypeFlagsEXT MessageTypeFlags;
                typedef VkDebugUtilsMessengerCallbackDataEXT CallbackData;
                static VKAPI_ATTR VkBool32 VKAPI_CALL _DebugCallback(SeverityFlags, MessageTypeFlags, const CallbackData*, void*);
#endif
                void _FindPhysicalDeviceSurfaceProperties(VkPhysicalDevice _gpu);
                void _PickPhysicalDevice();

                // returns true if necessary queue families were found, false otherwise
                bool _FindQueueFamilies();
                void _CreateLogicalDeviceHandle();
                bool _CheckExtensionSupport(VkPhysicalDevice _gpu, const std::string &_required_ext);
                uint32_t _ScoreDevice(VkPhysicalDevice _gpu);

            public:
                InstanceCreator(Window &_win);
                ~InstanceCreator();

                // inline methods
                inline VkDevice GetDevice() {
                    return m_device;
                }

                inline VkPhysicalDevice GetPhysicalDevice() {
                    return m_gpu;
                }

                inline VkSurfaceKHR GetSurface() {
                    return m_surface;
                }

                inline uint32_t GetGraphicsFamilyIndex() {
                    return m_graphics_family_index;
                }

                inline uint32_t GetPresentationFamilyIndex() {
                    return m_presentation_family_index;
                }

                inline VkQueue GetGraphicsQueue() {
                    return m_graphics_queue;
                }

                inline VkQueue GetPresentationQueue() {
                    return m_presentation_queue;
                }

                inline std::vector<VkSurfaceFormatKHR> &GetSurfaceFormats() {
                    return m_surface_formats;
                }

                inline std::vector<VkPresentModeKHR> &GetPresentationModes() {
                    return m_present_modes;
                }

                inline void UpdateSurfaceProperties() {
                    _FindPhysicalDeviceSurfaceProperties(m_gpu);
                }

                inline VkSurfaceCapabilitiesKHR &GetSurfaceCapabilities() {
                    return m_surface_capabilities;
                }

                inline uint32_t GetMinimalUniformBufferAlignment() {
                    return m_gpu_properties.limits.minUniformBufferOffsetAlignment;
                }

                inline float GetMaxSamplerAnisotropy() {
                    return m_gpu_properties.limits.maxSamplerAnisotropy;
                }
        };

    }
}
#endif
