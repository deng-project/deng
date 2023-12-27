// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanGraphicsInstantiator.cpp - Vulkan graphics instantiator class implementation
// author: Karl-Mihkel Ott

#include "deng/ErrorDefinitions.h"
#include "deng/Exceptions.h"
#include "deng/VulkanGraphicsInstantiator.h"
#include <map>
#include <unordered_map>

namespace DENG
{
	namespace Vulkan
	{
		GraphicsInstantiator::GraphicsInstantiator(Handle<IWindowContext> _hWindow, GPUPickStrategy _eStrategy) :
			IGraphicsInstantiator(_hWindow, _eStrategy)
		{
			_CreateInstance();
			m_hSurface = static_cast<VkSurfaceKHR>(m_hWindow->CreateVulkanSurface(m_hInstance));
			
#ifdef _DEBUG
			_CreateDebugMessenger();
#endif
			_PickPhysicalDevice();
			_CreateLogicalDeviceHandle();
		}

		void GraphicsInstantiator::_CreateInstance()
		{
			VkApplicationInfo appinfo = {};
			appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appinfo.pApplicationName = m_hWindow->GetTitle().c_str();
			appinfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
			appinfo.pEngineName = "DENG";
			appinfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);

			// Set up instance create info
			VkInstanceCreateInfo instanceCreateInfo = {};
			instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceCreateInfo.pApplicationInfo = &appinfo;

			std::vector<const char*> enabledExtensions = m_hWindow->QueryRequiredVulkanExtensions();
#ifdef _DEBUG
			// check if VK_EXT_debug_utils is present in query results
			// if not then add it to required extension vector
			{
				bool bIsDebugUtils = false;
				for (const char* pExtension : enabledExtensions)
				{
					if (!std::strcmp(pExtension, "VK_EXT_debug_utils"))
					{
						bIsDebugUtils = true;
						break;
					}
				}

				if (!bIsDebugUtils)
				{
					enabledExtensions.push_back("VK_EXT_debug_utils");
				}
			}
#endif
			for (const char* pExt : enabledExtensions)
			{
				LOG("Required extension " << pExt);
			}

			instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
			instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();

#ifdef _DEBUG
			// check for validation layer support
			if (!_CheckValidationLayerSupport())
			{
				WARNME("Validation layers are not supported on current machine, although they are implictly used in DEBUG mode.");
			}
			else
			{
				instanceCreateInfo.enabledLayerCount = 1;
				instanceCreateInfo.ppEnabledLayerNames = &m_szValidationLayerName;
			}
#endif
			if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_hInstance) != VK_SUCCESS)
			{
				throw RendererException("vkCreateInstance(): could not create a new Vulkan instance");
			}
		}

#ifdef _DEBUG
		bool GraphicsInstantiator::_CheckValidationLayerSupport()
		{
			uint32_t uLayerCount = 0;
			vkEnumerateInstanceLayerProperties(&uLayerCount, nullptr);

			std::vector<VkLayerProperties> availableLayers(static_cast<size_t>(uLayerCount));
			vkEnumerateInstanceLayerProperties(&uLayerCount, availableLayers.data());

			for (const VkLayerProperties& properties : availableLayers)
			{
				if (!std::strcmp(properties.layerName, m_szValidationLayerName))
				{
					return true;
				}

				return false;
			}
		}


		void GraphicsInstantiator::_CreateDebugMessenger()
		{
			VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
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
			debugMessengerCreateInfo.pfnUserCallback = GraphicsInstantiator::_DebugCallback;
			debugMessengerCreateInfo.pUserData = nullptr;

			PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_hInstance, "vkCreateDebugUtilsMessengerEXT");
			
			if (!vkCreateDebugReportCallbackEXT)
			{
				throw RendererException("Could not load vkCreateDebugUtilsMessengerEXT");
			}

			if (vkCreateDebugUtilsMessengerEXT(m_hInstance, &debugMessengerCreateInfo, nullptr, &m_hDebugUtilsMessenger) != VK_SUCCESS)
			{
				throw RendererException("vkCreateDebugUtilsMessengerEXT(): failed to create debug utils messenger");
			}
		}


		VKAPI_ATTR VkBool32 VKAPI_CALL GraphicsInstantiator::_DebugCallback(SeverityFlags _severity, MessageTypeFlags, const CallbackData* _pCallbackData, void*)
		{
			if (_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			{
				DISPATCH_ERROR_MESSAGE("ValidationLayerError", _pCallbackData->pMessage, ErrorSeverity::CRITICAL_NON_FATAL);
			}
			else if (_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				DISPATCH_ERROR_MESSAGE("ValidationLayerWarning", _pCallbackData->pMessage, ErrorSeverity::WARNING);
			}
			else if (_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			{
				DISPATCH_ERROR_MESSAGE("ValidationLayerInfo", _pCallbackData->pMessage, ErrorSeverity::INFO);
			}
			else if (_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			{
				DISPATCH_ERROR_MESSAGE("ValidationLayerVerbose", _pCallbackData->pMessage, ErrorSeverity::INFO);
			}

			return VK_FALSE;
		}


		uint32_t GraphicsInstantiator::_ScoreDevice(VkPhysicalDevice _hPhysicalDevice)
		{
			uint32_t uScore = 0;
			VkPhysicalDeviceFeatures deviceFeatures = {};
			VkPhysicalDeviceProperties deviceProperties = {};

			vkGetPhysicalDeviceProperties(_hPhysicalDevice, &deviceProperties);
			vkGetPhysicalDeviceFeatures(_hPhysicalDevice, &deviceFeatures);

			if (deviceFeatures.fillModeNonSolid)
				uScore += 500;
			if (deviceFeatures.multiDrawIndirect)
				uScore += 1000;
			if (deviceFeatures.multiViewport)
				uScore += 1000;
			if (deviceFeatures.tessellationShader)
				uScore += 500;
			if (deviceFeatures.wideLines)
				uScore += 500;
			if (deviceFeatures.geometryShader)
				uScore += 500;
			
			// sampler anisotropy is required by DENG
			if (!deviceFeatures.samplerAnisotropy)
				return 0;

			uScore += deviceProperties.limits.maxImageDimension2D;
			uScore += deviceProperties.limits.maxImageDimension3D;
			uScore += deviceProperties.limits.maxMemoryAllocationCount;
			uScore += deviceProperties.limits.maxVertexOutputComponents;
			uScore += deviceProperties.limits.maxDrawIndirectCount;

			for (const char* pExt : m_requiredExtensions)
			{
				// TODO: write extension check functionality
			}
		}


		void GraphicsInstantiator::_PickPhysicalDeviceStratBestScoring()
		{
			uint32_t uDeviceCount = 0;
			uint32_t uDeviceScore = 0;
			if (vkEnumeratePhysicalDevices(m_hInstance, &uDeviceCount, nullptr) != VK_SUCCESS)
			{
				throw HardwareException("vkEnumeratePhysicalDevices(): failed to enumerate physical devices");
			}

			std::vector<VkPhysicalDevice> devices(uDeviceCount);
			std::multimap<uint32_t, VkPhysicalDevice> deviceCandidates;
			std::unordered_map<VkPhysicalDevice, std::vector<VkPresentModeKHR>> presentModes;

			if (vkEnumeratePhysicalDevices(m_hInstance, &uDeviceCount, devices.data()) != VK_SUCCESS)
			{
				throw HardwareException("vkEnumeratePhysicalDevices(): failed to enumerate physical devices");
			}

			for (size_t i = 0; i < devices.size(); i++)
			{
				uDeviceScore = _ScoreDevice(devices[i]);
			}
		}
		
		
		void GraphicsInstantiator::_PickPhysicalDeviceStratFirstDiscreate()
		{
		}
		
		
		void GraphicsInstantiator::_PickPhysicalDeviceStratFirstIntegrated()
		{
		}
		
		
		void GraphicsInstantiator::_PickPhysicalDevice()
		{
			switch (m_eGPUPickStrategy)
			{
				case GPUPickStrategy::BestScoring:
					_PickPhysicalDeviceStratBestScoring();
					break;

				case GPUPickStrategy::FirstDiscreate:
					_PickPhysicalDeviceStratFirstDiscreate();
					break;

				case GPUPickStrategy::FirstIntegrated:
					_PickPhysicalDeviceStratFirstIntegrated();
					break;

				default:
					DENG_ASSERT(false);
					break;
			}
		}
#endif
	}
}