// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IGraphicsInstantiator.h - graphics instantiator interface class header
// author: Karl-Mihkel Ott

#pragma once

#include <array>

#include "deng/Api.h"
#include "deng/Handle.h"
#include "deng/IWindowContext.h"

namespace DENG
{
	enum class QueryResource 
	{
		VkDevice,
		VkPhysicalDevice,
		VkSurfaceKHR,
		VkGraphicsQueueFamilyIndex,
		VkPresentationQueueFamilyIndex,
		VkGraphicsQueue,
		VkPresentationQueue,
		MaxValue
	};

	enum class GPUPickStrategy
	{
		Unknown,
		BestScoring,
		FirstDiscreate,
		LastDiscreate,
		FirstIntegrated,
		LastIntegrated
	};

	enum class UintQueryType
	{
		APIVersion,
		DriverVersion,
		VendorID,
		DeviceID,
		MaxImageDimension1D,
		MaxImageDimension2D,
		MaxImageDimension3D,
		MaxImageDimensionCube,
		MaxUniformBufferSize,
		MaxStorageBufferSize,
		MaxMemoryAllocationCount,
		MaxVertexInputAttributes,
		MaxVertexInputAttributeOffset,
		MaxVertexInputBindingStride,
		MaxFramebufferWidth,
		MaxFramebufferHeight,
		MaxViewports,
		MaxDescriptorSetSamplers, // vulkan
		MaxBoundDescriptorSets, // vulkan
		MaxSamplerAllocationCount, // vulkan
		MaxVertexInputBindings, // vulkan
	};

	enum class SizeQueryType
	{
		MinTexelBufferOffsetAlignment,
		MinUniformBufferOffsetAlignment,
		MinStorageBufferOffsetAlignment,
	};

	enum class FloatQueryType
	{
		MaxSamplerLodBias,
		MaxSamplerAnisotropy,
		MinInterpolationOffset,
		MaxInterpolationOffset,
		MinPointWidth,
		MaxPointWidth,
		MinLineWidth,
		MaxLineWidth,
	};

	class DENG_API IGraphicsInstantiator
	{
		private:
			std::array<std::uintptr_t, static_cast<size_t>(QueryResource::MaxValue)> m_apiHandles{};
			
		protected:
			Handle<IWindowContext> m_hWindow = nullptr;
			GPUPickStrategy m_eGPUPickStrategy = GPUPickStrategy::Unknown;

		protected:
			template <typename T>
			inline void _SetResource(QueryResource _queryResource, T _val)
			{
				m_apiHandles[static_cast<size_t>(_queryResource)] = reinterpret_cast<std::uintptr_t>(_val);
			}

		public:
			IGraphicsInstantiator(Handle<IWindowContext> _hWindow, GPUPickStrategy _eStrategy = GPUPickStrategy::BestScoring) :
				m_hWindow(_hWindow),
				m_eGPUPickStrategy(_eStrategy)
			{
			}
			virtual ~IGraphicsInstantiator() = default;

			template <typename ApiHandle_T>
			inline ApiHandle_T GetResource(QueryResource _queryResource)
			{
				return reinterpret_cast<ApiHandle_T>(m_apiHandles[static_cast<size_t>(_queryResource)]);
			}

			virtual const char* GetGraphicsCardName() = 0;
			virtual uint32_t UintQuery(UintQueryType _queryType) = 0;
			virtual size_t SizeQuery(SizeQueryType _queryType) = 0;
			virtual float FloatQuery(FloatQueryType _queryType) = 0;
	};
}