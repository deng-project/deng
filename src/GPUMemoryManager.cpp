// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GPUMemoryManager.cpp - Universal graphics memory manager implementation for DENG components
// author: Karl-Mihkel Ott

#define GPU_MEMORY_MANAGER_CPP
#include "deng/GPUMemoryManager.h"

namespace DENG {

    GPUMemoryManager *GPUMemoryManager::m_instance = nullptr;
    
    uint32_t GPUMemoryManager::RequestMainMemoryLocationF(uint32_t _component_stride, uint32_t _size) {
        if(m_main_buffer_blocks.size()) {
            const uint32_t offset = m_main_buffer_blocks.back().first + m_main_buffer_blocks.back().second;
            const uint32_t component_offset = offset % _component_stride ? (_component_stride - (offset % _component_stride)) : 0;
            m_main_buffer_blocks.push_back(std::make_pair(offset + component_offset, _size));
            return m_main_buffer_blocks.back().first;
        }

        m_main_buffer_blocks.push_back(std::make_pair(0, _size));
        return 0;
    }


    uint32_t GPUMemoryManager::RequestUniformMemoryLocationF(Renderer &_rend, uint32_t _size) {
        if(m_ubo_buffer_blocks.size()) {
            const uint32_t offset = _rend.AlignUniformBufferOffset(m_ubo_buffer_blocks.back().first + m_ubo_buffer_blocks.back().second);
            m_ubo_buffer_blocks.push_back(std::make_pair(offset, _size));
            return offset;
        }

        m_ubo_buffer_blocks.push_back(std::make_pair(0, _size));
        return 0;
    }


    uint32_t GPUMemoryManager::RequestMainMemoryLocationP(uint32_t _component_stride, uint32_t _size) {
        if(m_main_buffer_blocks.size()) {
            uint32_t offset = UINT32_MAX;
            uint32_t block_index = 0;

            // check if there is any space between regions that can be used for memory allocation
            for(uint32_t i = 0; i < m_main_buffer_blocks.size() - 1; i++) {
                const uint32_t first_block_offset = (m_main_buffer_blocks[i].first + m_main_buffer_blocks[i].second);
                const uint32_t second_block_offset = m_main_buffer_blocks[i + 1].first;
                const uint32_t component_offset = first_block_offset % _component_stride ? (_component_stride - (first_block_offset % _component_stride)) : 0;
                if(second_block_offset - (first_block_offset + component_offset) >= _size) {
                    offset = first_block_offset + (_component_stride - (first_block_offset % _component_stride));
                    block_index = i;
                    break;
                }
            }

            // found a region in-between
            if(offset != UINT32_MAX) {
                std::vector<std::pair<uint32_t, uint32_t>> new_blocks;
                new_blocks.reserve(m_main_buffer_blocks.size() + 1);
                for(uint32_t i = 0; i < m_main_buffer_blocks.size(); i++) {
                    new_blocks.push_back(m_main_buffer_blocks[i]);
                    if(i == block_index)
                        new_blocks.push_back(std::make_pair(offset, _size));
                }

                m_main_buffer_blocks = new_blocks;
                return offset;
            }
        }

        return RequestMainMemoryLocationF(_component_stride, _size);
    }


    uint32_t GPUMemoryManager::RequestUniformMemoryLocationP(Renderer &_rend, uint32_t _size) {
        if(m_ubo_buffer_blocks.size()) {
            uint32_t offset = UINT32_MAX;
            uint32_t block_index = 0;

            // check if there is any space between regions that can be used for memory allocation
            for(uint32_t i = 0; i < m_ubo_buffer_blocks.size() - 1; i++) {
                const uint32_t first_block_offset = _rend.AlignUniformBufferOffset(m_ubo_buffer_blocks[i].first + m_ubo_buffer_blocks[i].second);
                const uint32_t second_block_offset = m_ubo_buffer_blocks[i + 1].first;

                if(_rend.AlignUniformBufferOffset(first_block_offset + _size) <= second_block_offset) {
                    offset = first_block_offset;
                    block_index = i;
                    break;
                }
            }

            // found a suitable region in-between
            if(offset != UINT32_MAX) {
                std::vector<std::pair<uint32_t, uint32_t>> new_regions;
                new_regions.reserve(m_ubo_buffer_blocks.size() + 1);
                for(uint32_t i = 0; i < m_ubo_buffer_blocks.size(); i++) {
                    new_regions.push_back(m_ubo_buffer_blocks[i]);
                    if(i == block_index)
                        new_regions.push_back(std::make_pair(offset, _size));
                }

                m_ubo_buffer_blocks = new_regions;
                return offset;
            }
        }

        return RequestUniformMemoryLocationF(_rend, _size);
    }


    void GPUMemoryManager::DeleteUniformMemoryLocation(uint32_t _offset) {
        size_t index = m_ubo_buffer_blocks.size() / 2;
        size_t l = 0;
        size_t r = m_ubo_buffer_blocks.size() - 1;

        while(index < m_ubo_buffer_blocks.size()) {
            if(m_ubo_buffer_blocks[index].first == _offset) { // found
                m_ubo_buffer_blocks.erase(m_ubo_buffer_blocks.begin() + index);
                return;
            } else if(m_ubo_buffer_blocks[index].first > _offset) { // look left
                DENG_ASSERT(r != 0);
                r = index;
                uint32_t sub = (r - l) >> 1;
                index -= sub ? sub : 1;
            } else if(m_ubo_buffer_blocks[index].first < _offset) { // look right
                DENG_ASSERT(l != m_ubo_buffer_blocks.size() - 1);
                l = index;
                uint32_t add = (r - l) >> 1;
                index += add ? add : 1;
            }
        }
    }


    void GPUMemoryManager::DeleteMainMemoryLocation(uint32_t _offset) {
        size_t index = m_main_buffer_blocks.size() / 2;
        size_t l = 0;
        size_t r = m_main_buffer_blocks.size() - 1;

        while(index < m_main_buffer_blocks.size()) {
            if(m_main_buffer_blocks[index].first == _offset) { // found
                m_main_buffer_blocks.erase(m_main_buffer_blocks.begin() + index);
                return;
            } else if(m_main_buffer_blocks[index].first > _offset) { // look left
                DENG_ASSERT(r != 0);
                r = index;
                const uint32_t sub = (r - l) >> 1;
                index -= sub ? sub : 1;
            } else if(m_main_buffer_blocks[index].first < _offset) { // look right
                DENG_ASSERT(l != m_main_buffer_blocks.size() - 1);
                l = index;
                const uint32_t add = (r - l) >> 1;
                index += add ? add : 1;
            }
        }
    }
}
