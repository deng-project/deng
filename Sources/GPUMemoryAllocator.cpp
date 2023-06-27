// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GPUMemoryAllocator.cpp - GPU memory allocator class implementation
// author: Karl-Mihkel Ott

#define GPU_MEMORY_ALLOCATOR_CPP
#include "deng/GPUMemoryAllocator.h"

using namespace std;

namespace DENG {

#ifdef _DEBUG
	void GPUMemoryAllocator::DbPrintMemoryRegions() {
		for (auto it = m_memoryRegions.begin(); it != m_memoryRegions.end(); it++) {
			if (it != m_memoryRegions.begin()) {
				auto itPrevious = it;
				itPrevious--;
				size_t uFragmentSize = it->uOffset - itPrevious->uOffset - itPrevious->uSize;

				if (uFragmentSize) {
					cout << '[';
					for (uint32_t i = 0; i < uFragmentSize; i++)
						cout << '-';
					cout << "] ";
				}
			}

			cout << '[';
			for (uint32_t i = 0; i < it->uSize; i++) {
				cout << '*';
			}
			cout << "] ";
		}

		cout << '\n';
	}
#endif
	
	MemoryRegion GPUMemoryAllocator::RequestMemory(size_t _uSize, size_t _uMinimalAlignmentOffset) {
		if (_uMinimalAlignmentOffset) {
			_uSize = (_uSize + _uMinimalAlignmentOffset - 1) & ~(_uMinimalAlignmentOffset - 1);
		}

		// check if there are any fragmented regions available
		size_t uOffset = 0;
		if (m_storageFragments.size()) {
			pop_heap(m_storageFragments.begin(), m_storageFragments.end(), UnusedMemoryFragment::Compare());
			
			// assuming that the heap has already been constructed
			uOffset = m_storageFragments.back().uOffset;
			size_t uAvailableSize = m_storageFragments.back().uSize;

			if (_uMinimalAlignmentOffset > 0) {
				uOffset = (uOffset + _uMinimalAlignmentOffset - 1) & ~(_uMinimalAlignmentOffset - 1);
				uAvailableSize -= uOffset - m_storageFragments.back().uOffset;
			}

			if (uAvailableSize >= _uSize) {
				auto itRegion1 = m_storageFragments.back().region1;
				auto itRegion2 = m_storageFragments.back().region2;
				std::list<MemoryRegion>::iterator itEmplacedArea;

				if (itRegion1 != itRegion2) {
					itEmplacedArea = m_memoryRegions.emplace(itRegion2, uOffset, _uSize);
				}
				else {
					itEmplacedArea = m_memoryRegions.emplace(m_memoryRegions.begin(), uOffset, _uSize);
				}

				// check if there is a fragment before emplaced region
				if (uOffset > m_storageFragments.back().uOffset) {
					m_storageFragments.back().region1 = itRegion1;
					m_storageFragments.back().region2 = itEmplacedArea;
					m_storageFragments.back().uSize =
						itEmplacedArea->uOffset - itRegion1->uOffset - itRegion1->uSize;

					// and after
					if (uOffset + _uSize < itRegion2->uOffset) {
						m_storageFragments.push_back({
							itEmplacedArea,
							itRegion2,
							itEmplacedArea->uOffset + itEmplacedArea->uSize,
							itRegion2->uOffset - itEmplacedArea->uOffset - itEmplacedArea->uSize });
					}
				}
				// after
				else if (uOffset + _uSize < itRegion2->uOffset) {
					m_storageFragments.back().region1 = itEmplacedArea;
					m_storageFragments.back().region2 = itRegion2;
					m_storageFragments.back().uSize =
						itRegion2->uOffset - itEmplacedArea->uOffset - itEmplacedArea->uSize;
					m_storageFragments.back().uOffset = itEmplacedArea->uOffset + itEmplacedArea->uSize;
				}
				// continuous
				else {
					m_storageFragments.pop_back();
				}

				make_heap(m_storageFragments.begin(), m_storageFragments.end(), UnusedMemoryFragment::Compare());
				return *itEmplacedArea;
			}
		}

		// memory region exists before
		if (m_memoryRegions.size()) {
			uOffset = m_memoryRegions.back().uOffset + m_memoryRegions.back().uSize;

			if (uOffset > 0)
				uOffset = (uOffset + _uMinimalAlignmentOffset - 1) & ~(_uMinimalAlignmentOffset - 1);
			m_memoryRegions.push_back({ uOffset, _uSize });
			auto itPreviousRegion = m_memoryRegions.end();
			--itPreviousRegion;
			--itPreviousRegion;
			auto itPushedRegion = m_memoryRegions.end();
			--itPushedRegion;

			if (uOffset > itPreviousRegion->uOffset + itPreviousRegion->uSize) {
				m_storageFragments.push_back({
					itPreviousRegion,
					itPushedRegion,
					itPreviousRegion->uOffset + itPreviousRegion->uSize,
					itPushedRegion->uOffset - itPreviousRegion->uOffset - itPreviousRegion->uSize });
				make_heap(m_storageFragments.begin(), m_storageFragments.end(), UnusedMemoryFragment::Compare());
			}
		}
		else {
			m_memoryRegions.push_back({ uOffset, _uSize });
		}

		return m_memoryRegions.back();
	}


	// O(n)
	bool GPUMemoryAllocator::FreeMemory(size_t _uOffset) {
		bool bIsRemoved = false;
		std::list<MemoryRegion>::iterator it = m_memoryRegions.begin();
		for (; it != m_memoryRegions.end(); it++) {
			if (it->uOffset == _uOffset) {
				bIsRemoved = true;
				break;
			}
		}

		if (bIsRemoved) {
			for (size_t i = 0; i < m_storageFragments.size(); i++) {
				if (m_storageFragments[i].region1 == it) {
					if (it == m_memoryRegions.begin()) {
						m_storageFragments[i].region1 = m_storageFragments[i].region2;
						m_storageFragments[i].uSize = m_storageFragments[i].region2->uOffset;
						m_storageFragments[i].uOffset = 0;
					}
					else {
						m_storageFragments[i].region1 = it;
						m_storageFragments[i].region1--;
						m_storageFragments[i].uSize = m_storageFragments[i].region2->uOffset -
							m_storageFragments[i].region1->uOffset - m_storageFragments[i].region1->uSize;
						m_storageFragments[i].uOffset = m_storageFragments[i].region1->uSize + m_storageFragments[i].region1->uOffset;
					}
				}
				else if (m_storageFragments[i].region2 == it) {
					if (it == --m_memoryRegions.end()) {
						m_storageFragments.erase(m_storageFragments.begin() + i);
						i--;
					}
					else {
						m_storageFragments[i].region2 = it;
						m_storageFragments[i].region2++;
						m_storageFragments[i].uSize = m_storageFragments[i].region2->uOffset -
							m_storageFragments[i].region1->uOffset - m_storageFragments[i].region1->uSize;
						m_storageFragments[i].uOffset = m_storageFragments[i].region1->uSize + m_storageFragments[i].region1->uOffset;
					}
				}
			}

			auto itNext = it;
			itNext++;
			auto itPrevious = it;

			if (itNext != m_memoryRegions.end() && 
				it != m_memoryRegions.begin() &&
				it->uOffset + it->uSize == itNext->uOffset &&
				(--itPrevious)->uOffset + itPrevious->uSize == it->uOffset)
			{
				m_storageFragments.push_back({
					itPrevious,
					itNext,
					it->uOffset,
					it->uSize });
			}
			else if (itNext != m_memoryRegions.end() &&
				it == m_memoryRegions.begin() &&
				it->uOffset + it->uSize == it->uOffset)
			{
				m_storageFragments.push_back({
					itNext,
					itNext,
					0, 
					it->uSize + it->uOffset });
			}
		}

		m_memoryRegions.erase(it);
		make_heap(m_storageFragments.begin(), m_storageFragments.end(), UnusedMemoryFragment::Compare());

		return bIsRemoved;
	}
}