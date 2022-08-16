// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLBufferLoader.cpp - OpenGL buffer loading helper class implementation
// author: Karl-Mihkel Ott

#define OPENGL_BUFFER_LOADER_CPP
#include "deng/OpenGLBufferLoader.h"

namespace DENG {

    namespace OpenGL {

        BufferLoader::BufferLoader() {
            // Generate and bind new vertex array handles
            glGenVertexArrays(1, &m_buffer_data.vert_array);
            glErrorCheck("glGenVertexArrays");
            glBindVertexArray(m_buffer_data.vert_array);
            glErrorCheck("glBindVertexArray");

            // Generate new buffers
            GLuint buffers[2];
            glGenBuffers(2, buffers);
            glErrorCheck("glGenBuffers");
            m_buffer_data.vert_buffer = buffers[0];
            m_buffer_data.ubo_buffer = buffers[1];

            // Bind all buffers handles with their corresponding buffer types
            glBindBuffer(GL_ARRAY_BUFFER, m_buffer_data.vert_buffer);
            glErrorCheck("glBindBuffer");

            glBindBuffer(GL_UNIFORM_BUFFER, m_buffer_data.ubo_buffer);
            glErrorCheck("glBindBuffer");

            // Allocate the initial amount of memory for buffers
            _FindLimits();
            _AllocateBufferMemory();
        }


        void BufferLoader::_AllocateBufferMemory() {
            // Allocate memory for vertex buffer
            glBufferData(GL_ARRAY_BUFFER, m_buffer_cap, NULL, GL_STATIC_DRAW);
            glErrorCheck("glBufferData");

            // Allocate memory for uniforms
            glBufferData(GL_UNIFORM_BUFFER, m_ubo_cap, NULL, GL_DYNAMIC_DRAW);
            glErrorCheck("glBufferData");
        }


        uint32_t BufferLoader::_CalculateNewCapacity(uint32_t _req_size) {
            uint32_t msb = 0, mask = 1u << 31u;
            for(int i = 0; i < 32; i++) {
                if((_req_size << i & mask) == mask) {
                    msb = i;
                    break;
                }
            }

            return mask >> (msb - 1);
        }


        void BufferLoader::_FindLimits() {
            glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &m_max_vertex_uniform_blocks);
            glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &m_max_fragment_uniform_blocks);
            glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &m_max_geometry_uniform_blocks);
            glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &m_uniform_buffer_offset_alignment);
        }


        void BufferLoader::_ReallocateBuffer(GLenum _buffer, uint32_t _required_size) {
            uint32_t cpy_len = 0;
            uint32_t new_size = 0;

            if(_buffer == GL_ARRAY_BUFFER) {
                DENG_ASSERT(_required_size > m_buffer_cap);
                glBindBuffer(GL_ARRAY_BUFFER, m_buffer_data.vert_buffer);
                glErrorCheck("glBindBuffer");

                cpy_len = m_buffer_cap;
                m_buffer_cap = _CalculateNewCapacity(_required_size);
                new_size = m_buffer_cap;
            } else {
                DENG_ASSERT(_required_size > m_ubo_cap);
                glBindBuffer(GL_UNIFORM_BUFFER, m_buffer_data.ubo_buffer);
                glErrorCheck("glBindBuffer");

                cpy_len = m_ubo_cap;
                m_ubo_cap = _CalculateNewCapacity(_required_size);
                new_size = m_ubo_cap;
            }

            char *buffer = reinterpret_cast<char*>(malloc(cpy_len));

            void *cp_data = glMapBufferRange(_buffer, (GLintptr) 0, (GLsizeiptr) cpy_len, GL_MAP_READ_BIT);
            glErrorCheck("glMapBufferRange");
                std::memcpy(buffer, cp_data, cpy_len);
            glUnmapBuffer(_buffer);
            glErrorCheck("glUnmapBuffer");

            glBufferData(_buffer, new_size, NULL, GL_DYNAMIC_DRAW);
            glErrorCheck("glBufferData");

            cp_data = glMapBufferRange(_buffer, (GLintptr)0, (GLsizeiptr)cpy_len, GL_MAP_WRITE_BIT);
            glErrorCheck("glMapBufferRange");
                std::memcpy(cp_data, buffer, cpy_len);
            glUnmapBuffer(_buffer);

            std::free(buffer);
        }

        
        void BufferLoader::RequestMemory(uint32_t _request, GLenum _buffer_type) {
            
            // check the buffer type and reallocate buffer if needed
            switch (_buffer_type) {
                case GL_ARRAY_BUFFER:
                    if (_request >= m_buffer_cap)
                        _ReallocateBuffer(GL_ARRAY_BUFFER, _request);
                    break;

                case GL_UNIFORM_BUFFER:
                    if (_request >= m_ubo_cap)
                        _ReallocateBuffer(GL_UNIFORM_BUFFER, _request);
                    break;

                default:
                    break;
            }
        }
    }
}
