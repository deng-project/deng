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
            GLuint buffers[2];
            glGenBuffers(2, buffers);
            glErrorCheck("glGenBuffers");
            m_buffer_data.vert_buffer = buffers[0];
            m_buffer_data.idx_buffer = buffers[1];

            // Bind all buffers handles with their corresponding buffer types
            glBindBuffer(GL_ARRAY_BUFFER, m_buffer_data.vert_buffer);
            glErrorCheck("glBindBuffer");

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer_data.idx_buffer);
            glErrorCheck("glBindBuffer");

            // Allocate the initial amount of memory for buffers
            _AllocateBufferMemory();
        }


        void BufferLoader::_AllocateBufferMemory() {
            // Allocate memory for vertex buffer
            glBufferData(GL_ARRAY_BUFFER, m_mesh_cap, NULL, GL_STATIC_DRAW);
            glErrorCheck("glBufferData");

            // Allocate memory for indices buffer
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index_cap, NULL, GL_STATIC_DRAW);
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


        void BufferLoader::RequestMemory(uint32_t _vertex_request, uint32_t _element_request) {
            if(_vertex_request >= m_mesh_cap) {
                m_mesh_cap = _CalculateNewCapacity(_vertex_request);
                glBufferData(GL_ARRAY_BUFFER, m_mesh_cap, NULL, GL_STATIC_DRAW);
                glErrorCheck("glBufferData");
            }

            if(_element_request >= m_index_cap) {
                m_index_cap = _CalculateNewCapacity(_vertex_request);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index_cap, NULL, GL_STATIC_DRAW);
                glErrorCheck("glBufferData");
            }
        }

    }
}
