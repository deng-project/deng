// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: MeshLoader.cpp - DAS data MeshLoader class implementation
// author: Karl-Mihkel Ott

#define MESH_LOADER_CPP
#include <MeshLoader.h>

namespace DENG {

    MeshLoader::MeshLoader(Libdas::DasMesh &_mesh, Libdas::DasParser &_parser, Renderer &_renderer, uint32_t _shader_id) : m_parser(_parser), m_renderer(_renderer), m_shader_id(_shader_id) {
        m_mesh_ref.name = _mesh.name;
        m_mesh_ref.shader_module_id = m_shader_id;

        m_mesh_ref.commands.reserve(_mesh.primitive_count);

        for(uint32_t i = 0; i < _mesh.primitive_count; i++) {
            m_mesh_ref.commands.emplace_back();
            Libdas::DasMeshPrimitive &prim = m_parser.AccessMeshPrimitive(_mesh.primitives[i]);
            const uint32_t index_offset = _CalculateAbsoluteOffset(prim.index_buffer_id, prim.index_buffer_offset);
            const uint32_t vertices_offset = _CalculateAbsoluteOffset(prim.vertex_buffer_id, prim.vertex_buffer_offset);

            m_mesh_ref.commands.back().indices_offset = index_offset;
            m_mesh_ref.commands.back().indices_count = prim.indices_count;
            m_mesh_ref.commands.back().vertices_offset = vertices_offset;
        }
    }


    uint32_t MeshLoader::_CalculateAbsoluteOffset(uint32_t _buffer_id, uint32_t _buffer_offset) {
        uint32_t abs = 0;
        for(uint32_t i = 0; i < _buffer_id; i++) {
            Libdas::DasBuffer &buffer = m_parser.AccessBuffer(i);

            for(auto it = buffer.data_ptrs.begin(); it != buffer.data_ptrs.end(); it++)
                abs += static_cast<uint32_t>(it->second);
        }

        abs += _buffer_offset;
        return abs;
    }
}
