// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLBufferLoader.h - OpenGL buffer loading helper class header
// author: Karl-Mihkel Ott

#ifndef OPENGL_BUFFER_LOADER_H
#define OPENGL_BUFFER_LOADER_H

#ifdef OPENGL_BUFFER_LOADER_CPP
    #include <cstdint>

    #include <glad/glad.h>
    #include <ErrorDefinitions.h>
#define DEBUG_ONLY
    #include <OpenGLRenderer.h>
#undef DEBUG_ONLY
#endif

#define DENG_DEFAULT_MESH_CAPACITY      65536
#define DENG_DEFAULT_UBO_CAPACITY       


namespace DENG {

    namespace OpenGL {

        struct BufferData {
            GLuint vert_array = 0;
            GLuint vert_buffer = 0;
            GLuint idx_buffer = 0;
            GLuint ubo_buffer = 0;
        };

        class BufferLoader {
            private:
                BufferData m_buffer_data;
                uint32_t m_mesh_cap = DENG_DEFAULT_MESH_CAPACITY;
                uint32_t m_index_cap = DENG_DEFAULT_MESH_CAPACITY;
                //uint32_t m_ubo_cap = DE

            private:
                void _AllocateBufferMemory();
                uint32_t _CalculateNewCapacity(uint32_t);

            public:
                BufferLoader();
                void RequestMemory(uint32_t _vertex_request, uint32_t _element_request);

                inline BufferData &GetBufferData() { return m_buffer_data; }
        };
    }
}


#endif
