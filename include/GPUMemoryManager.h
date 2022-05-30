// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: GPUMemoryManager.h - Universal graphics memory manager header for DENG components
// author: Karl-Mihkel Ott

#ifndef GPU_MEMORY_MANAGER_H
#define GPU_MEMORY_MANAGER_H

#ifdef GPU_MEMORY_MANAGER_CPP
    #include <cstdint>
    #include <cstring>
    #include <vector>
    #include <string>
#ifdef _DEBUG
    #include <iostream>
#endif

    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    
    #include <Api.h>
    #include <ShaderDefinitions.h>
    #include <ErrorDefinitions.h>
    #include <Window.h>
    #include <Renderer.h>
#endif

namespace DENG {

    // singleton class
    class DENG_API GPUMemoryManager {
        private:
            static GPUMemoryManager *m_instance;
            // first - offset in bytes
            // second - used size in bytes
            std::vector<std::pair<uint32_t, uint32_t>> m_ubo_buffer_blocks;
            std::vector<std::pair<uint32_t, uint32_t>> m_main_buffer_blocks;

        private:
            GPUMemoryManager() = default;
            
        public:
            static inline GPUMemoryManager *GetInstance() {
                if(!m_instance)
                    m_instance = new GPUMemoryManager();
                return m_instance;
            }

            static inline void DeleteInstance() {
                delete m_instance;
            }

            // request methods
            uint32_t RequestMainMemoryLocationF(uint32_t _component_stride, uint32_t _size);
            uint32_t RequestUniformMemoryLocationF(Renderer &_rend, uint32_t _size);
            uint32_t RequestMainMemoryLocationP(uint32_t _component_stride, uint32_t _size);
            uint32_t RequestUniformMemoryLocationP(Renderer &_rend, uint32_t _size);

            // update methods
            void UpdateMainMemoryLocation(uint32_t _offset, uint32_t _size); 
            void UpdateUniformMemoryLocation(uint32_t _offset, uint32_t _size);

            inline uint32_t GetMaxMainBufferOffset() {
                return m_main_buffer_blocks.back().first + m_main_buffer_blocks.back().second;
            }

            inline uint32_t GetMaxUniformOffset() {
                return m_ubo_buffer_blocks.back().first + m_ubo_buffer_blocks.back().second;
            }

            // checking methods
            inline bool IsLastMainMemoryLocation(uint32_t _offset) {
                return m_main_buffer_blocks.back().first == _offset;
            }

            inline bool IsLastUniformMemoryLocation(uint32_t _offset) {
                return m_ubo_buffer_blocks.back().first == _offset;
            }

            // deletion methods
            void DeleteUniformMemoryLocation(uint32_t _offset);
            void DeleteMainMemoryLocation(uint32_t _offset);
    };
}

#endif
