// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: IRenderer.h - renderer interface class header
// author: Karl-Mihkel Ott

#ifndef IRENDERER_H
#define IRENDERER_H

#include <iostream>
#include <vector>

#include "deng/Api.h"
#include "deng/SID.h"
#include "deng/IWindowContext.h"
#include "deng/IFramebuffer.h"
#include "deng/GPUMemoryAllocator.h"

#ifndef MAX_FRAMES_IN_FLIGHT
#define MAX_FRAMES_IN_FLIGHT 2
#endif

#ifndef DEFAULT_STAGING_BUFFER_SIZE
#define DEFAULT_STAGING_BUFFER_SIZE (1 << 22)
#endif

#ifndef DEFAULT_BUFFER_SIZE
#define DEFAULT_BUFFER_SIZE (1 << 24)
#endif

namespace DENG {

    enum class BufferDataType { Vertex, Index, Uniform };

    class DENG_API IRenderer {
        protected:
			IWindowContext* m_pWindowContext = nullptr;
            GPUMemoryAllocator m_gpuMemoryAllocator;
            std::vector<IFramebuffer*> m_framebuffers;

            hash_t m_hshMissing2DTexture = 0;
			hash_t m_hshMissing3DTexture = 0;

        public:
            IRenderer() = default;
			virtual ~IRenderer() {};
			
            virtual void DeleteTextureHandles() = 0;
            virtual void UpdateViewport(uint32_t _uWidth, uint32_t _uHeight) = 0;
            virtual void DestroyPipeline(hash_t _hshShader) = 0;
            virtual IFramebuffer* CreateFramebuffer(uint32_t _uWidth, uint32_t _uHeight) = 0;
            virtual IFramebuffer* CreateContext(IWindowContext* _pWindow) = 0;
            virtual size_t AllocateMemory(size_t _uSize, BufferDataType _eType) = 0;
			virtual void DeallocateMemory(size_t _uOffset) = 0;
            virtual void UpdateBuffer(const void* _pData, size_t _uSize, size_t _uOffset) = 0;
			virtual bool SetupFrame() = 0;
			virtual void DrawInstance(
                hash_t _hshMesh,
                hash_t _hshShader,
                IFramebuffer* _pFramebuffer,
                uint32_t _uInstanceCount,
                uint32_t _uFirstInstance = 0,
                hash_t _hshMaterial = 0) = 0;
    };
}

#endif
