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
    #include "deng/BaseTypes.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Renderer.h"
    #include "deng/HardwareInfo.h"
#endif

namespace DENG {
    namespace Vulkan {

        class InstanceCreator {
            private:
                const DENG::RendererConfig &m_config;

                // vulkan stuff
                VkDevice m_device = VK_NULL_HANDLE;
                VkPhysicalDevice m_gpu = VK_NULL_HANDLE;
                VkInstance m_instance = VK_NULL_HANDLE;
                VkSurfaceKHR m_surface = VK_NULL_HANDLE;
                VkSurfaceCapabilitiesKHR m_surface_capabilities = {};
                std::vector<std::string> m_required_extensions = {
                    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                    "VK_KHR_maintenance1"
                };

                HardwareInfo m_gpu_info = {};

                // queue family indices
                uint32_t m_graphics_family_index = UINT32_MAX;
                uint32_t m_presentation_family_index = UINT32_MAX;

                // device limits
                uint32_t m_minimal_uniform_buffer_alignment = 0;
                float m_max_sampler_anisotropy = 0.0f;

                // vulkan queue handles
                VkQueue m_graphics_queue;
                VkQueue m_presentation_queue;
#ifdef __DEBUG
                const char *m_validation_layer = "VK_LAYER_KHRONOS_validation";
                VkDebugUtilsMessengerEXT m_debug_messenger = {};
#endif

                // details needed for swapchain creation
                std::vector<VkSurfaceFormatKHR> m_surface_formats;
                std::vector<VkPresentModeKHR> m_present_modes;

            private:
                void _CreateInstance();
                VkResult _CreateNativeSurface();
#ifdef __DEBUG
                bool _CheckValidationLayerSupport();
                void _CreateDebugMessenger();

                // callback function for validation layer
                typedef VkDebugUtilsMessageSeverityFlagBitsEXT SeverityFlags;
                typedef VkDebugUtilsMessageTypeFlagsEXT MessageTypeFlags;
                typedef VkDebugUtilsMessengerCallbackDataEXT CallbackData;
                static VKAPI_ATTR VkBool32 VKAPI_CALL _DebugCallback(SeverityFlags, MessageTypeFlags, const CallbackData*, void*);
#endif
                void _FindPhysicalDeviceSurfaceProperties(VkPhysicalDevice _gpu, bool _ignore_no_formats);
                void _PickPhysicalDevice();

                // returns true if necessary queue families were found, false otherwise
                bool _FindQueueFamilies();
                void _CreateLogicalDeviceHandle();
                bool _CheckExtensionSupport(VkPhysicalDevice _gpu, const std::string &_required_ext);
                uint32_t _ScoreDevice(VkPhysicalDevice _gpu);

            public:
                InstanceCreator(const RendererConfig &_conf);
                ~InstanceCreator();

                // ~~inlined~~ non-inlined methods (temporary workaround)
                VkDevice GetDevice() const;
                VkPhysicalDevice GetPhysicalDevice() const;
                VkSurfaceKHR GetSurface() const;
                uint32_t GetGraphicsFamilyIndex() const;
                uint32_t GetPresentationFamilyIndex() const;
                VkQueue GetGraphicsQueue() const;
                VkQueue GetPresentationQueue() const;
                const std::vector<VkSurfaceFormatKHR> &GetSurfaceFormats() const;
                const std::vector<VkPresentModeKHR> &GetPresentationModes() const;
                void UpdateSurfaceProperties();
                const VkSurfaceCapabilitiesKHR &GetSurfaceCapabilities() const;
                uint32_t GetMinimalUniformBufferAlignment() const;
                float GetMaxSamplerAnisotropy() const;
        };

    }
}
#endif
