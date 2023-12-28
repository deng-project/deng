// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanGraphicsInstantiator.h - Vulkan graphics instantiator class header
// author: Karl-Mihkel Ott

#pragma once

#include <vulkan/vulkan.h>
#include <array>
#include "deng/IGraphicsInstantiator.h"

namespace DENG
{
	namespace Vulkan
	{
		class DENG_API GraphicsInstantiator : public IGraphicsInstantiator
		{
			private:
				VkInstance m_hInstance = VK_NULL_HANDLE;
				VkSurfaceKHR m_hSurface = VK_NULL_HANDLE;
				VkPhysicalDeviceProperties m_physicalDeviceProperties = {};
				VkPhysicalDeviceFeatures m_physicalDeviceFeatures = {};

				VkSurfaceCapabilitiesKHR m_surfaceCapabilities = {};
				std::vector<VkSurfaceFormatKHR> m_surfaceFormats = {};
				std::vector<VkPresentModeKHR> m_presentModes = {};

				const char* m_requiredExtensions[2] = {
					VK_KHR_SWAPCHAIN_EXTENSION_NAME,
					"VK_KHR_maintenance1",
				};

#ifdef _DEBUG
				const char* m_szValidationLayerName = "VK_LAYER_KHRONOS_validation";
				VkDebugUtilsMessengerEXT m_hDebugUtilsMessenger = VK_NULL_HANDLE;
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
				void _FindPhysicalDeviceSurfaceProperties(VkPhysicalDevice _hPhysicalDevice);
				bool _CheckExtensionSupport(VkPhysicalDevice, const char* _pExt);
				uint32_t _ScoreDevice(VkPhysicalDevice _hPhysicalDevice);
				void _PickPhysicalDeviceStratBestScoring();
				void _PickPhysicalDeviceStratFirstDiscrete();
				void _PickPhysicalDeviceStratFirstIntegrated();
				void _PickPhysicalDevice();
				/* returns a bitmask of supported queue families */
				uint32_t _FindQueueFamilies(VkPhysicalDevice _hPhysicalDevice);
				void _CreateLogicalDeviceHandle();

			public:
				GraphicsInstantiator(Handle<IWindowContext> _hWindow, GPUPickStrategy _eStrategy = GPUPickStrategy::BestScoring);
				virtual ~GraphicsInstantiator() override;

				virtual const char* GetGraphicsCardName() override;
				virtual bool BoolQuery(BoolQueryType _queryType) override;
				virtual uint32_t UintQuery(UintQueryType _queryType) override;
				virtual size_t SizeQuery(SizeQueryType _queryType) override;
				virtual float FloatQuery(FloatQueryType _queryType) override;
		};
	}
}