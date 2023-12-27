// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: IRenderer.h - renderer interface class header
// author: Karl-Mihkel Ott

#ifndef IRENDERER_H
#define IRENDERER_H

#include <iostream>
#include <vector>

#include <cvar/SID.h>

#include "deng/Api.h"
#include "deng/IWindowContext.h"
#include "deng/IFramebuffer.h"
#include "deng/GPUMemoryAllocator.h"
#include "deng/IGraphicsMemory.h"
#include "deng/IGraphicsPipeline.h"
#include "deng/Handle.h"
#include "deng/Mesh.h"

namespace DENG
{
	class IGraphicsInstantiator;
	class DENG_API IRenderer
	{
		protected:
			Handle<IWindowContext> m_hWindowContext = nullptr;
			Handle<IGraphicsInstantiator> m_hGraphicsInstantiator = nullptr;

		public:
			IRenderer(Handle<IWindowContext> _hWindowContext, Handle<IGraphicsInstantiator> _hGraphicsInstantiator) :
				m_hWindowContext(_hWindowContext),
				m_hGraphicsInstantiator(_hGraphicsInstantiator)
			{
			}
			virtual ~IRenderer() {};

			// virtual void UpdateViewport(uint32_t _uWidth, uint32_t _uHeight) = 0;
			// virtual Handle<IFramebuffer> CreateFramebufferHandle (uint32_t _uWidth, uint32_t _uHeight) = 0;
			// virtual Handle<IFramebuffer> CreateContextHandle (Handle<IWindowContext> _pWindow) = 0;
			// virtual Handle<IGPUManagedBuffer> CreateManagedBufferHandle() = 0;
			// virtual Handle<IGPUImage> CreateImageHandle() = 0;
			// virtual Handle<IGraphicsPipeline> CreateGraphicsPipelineHandle(const char* _szVertexShaderFileName, const char* _szGeometryShaderFileName, const char* _szFragmentShaderFileName) = 0;
			// virtual Handle<IComputePipeline> CreateComputePipelineHandle() = 0;
			virtual bool SetupFrame() = 0;
			virtual void DrawDirect(
				const Batch& _batch,
				Handle<IFramebuffer> _hFramebuffer,
				Handle<IGraphicsPipeline> _hGraphicsPipeline,
				Handle<IGPUManagedBuffer> _hVertexBuffer,
				Handle<IGPUManagedBuffer> _hUniformBuffer) = 0;

			virtual void DrawDirectInstanced(
				const Batch& _batch,
				Handle<IFramebuffer> _hFramebuffer,
				Handle<IGraphicsPipeline> _hGraphicsPipeline,
				Handle<IGPUManagedBuffer> _hVertexBuffer,
				Handle<IGPUManagedBuffer> _hUniformBuffer,
				uint32_t _uInstanceCount
			) = 0;

			// virtual void DrawIndirect(
			// 	const Batch* _pBatches,
			// 	size_t _uBatchCount
			// ) = 0;
	};
}

#endif
