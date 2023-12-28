// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanGraphicsInstantiator.cpp - Vulkan graphics instantiator class implementation
// author: Karl-Mihkel Ott

#include "deng/ErrorDefinitions.h"
#include "deng/Exceptions.h"
#include "deng/VulkanGraphicsInstantiator.h"
#include <sstream>
#include <iomanip>
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


		GraphicsInstantiator::~GraphicsInstantiator()
		{
			VkDevice hDevice = (*this).GetResource<VkDevice>(QueryResource::VkDevice);
			vkDestroyDevice(hDevice, nullptr);

#ifdef _DEBUG
			PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT =
				(PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_hInstance, "vkDestroyDebugUtilsMessengerEXT");
			
			// check if debug utils messenger was created
			if (m_hDebugUtilsMessenger)
			{
				vkDestroyDebugUtilsMessengerEXT(m_hInstance, m_hDebugUtilsMessenger, nullptr);
			}
#endif
			vkDestroyInstance(m_hInstance, nullptr);
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
			}
			return false;
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
			
			if (vkCreateDebugUtilsMessengerEXT == nullptr)
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
#endif


		bool GraphicsInstantiator::_CheckExtensionSupport(VkPhysicalDevice _hPhysicalDevice, const char* _pExt)
		{
			uint32_t uSupportedExtensionCount = 0;
			vkEnumerateDeviceExtensionProperties(_hPhysicalDevice, nullptr, &uSupportedExtensionCount, nullptr);

			// Get extensions by names
			std::vector<VkExtensionProperties> allSupportedExtensions(uSupportedExtensionCount);
			vkEnumerateDeviceExtensionProperties(_hPhysicalDevice, nullptr, &uSupportedExtensionCount, allSupportedExtensions.data());
		
			for (VkExtensionProperties& extension : allSupportedExtensions)
			{
				const char* szCurrentExtensionName = extension.extensionName;

				if (!std::strcmp(szCurrentExtensionName, _pExt))
				{
					return true;
				}
			}

			return false;
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
			
			// check for queue family support
			uint32_t uQueueFamilySupportMask = _FindQueueFamilies(_hPhysicalDevice);
			uScore += (uQueueFamilySupportMask << 8);

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
				if (!_CheckExtensionSupport(_hPhysicalDevice, pExt))
				{
					return 0;
				}
			}

			return uScore;
		}


		void GraphicsInstantiator::_FindPhysicalDeviceSurfaceProperties(VkPhysicalDevice _hPhysicalDevice)
		{
			// find surface properties
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_hPhysicalDevice, m_hSurface, &m_surfaceCapabilities);
			uint32_t uFormatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(_hPhysicalDevice, m_hSurface, &uFormatCount, nullptr);

			if (!uFormatCount)
			{
				std::stringstream ss;
				ss << "No Vulkan surface formats are available for physical device '" << m_physicalDeviceProperties.deviceName << '\'';
				throw HardwareException(ss.str());
			}

			m_surfaceFormats.clear();
			m_surfaceFormats.resize(uFormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(_hPhysicalDevice, m_hSurface, &uFormatCount, m_surfaceFormats.data());

			uint32_t uPresentModeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(_hPhysicalDevice, m_hSurface, &uPresentModeCount, nullptr);

			// check if present modes are available
			if (!uPresentModeCount)
			{
				std::stringstream ss;
				ss << "No Vulkan presentation modes are available for physical device '" << m_physicalDeviceProperties.deviceName << '\'';
				throw HardwareException(ss.str());
			}

			m_presentModes.clear();
			m_presentModes.resize(uPresentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(_hPhysicalDevice, m_hSurface, &uPresentModeCount, m_presentModes.data());
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

			if (vkEnumeratePhysicalDevices(m_hInstance, &uDeviceCount, devices.data()) != VK_SUCCESS)
			{
				throw HardwareException("vkEnumeratePhysicalDevices(): failed to enumerate physical devices");
			}

			for (size_t i = 0; i < devices.size(); i++)
			{
				uDeviceScore = _ScoreDevice(devices[i]);
				deviceCandidates.insert(std::make_pair(uDeviceScore, devices[i]));
			}

			if (deviceCandidates.empty() || deviceCandidates.rbegin()->first == 0)
			{
				throw HardwareException("No suitable graphics device found.\nUpdate your drivers or try again on different hardware configuration");
			}

			(*this)._SetResource(QueryResource::VkPhysicalDevice, deviceCandidates.rbegin()->second);
			VkPhysicalDevice hPhysicalDevice = (*this).GetResource<VkPhysicalDevice>(QueryResource::VkPhysicalDevice);

			vkGetPhysicalDeviceProperties(hPhysicalDevice, &m_physicalDeviceProperties);
			_FindPhysicalDeviceSurfaceProperties(hPhysicalDevice);
		}
		
		
		void GraphicsInstantiator::_PickPhysicalDeviceStratFirstDiscrete()
		{
			uint32_t uDeviceCount = 0;
			if (vkEnumeratePhysicalDevices(m_hInstance, &uDeviceCount, nullptr) != VK_SUCCESS)
			{
				throw HardwareException("vkEnumeratePhysicalDevices(): failed to enumerate physical devices");
			}

			std::vector<VkPhysicalDevice> devices(uDeviceCount);
			
			if (vkEnumeratePhysicalDevices(m_hInstance, &uDeviceCount, devices.data()) != VK_SUCCESS)
			{
				throw HardwareException("vkEnumeratePhysicalDevices(): failed to retrieve handles for Vulkan capable physical devices");
			}

			size_t uFirstDiscrete = SIZE_MAX;
			size_t uFirstIntegrated = SIZE_MAX;

			for (size_t i = 0; i < devices.size(); i++)
			{
				VkPhysicalDeviceProperties physicalDeviceProperties = {};
				vkGetPhysicalDeviceProperties(devices[i], &physicalDeviceProperties);

				if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					uFirstDiscrete = i;
					m_physicalDeviceProperties = physicalDeviceProperties;
				}
				else if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
				{
					uFirstIntegrated = i;
				}
			}

			if (uFirstDiscrete != SIZE_MAX)
			{
				(*this)._SetResource(QueryResource::VkPhysicalDevice, devices[uFirstDiscrete]);
				VkPhysicalDevice hPhysicalDevice = (*this).GetResource<VkPhysicalDevice>(QueryResource::VkPhysicalDevice);
				_FindPhysicalDeviceSurfaceProperties(hPhysicalDevice);
			}
			else if (uFirstIntegrated != SIZE_MAX)
			{
				WARNME("Device pick strategy FirstDiscrete failed, no discrete graphics device found. Falling back to first integrated strategy.");
				(*this)._SetResource(QueryResource::VkPhysicalDevice, devices[uFirstIntegrated]);
				VkPhysicalDevice hPhysicalDevice = (*this).GetResource<VkPhysicalDevice>(QueryResource::VkPhysicalDevice);
				vkGetPhysicalDeviceProperties(devices[uFirstIntegrated], &m_physicalDeviceProperties);
				_FindPhysicalDeviceSurfaceProperties(hPhysicalDevice);
			}
			else 
			{
				throw HardwareException("Failed to find Vulkan capable graphics device");
			}
		}
		
		
		void GraphicsInstantiator::_PickPhysicalDeviceStratFirstIntegrated()
		{
			uint32_t uDeviceCount = 0;
			if (vkEnumeratePhysicalDevices(m_hInstance, &uDeviceCount, nullptr) != VK_SUCCESS)
			{
				throw HardwareException("vkEnumeratePhysicalDevices(): failed to enumerate physical devices");
			}

			std::vector<VkPhysicalDevice> devices(uDeviceCount);

			if (vkEnumeratePhysicalDevices(m_hInstance, &uDeviceCount, devices.data()) != VK_SUCCESS)
			{
				throw HardwareException("vkEnumeratePhysicalDevices(): failed to retrieve handles for Vulkan capable physical devices");
			}
			
			size_t uFirstIntegrated = SIZE_MAX;
			size_t uFirstDiscrete = SIZE_MAX;

			for (size_t i = 0; i < devices.size(); i++)
			{
				VkPhysicalDeviceProperties physicalDeviceProperties = {};
				vkGetPhysicalDeviceProperties(devices[i], &physicalDeviceProperties);

				if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					uFirstDiscrete = i;
					m_physicalDeviceProperties = physicalDeviceProperties;
				}
				else if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
				{
					uFirstIntegrated = i;
				}
			}

			if (uFirstIntegrated != SIZE_MAX)
			{
				(*this)._SetResource(QueryResource::VkPhysicalDevice, devices[uFirstIntegrated]);
				VkPhysicalDevice hPhysicalDevice = (*this).GetResource<VkPhysicalDevice>(QueryResource::VkPhysicalDevice);
				_FindPhysicalDeviceSurfaceProperties(hPhysicalDevice);
			}
			else if (uFirstDiscrete != SIZE_MAX)
			{
				WARNME("Device pick strategy FirstIntegrated failed, no integrated graphics device found. Falling back to first discrete strategy.");
				(*this)._SetResource(QueryResource::VkPhysicalDevice, devices[uFirstDiscrete]);
				VkPhysicalDevice hPhysicalDevice = (*this).GetResource<VkPhysicalDevice>(QueryResource::VkPhysicalDevice);
				vkGetPhysicalDeviceProperties(devices[uFirstDiscrete], &m_physicalDeviceProperties);
				_FindPhysicalDeviceSurfaceProperties(hPhysicalDevice);
			}
			else
			{
				throw HardwareException("Failed to find Vulkan capable graphics device");
			}
		}
		
		
		void GraphicsInstantiator::_PickPhysicalDevice()
		{
			switch (m_eGPUPickStrategy)
			{
				case GPUPickStrategy::BestScoring:
					_PickPhysicalDeviceStratBestScoring();
					break;

				case GPUPickStrategy::FirstDiscreate:
					_PickPhysicalDeviceStratFirstDiscrete();
					break;

				case GPUPickStrategy::FirstIntegrated:
					_PickPhysicalDeviceStratFirstIntegrated();
					break;

				default:
					DENG_ASSERT(false);
					break;
			}
		}


		uint32_t GraphicsInstantiator::_FindQueueFamilies(VkPhysicalDevice _hPhysicalDevice)
		{
			// Get the total number of queue families
			uint32_t uQueueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(_hPhysicalDevice, &uQueueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilyProperties(uQueueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(_hPhysicalDevice, &uQueueFamilyCount, queueFamilyProperties.data());

			uint32_t uGraphicsQueueIndex = UINT32_MAX, 
					 uComputeQueueIndex = UINT32_MAX,
					 uPresentationQueueIndex = UINT32_MAX;

			uint32_t uQueueFamilySupportMask = 0;

			// Search for appropriate queue family indices
			for (uint32_t i = 0; i < uQueueFamilyCount; i++)
			{
				if (uGraphicsQueueIndex != UINT32_MAX && uComputeQueueIndex != UINT32_MAX && uPresentationQueueIndex != UINT32_MAX)
					break;

				if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					uGraphicsQueueIndex = i;
					uQueueFamilyCount |= (1 << 0);
				}
				if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
				{
					uComputeQueueIndex = i;
					uQueueFamilyCount |= (1 << 1);
				}

				VkBool32 bPresentationSupported = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(_hPhysicalDevice, i, m_hSurface, &bPresentationSupported);
				if (bPresentationSupported)
				{
					uPresentationQueueIndex = i;
					uQueueFamilyCount |= (1 << 2);
				}
			}

			(*this)._SetResource(QueryResource::VkGraphicsQueueFamilyIndex, uGraphicsQueueIndex);
			(*this)._SetResource(QueryResource::VkComputeQueueFamilyIndex, uComputeQueueIndex);
			(*this)._SetResource(QueryResource::VkPresentationQueueFamilyIndex, uPresentationQueueIndex);
		
			return uQueueFamilySupportMask;
		}


		void GraphicsInstantiator::_CreateLogicalDeviceHandle()
		{
			VkPhysicalDevice hPhysicalDevice = (*this).GetResource<VkPhysicalDevice>(QueryResource::VkPhysicalDevice);
			uint32_t uQueueFamilySupportMask = _FindQueueFamilies(hPhysicalDevice);
			if ((uQueueFamilySupportMask & 0b11) != 0b11)
			{
				throw HardwareException("Selected physical device does not support graphics nor presentation queues, thus making rendering impossible. Try again with different hardware configuration");
			}
			else if (!(uQueueFamilySupportMask & (1 << 2)))
			{
				WARNME("Selected physical device does not support compute queues, thus making MultiDrawIndirect impossible");
			}

			std::vector<uint32_t> queueFamilyIndices;
			queueFamilyIndices.reserve(3);
			queueFamilyIndices.push_back((*this).GetResource<uint32_t>(QueryResource::VkGraphicsQueueFamilyIndex));
			queueFamilyIndices.push_back((*this).GetResource<uint32_t>(QueryResource::VkPresentationQueueFamilyIndex));

			if ((*this).GetResource<uint32_t>(QueryResource::VkComputeQueueFamilyIndex) != UINT32_MAX)
			{
				queueFamilyIndices.push_back((*this).GetResource<uint32_t>(QueryResource::VkComputeQueueFamilyIndex));
			}

			// get the amount of unique queue family indices
			std::vector<uint32_t> uniqueQueueFamilyIndices;
			uniqueQueueFamilyIndices.reserve(queueFamilyIndices.size());

			uint32_t uUniqueQueueCount = static_cast<uint32_t>(queueFamilyIndices.size());
			for (size_t i = 0; i < queueFamilyIndices.size(); i++)
			{
				bool bIsUnique = true;
				for (size_t j = i + 1; j < queueFamilyIndices.size(); j++)
				{
					if (queueFamilyIndices[i] == queueFamilyIndices[j])
					{
						uUniqueQueueCount--;
						bIsUnique = true;
					}
				}

				if (bIsUnique)
				{
					uniqueQueueFamilyIndices.push_back(queueFamilyIndices[i]);
				}
			}

			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};
			queueCreateInfos.reserve(uniqueQueueFamilyIndices.size());
			const float fQueuePriority = 1.0f;

			for (size_t i = 0; i < uniqueQueueFamilyIndices.size(); i++)
			{
				queueCreateInfos.emplace_back();
				queueCreateInfos.back().sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfos.back().queueFamilyIndex = uniqueQueueFamilyIndices[i];
				queueCreateInfos.back().queueCount = 1;
				queueCreateInfos.back().pQueuePriorities = &fQueuePriority;
			}

			VkPhysicalDeviceFeatures deviceFeatures = {};
			deviceFeatures.geometryShader = static_cast<VkBool32>(BoolQuery(BoolQueryType::GeometryShader));
			deviceFeatures.tessellationShader = static_cast<VkBool32>(BoolQuery(BoolQueryType::TesselationShader));
			deviceFeatures.multiDrawIndirect = static_cast<VkBool32>(BoolQuery(BoolQueryType::MultiDrawIndirect));
			deviceFeatures.drawIndirectFirstInstance = static_cast<VkBool32>(BoolQuery(BoolQueryType::DrawIndirectFirstInstance));
			deviceFeatures.samplerAnisotropy = static_cast<VkBool32>(BoolQuery(BoolQueryType::SamplerAnisotropy));
			deviceFeatures.multiViewport = static_cast<VkBool32>(BoolQuery(BoolQueryType::MultiViewport));
			
			VkDeviceCreateInfo deviceCreateInfo = {};
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.queueCreateInfoCount = uUniqueQueueCount;
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
			deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
			
			deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(sizeof(m_requiredExtensions) / sizeof(const char*));
			deviceCreateInfo.ppEnabledExtensionNames = m_requiredExtensions;

#ifdef _DEBUG
			if (m_hDebugUtilsMessenger)
			{
				deviceCreateInfo.enabledLayerCount = 1;
				deviceCreateInfo.ppEnabledExtensionNames = &m_szValidationLayerName;
			}
#endif

			VkDevice hDevice = VK_NULL_HANDLE;
			if (vkCreateDevice(hPhysicalDevice, &deviceCreateInfo, nullptr, &hDevice) != VK_SUCCESS)
			{
				throw RendererException("vkCreateDevice(): could not create logical device handle");
			}

			(*this)._SetResource(QueryResource::VkDevice, hDevice);

			VkQueue hGraphicsQueue = VK_NULL_HANDLE, hPresentationQueue = VK_NULL_HANDLE;
			vkGetDeviceQueue(hDevice, (*this).GetResource<uint32_t>(QueryResource::VkGraphicsQueueFamilyIndex), 0, &hGraphicsQueue);
			vkGetDeviceQueue(hDevice, (*this).GetResource<uint32_t>(QueryResource::VkPresentationQueueFamilyIndex), 0, &hPresentationQueue);

			(*this)._SetResource(QueryResource::VkGraphicsQueue, hGraphicsQueue);
			(*this)._SetResource(QueryResource::VkPresentationQueue, hPresentationQueue);

			if ((*this).GetResource<uint32_t>(QueryResource::VkComputeQueue) != UINT32_MAX)
			{
				VkQueue hComputeQueue = VK_NULL_HANDLE;
				vkGetDeviceQueue(hDevice, (*this).GetResource<uint32_t>(QueryResource::VkComputeQueue), 0, &hComputeQueue);
				
				(*this)._SetResource(QueryResource::VkComputeQueue, hComputeQueue);
			}
		}


		const char* GraphicsInstantiator::GetGraphicsCardName()
		{
			return m_physicalDeviceProperties.deviceName;
		}


		bool GraphicsInstantiator::BoolQuery(BoolQueryType _queryType)
		{
			switch (_queryType)
			{
				case BoolQueryType::DrawIndirectFirstInstance:
					return static_cast<bool>(m_physicalDeviceFeatures.drawIndirectFirstInstance);

				case BoolQueryType::GeometryShader:
					return static_cast<bool>(m_physicalDeviceFeatures.geometryShader);

				case BoolQueryType::MultiDrawIndirect:
					return static_cast<bool>(m_physicalDeviceFeatures.multiDrawIndirect);

				case BoolQueryType::MultiViewport:
					return static_cast<bool>(m_physicalDeviceFeatures.multiViewport);

				case BoolQueryType::SamplerAnisotropy:
					return static_cast<bool>(m_physicalDeviceFeatures.samplerAnisotropy);

				case BoolQueryType::TesselationShader:
					return static_cast<bool>(m_physicalDeviceFeatures.tessellationShader);

				default:
					DENG_ASSERT(false);
					break;
			}

			return false;
		}


		uint32_t GraphicsInstantiator::UintQuery(UintQueryType _queryType)
		{
			switch (_queryType)
			{
				case DENG::UintQueryType::APIVersion:
					return m_physicalDeviceProperties.apiVersion;
				
				case DENG::UintQueryType::DriverVersion:
					return m_physicalDeviceProperties.driverVersion;
				
				case DENG::UintQueryType::VendorID:
					return m_physicalDeviceProperties.vendorID;

				case DENG::UintQueryType::DeviceID:
					return m_physicalDeviceProperties.deviceID;

				case DENG::UintQueryType::MaxImageDimension1D:
					return m_physicalDeviceProperties.limits.maxImageDimension1D;

				case DENG::UintQueryType::MaxImageDimension2D:
					return m_physicalDeviceProperties.limits.maxImageDimension2D;

				case DENG::UintQueryType::MaxImageDimension3D:
					return m_physicalDeviceProperties.limits.maxImageDimension3D;

				case DENG::UintQueryType::MaxImageDimensionCube:
					return m_physicalDeviceProperties.limits.maxImageDimensionCube;

				case DENG::UintQueryType::MaxUniformBufferSize:
					return m_physicalDeviceProperties.limits.maxUniformBufferRange;

				case DENG::UintQueryType::MaxStorageBufferSize:
					return m_physicalDeviceProperties.limits.maxStorageBufferRange;

				case DENG::UintQueryType::MaxMemoryAllocationCount:
					return m_physicalDeviceProperties.limits.maxMemoryAllocationCount;

				case DENG::UintQueryType::MaxVertexInputAttributes:
					return m_physicalDeviceProperties.limits.maxVertexInputAttributes;

				case DENG::UintQueryType::MaxVertexInputAttributeOffset:
					return m_physicalDeviceProperties.limits.maxVertexInputAttributeOffset;

				case DENG::UintQueryType::MaxVertexInputBindingStride:
					return m_physicalDeviceProperties.limits.maxVertexInputBindingStride;

				case DENG::UintQueryType::MaxFramebufferWidth:
					return m_physicalDeviceProperties.limits.maxFramebufferWidth;

				case DENG::UintQueryType::MaxFramebufferHeight:
					return m_physicalDeviceProperties.limits.maxFramebufferHeight;

				case DENG::UintQueryType::MaxViewports:
					return m_physicalDeviceProperties.limits.maxViewports;

				case DENG::UintQueryType::MaxDescriptorSetSamplers:
					return m_physicalDeviceProperties.limits.maxDescriptorSetSamplers;

				case DENG::UintQueryType::MaxBoundDescriptorSets:
					return m_physicalDeviceProperties.limits.maxBoundDescriptorSets;

				case DENG::UintQueryType::MaxSamplerAllocationCount:
					return m_physicalDeviceProperties.limits.maxSamplerAllocationCount;

				case DENG::UintQueryType::MaxVertexInputBindings:
					return m_physicalDeviceProperties.limits.maxVertexInputBindings;

				default:
					DENG_ASSERT(false);
					break;
			}

			return 0;
		}


		size_t GraphicsInstantiator::SizeQuery(SizeQueryType _queryType)
		{
			switch (_queryType)
			{
				case DENG::SizeQueryType::MinTexelBufferOffsetAlignment:
					return static_cast<size_t>(m_physicalDeviceProperties.limits.minTexelBufferOffsetAlignment);

				case DENG::SizeQueryType::MinUniformBufferOffsetAlignment:
					return static_cast<size_t>(m_physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
				
				case DENG::SizeQueryType::MinStorageBufferOffsetAlignment:
					return static_cast<size_t>(m_physicalDeviceProperties.limits.minStorageBufferOffsetAlignment);
				
				default:
					DENG_ASSERT(false);
					break;
			}

			return 0;
		}


		float GraphicsInstantiator::FloatQuery(FloatQueryType _queryType)
		{
			switch (_queryType)
			{
				case DENG::FloatQueryType::MaxSamplerLodBias:
					return m_physicalDeviceProperties.limits.maxSamplerLodBias;

				case DENG::FloatQueryType::MaxSamplerAnisotropy:
					return m_physicalDeviceProperties.limits.maxSamplerAnisotropy;

				case DENG::FloatQueryType::MinInterpolationOffset:
					return m_physicalDeviceProperties.limits.minInterpolationOffset;

				case DENG::FloatQueryType::MaxInterpolationOffset:
					return m_physicalDeviceProperties.limits.maxInterpolationOffset;

				case DENG::FloatQueryType::MinPointWidth:
					return m_physicalDeviceProperties.limits.pointSizeRange[0];

				case DENG::FloatQueryType::MaxPointWidth:
					return m_physicalDeviceProperties.limits.pointSizeRange[1];

				case DENG::FloatQueryType::MinLineWidth:
					return m_physicalDeviceProperties.limits.lineWidthRange[0];
				
				case DENG::FloatQueryType::MaxLineWidth:
					return m_physicalDeviceProperties.limits.lineWidthRange[1];
				
				default:
					DENG_ASSERT(false);
					break;
			}

			return 0.f;
		}
	}
}