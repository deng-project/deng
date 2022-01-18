// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLBufferLoader.cpp - OpenGL buffer loading helper class implementation
// author: Karl-Mihkel Ott

#define OPENGL_BUFFER_LOADER_CPP
#include <OpenGLBufferLoader.h>

namespace DENG {

    namespace OpenGL {

        BufferLoader::BufferLoader() {
            // Generate and bind new vertex array handles
            glGenVertexArrays(1, &m_buffer_data.vert_array);
            glErrorCheck("glGenVertexArrays");
            glBindVertexArray(m_buffer_data.vert_array);
            glErrorCheck("glBindVertexArray");

            // Generate new buffers
            GLuint buffers[3];
            glGenBuffers(3, buffers);
            glErrorCheck("glGenBuffers");
            m_buffer_data.vert_buffer = buffers[0];
            m_buffer_data.idx_buffer = buffers[1];
            m_buffer_data.ubo_buffer = buffers[2];

            // Bind all buffers handles with their corresponding buffer types
            glBindBuffer(GL_ARRAY_BUFFER, m_buffer_data.vert_buffer);
            glErrorCheck("glBindBuffer");

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer_data.idx_buffer);
            glErrorCheck("glBindBuffer");

            glBindBuffer(GL_UNIFORM_BUFFER, m_buffer_data.ubo_buffer);
            glErrorCheck("glBindBuffer");

            // Allocate the initial amount of memory for buffers
            _FindLimits();
            _AllocateBufferMemory();
        }



        void BufferLoader::_AllocateBufferMemory() {
            // Allocate memory for vertex buffer
            glBufferData(GL_ARRAY_BUFFER, m_mesh_cap, NULL, GL_STATIC_DRAW);
            glErrorCheck("glBufferData");

            // Allocate memory for indices buffer
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index_cap, NULL, GL_STATIC_DRAW);
            glErrorCheck("glBufferData");

            // Allocate memory for uniforms
            glBufferData(GL_UNIFORM_BUFFER, m_ubo_cap, NULL, GL_STATIC_DRAW);
            glErrorCheck("glBufferData");
        }


        uint32_t BufferLoader::_CalculateNewCapacity(uint32_t _req_size) {
            uint32_t msb = 0, mask = 1 << 31;
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


        void BufferLoader::_ReallocateBuffer(GLenum _buffer, uint32_t _new_size, uint32_t &_supplement) {
            size_t len = _supplement;
            char *buffer = reinterpret_cast<char*>(malloc(len));

            void *cp_data = glMapBufferRange(_buffer, (GLintptr)0, (GLsizeiptr)len, GL_MAP_READ_BIT);
            glErrorCheck("glMapBufferRange");
            std::memcpy(buffer, cp_data, len);
            glUnmapBuffer(_buffer);
            glErrorCheck("glUnmapBuffer");

            _supplement = _new_size;
            glBufferData(_buffer, m_mesh_cap, NULL, GL_STATIC_DRAW);
            glErrorCheck("glBufferData");

            cp_data = glMapBufferRange(_buffer, (GLintptr)0, (GLsizeiptr)len, GL_MAP_WRITE_BIT);
            glErrorCheck("glMapBufferRange");
            std::memcpy(cp_data, buffer, len);
            glUnmapBuffer(_buffer);
        }

        
        void BufferLoader::RequestMemory(uint32_t _request, GLenum _buffer_type) {
            
            // check the buffer type and reallocate buffer if needed
            switch (_buffer_type) {
                case GL_ARRAY_BUFFER:
                    if (_request >= m_mesh_cap)
                        _ReallocateBuffer(GL_ARRAY_BUFFER, _CalculateNewCapacity(_request), m_mesh_cap);
                    break;

                case GL_ELEMENT_ARRAY_BUFFER:
                    if (_request >= m_index_cap)
                        _ReallocateBuffer(GL_ELEMENT_ARRAY_BUFFER, _CalculateNewCapacity(_request), m_index_cap);
                    break;

                case GL_UNIFORM_BUFFER:
                    if (_request >= m_ubo_cap)
                        _ReallocateBuffer(GL_UNIFORM_BUFFER, _CalculateNewCapacity(_request), m_ubo_cap);
                    break;

                default:
                    break;
            }
        }
    }
}
