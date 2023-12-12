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

            cvar::hash_t m_hshMissing2DTexture = 0;
			cvar::hash_t m_hshMissing3DTexture = 0;

        public:
            IRenderer() = default;
			virtual ~IRenderer() {};
			
            virtual void DeleteTextureHandles() = 0;
            virtual void UpdateViewport(uint32_t _uWidth, uint32_t _uHeight) = 0;
            virtual void DestroyPipeline(cvar::hash_t _hshShader) = 0;
            virtual IFramebuffer* CreateFramebuffer(uint32_t _uWidth, uint32_t _uHeight) = 0;
            virtual IFramebuffer* CreateContext(IWindowContext* _pWindow) = 0;
            virtual IGPUManagedBuffer* CreateManagedBuffer() = 0;
            virtual IGPUImage* CreateImage() = 0;
			virtual bool SetupFrame() = 0;
			virtual void DrawInstance(
                cvar::hash_t _hshMesh,
                cvar::hash_t _hshGraphicsShader,
                cvar::hash_t _hshComputeShader,
                IFramebuffer* _pFramebuffer,
                uint32_t _uInstanceCount,
                uint32_t _uFirstInstance = 0,
                cvar::hash_t _hshMaterial = 0) = 0;
    };
}

#endif
