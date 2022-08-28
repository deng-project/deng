// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLBufferLoader.h - OpenGL buffer loading helper class header
// author: Karl-Mihkel Ott

#ifndef OPENGL_BUFFER_LOADER_H
#define OPENGL_BUFFER_LOADER_H

#ifdef OPENGL_BUFFER_LOADER_CPP
    #include <cstdint>
    #include <vector>
    #include <cstring>

    #include "nwin/glad/glad.h"

    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/BufferAlignment.h"
#define DEBUG_ONLY
    #include "deng/OpenGLRenderer.h"
#undef DEBUG_ONLY
#endif


namespace DENG {

    namespace OpenGL {

        struct BufferData {
            GLuint vert_array = 0;
            GLuint vert_buffer = 0;
            GLuint ubo_buffer = 0;
        };

        class BufferLoader {
            private:
                BufferData m_buffer_data;
                uint32_t m_buffer_cap = DEFAULT_BUFFER_SIZE;
                uint32_t m_ubo_cap = DEFAULT_UNIFORM_SIZE;

                // device limitations
                GLint m_max_vertex_uniform_blocks = 0;
                GLint m_max_geometry_uniform_blocks = 0;
                GLint m_max_fragment_uniform_blocks = 0;
                GLint m_uniform_buffer_offset_alignment = 0;

            private:
                void _AllocateBufferMemory();
                uint32_t _CalculateNewCapacity(uint32_t);
                void _FindLimits();

                void _ReallocateBuffer(GLenum _buffer, uint32_t _required_size);
            public:
                BufferLoader();
                void RequestMemory(uint32_t _request, GLenum _buffer_type);

                inline BufferData &GetBufferData() { return m_buffer_data; }
                inline GLint GetMaxVertexUniformBlocks() { return m_max_vertex_uniform_blocks; }
                inline GLint GetMaxFragmentUniformBlocks() { return m_max_fragment_uniform_blocks; }
                inline GLint GetMaxGeometryUniformBlocks() { return m_max_geometry_uniform_blocks; }
                inline GLint GetUniformBufferOffsetAlignment() { return m_uniform_buffer_offset_alignment; }

        };
    }
}


#endif
