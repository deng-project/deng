// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GPUMemoryAllocator.h - GPU memory allocator class header
// author: Karl-Mihkel Ott

#ifndef GPU_MEMORY_ALLOCATOR_H
#define GPU_MEMORY_ALLOCATOR_H

#include <cstdint>
#include <list>
#include <vector>

#ifdef GPU_MEMORY_ALLOCATOR_CPP
	#include <queue>
	#include <algorithm>
	#include <iostream>
	
	//#include "deng/Api.h"
	#define DENG_API
#endif

namespace DENG {
	struct MemoryRegion {
		MemoryRegion() = default;
		MemoryRegion(const MemoryRegion&) = default;

		MemoryRegion(size_t _uOffset, size_t _uSize) :
			uOffset(_uOffset),
			uSize(_uSize) {}

		size_t uOffset;
		size_t uSize;
	};

	class DENG_API GPUMemoryAllocator {
		private:
			struct UnusedMemoryFragment {
				std::list<MemoryRegion>::iterator region1;
				std::list<MemoryRegion>::iterator region2;

				size_t uOffset = 0;
				size_t uSize = 0;

				struct Compare {
					bool operator()(const UnusedMemoryFragment& _frag1, const UnusedMemoryFragment& _frag2) const {
						return _frag1.uSize > _frag2.uSize;
					}
				};
			};

			std::vector<UnusedMemoryFragment> m_storageFragments;
			std::list<MemoryRegion> m_memoryRegions;

		public:
			GPUMemoryAllocator() = default;
			
#ifdef _DEBUG
			void DbPrintMemoryRegions();
#endif
			MemoryRegion RequestMemory(size_t _uSize, size_t _uMinimalAlignmentOffset);
			bool FreeMemory(size_t _uOffset);
	};
}

#endif
