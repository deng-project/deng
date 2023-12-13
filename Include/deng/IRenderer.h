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

#ifndef MAX_FRAMES_IN_FLIGHT
#define MAX_FRAMES_IN_FLIGHT 2
#endif

#ifndef DEFAULT_STAGING_BUFFER_SIZE
#define DEFAULT_STAGING_BUFFER_SIZE (1 << 22)
#endif

#ifndef DEFAULT_BUFFER_SIZE
#define DEFAULT_BUFFER_SIZE (1 << 24)
#endif


namespace DENG
{
	enum class BufferDataType { Vertex, Index, Uniform };

	class DENG_API IRenderer
	{
		protected:
			IWindowContext* m_pWindowContext = nullptr;
			GPUMemoryAllocator m_gpuMemoryAllocator;
			std::vector<IFramebuffer*> m_framebuffers;

			Handle<IGPUImage> m_hMissingTexture2D;
			Handle<IGPUImage> m_hMissingTexture3D;

		public:
			IRenderer() = default;
			virtual ~IRenderer() {};

			virtual void UpdateViewport(uint32_t _uWidth, uint32_t _uHeight) = 0;
			virtual Handle<IFramebuffer> CreateFramebufferHandle (uint32_t _uWidth, uint32_t _uHeight) = 0;
			virtual Handle<IFramebuffer> CreateContextHandle (Handle<IWindowContext> _pWindow) = 0;
			virtual Handle<IGPUManagedBuffer> CreateManagedBufferHandle() = 0;
			virtual Handle<IGPUImage> CreateImageHandle() = 0;
			virtual Handle<IGraphicsPipeline> CreateGraphicsPipelineHandle(const char* _szVertexShaderFileName, const char* _szGeometryShaderFileName, const char* _szFragmentShaderFileName) = 0;
			// virtual Handle<IComputePipeline> CreateComputePipelineHandle() = 0;
			virtual bool SetupFrame() = 0;
			virtual void DirectDraw(
				const MeshPass& _meshPass,
				Handle<IFramebuffer> _hFramebuffer,
				Handle<IGraphicsPipeline> _hPipeline,
				Handle<IGPUManagedBuffer> _hVertexBuffer,
				Handle<IGPUManagedBuffer> _hUniformBuffer) = 0;
	};
}

#endif
