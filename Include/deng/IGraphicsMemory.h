// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: IGraphicsMemory.h - abstractions for allocating and managing memory on GPU
// author: Karl-Mihkel Ott

#pragma once

#include <cstdint>
#include "deng/Api.h"
#include "deng/GPUMemoryAllocator.h"

namespace DENG 
{

	enum class MemoryPropertyFlags : uint8_t 
	{
		GPUOnly,
		CPUVisible,
		CPUCoherent,
		CPUCached
	};

	enum BufferUsageFlagBits_T : uint8_t 
	{
		BufferUsageFlagBit_CopySource = (1 << 0),
		BufferUsageFlagBit_CopyDestination = (1 << 1),
		BufferUsageFlagBit_Indices = (1 << 2),
		BufferUsageFlagBit_Vertices = (1 << 3),
		BufferUsageFlagBit_Uniform = (1 << 4),
		BufferUsageFlagBit_StorageBuffer = (1 << 5),
		BufferUsageFlagBit_IndirectBuffer = (1 << 6),
		BufferUsageFlagBit_StorageTexels = (1 << 7)
	};

	typedef uint8_t BufferUsageFlagBits;

	/// <summary>
	///   This class is responsible for giving a low level interface for handling graphics memory
	///   Memory allocated with this class needs to be freed manually and a great care needs to be taken to avoid memory leaks.
	///   Use this class only when necessary! Otherwise use IGPUManagedBufferAllocator instead.
	/// </summary>
	class DENG_API IGPUBuffer 
	{
		protected:
			MemoryPropertyFlags m_ePropertyFlags;
			BufferUsageFlagBits m_bBufferUsageFlags;
			size_t m_uLength = 0;

		protected:
			virtual void Allocate(size_t _uLength) = 0;
			virtual void Free() = 0;

		public:
			IGPUBuffer(MemoryPropertyFlags _ePropertyFlags, BufferUsageFlagBits _bBufferUsage) :
				m_ePropertyFlags(_ePropertyFlags),
				m_bBufferUsageFlags(_bBufferUsage) {}
			virtual void* MapMemory(size_t _uOffset, size_t _uSize) = 0;
			virtual void UnmapMemory() = 0;

			inline bool IsAllocated() { return m_uLength != 0; }
			inline size_t GetBufferLength() { return m_uLength; }
	};


	class DENG_API IGPUManagedBuffer : public IGPUBuffer 
	{
		protected:
			GPUMemoryAllocator m_memoryAllocator;
		
		public:
			IGPUManagedBuffer(MemoryPropertyFlags _ePropertyFlags, BufferUsageFlagBits _bBufferUsage) :
				IGPUBuffer(_ePropertyFlags, _bBufferUsage) {}
			
			virtual void Copy(IGPUManagedBuffer* _pOther, size_t _uDstOffset, size_t _uSrcOffset, size_t _uSize) = 0;
			virtual void Reserve(size_t _uCapacity) = 0;
			virtual size_t SubAllocate(size_t _uSize, size_t _uAlignment) = 0;
			virtual void SubDeallocate(size_t _uOffset) = 0;
	};


#define COLOR_RGBA(r, g, b, a) (((r%256) << 24) | ((g%256) << 16) | ((b%256) << 8) | (a%256))
#define COLOR_RGB(r, g, b) (((r%256) << 16) | ((g%256) << 8) | (b%256))
#define COLOR_RG(r, g) (())

	enum class TextureType : uint8_t 
	{
		None,
		Image_1D,
		Image_2D,
		Image_3D_Array,
		Image_3D,
		InternalFramebufferImage_2D,
		InternalFramebufferImage_3D,
		SwapchainImage
	};

	class DENG_API IGPUImage 
	{
		private:
			uint32_t m_uWidth = 0;
			uint32_t m_uHeight = 0;
			TextureType m_textureType = TextureType::None;

		public:
			IGPUImage() = default;
			virtual void CreateDummy(uint32_t _uWidth, uint32_t _uHeight, uint32_t _uBitDepth, TextureType _eTextureType, uint32_t _uColor = 0, bool _bCreateMipmaps = 0) = 0;
			virtual void LoadTexture1DFromMemory(const void* _pData, size_t _uLength, uint32_t _uBitDepth) = 0;
			virtual void LoadTexture2DFromMemory(const void* _pData, uint32_t _uWidth, uint32_t _uHeight, uint32_t _uBitDepth, bool _bCreateMipmaps = false) = 0;
			virtual void LoadTexture3DFromMemory(
				uint32_t _uWidth, uint32_t _uHeight, uint32_t _uBitDepth,
				const void* _pPosXData,
				const void* _pNegXData,
				const void* _pPosYData,
				const void* _pNegYData,
				const void* _pPosZData,
				const void* _pNegData,
				bool _bCreateMipMaps = false
			) = 0;
			virtual void CopyFrom(IGPUManagedBuffer* _pBuffer, size_t _uOffset) = 0;

			void LoadTexture1DFromFile(const char* _szFileName);
			void LoadTexture2DFromFile(const char* _szFileName, bool _bCreateMipmaps = false);
			void LoadTexture3DFromFiles(
				const char* _szPosXFileName,
				const char* _szNegXFileName,
				const char* _szPosYFileName,
				const char* _szNegYFileName,
				const char* _szPosZFileName,
				const char* _szNegZFileName,
				bool _bCreateMipMaps);

			inline uint32_t GetWidth() { return m_uWidth; }
			inline uint32_t GetHeight() { return m_uHeight; }
			inline TextureType GetTextureType() { return m_textureType; }
	};
}