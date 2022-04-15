// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: MeshLoader.cpp - DAS data MeshLoader class implementation
// author: Karl-Mihkel Ott

#define MESH_LOADER_CPP
#include <MeshLoader.h>

namespace DENG {

    MeshLoader::MeshLoader(const Libdas::DasMesh &_mesh, Libdas::DasParser &_parser, Renderer &_renderer, uint32_t _shader_id, uint32_t _base_ubo_offset) : 
        m_parser(_parser), 
        m_mesh(_mesh),
        m_renderer(_renderer), 
        m_shader_id(_shader_id),
        m_ubo_offset(_base_ubo_offset) 
    {
        if(m_mesh.name != "") {
            m_name = m_mesh.name;
        }
    }


    uint32_t MeshLoader::CalculateAbsoluteOffset(const Libdas::DasParser &_parser, uint32_t _buffer_id, uint32_t _buffer_offset) {
        uint32_t abs = 0;
        for(uint32_t i = 0; i < _buffer_id; i++) {
            auto &buffer = _parser.AccessBuffer(i);

            for(auto it = buffer.data_ptrs.begin(); it != buffer.data_ptrs.end(); it++)
                abs += static_cast<uint32_t>(it->second);
        }

        abs += _buffer_offset;
        return abs;
    }


    void MeshLoader::Attach() {
        m_mesh_ref_id = m_renderer.NewMeshReference();
        MeshReference &mesh = m_renderer.GetMeshes()[m_mesh_ref_id];
        mesh.name = m_mesh.name;
        mesh.shader_module_id = m_shader_id;

        // ModelAnimationUbo
        mesh.ubo_blocks.reserve(2);
        mesh.ubo_blocks.emplace_back();
        mesh.ubo_blocks.back().binding = 1;
        mesh.ubo_blocks.back().size = static_cast<uint32_t>(sizeof(ModelAnimationUbo));
        mesh.ubo_blocks.back().offset = m_ubo_offset;
        
        // ModelUbo
        mesh.ubo_blocks.emplace_back();
        mesh.ubo_blocks.back().binding = 2;
        mesh.ubo_blocks.back().size = static_cast<uint32_t>(sizeof(ModelUbo));
        mesh.ubo_blocks.back().offset = m_ubo_offset + m_renderer.AlignUniformBufferOffset(static_cast<uint32_t>(sizeof(ModelAnimationUbo)));

        // create mesh draw commands
        mesh.commands.reserve(m_mesh.primitive_count);
        for(uint32_t i = 0; i < m_mesh.primitive_count; i++) {
            mesh.commands.emplace_back();
            const Libdas::DasMeshPrimitive &prim = m_parser.AccessMeshPrimitive(m_mesh.primitives[i]);
            const uint32_t index_offset = CalculateAbsoluteOffset(m_parser, prim.index_buffer_id, prim.index_buffer_offset);
            const uint32_t vertices_offset = CalculateAbsoluteOffset(m_parser, prim.vertex_buffer_id, prim.vertex_buffer_offset);

            mesh.commands.back().indices_offset = index_offset;
            mesh.commands.back().indices_count = prim.indices_count;
            mesh.commands.back().vertices_offset = vertices_offset;
        }
    }


    void MeshLoader::UseTexture(const std::string &_name) {
        MeshReference &mesh = m_renderer.GetMeshes()[m_mesh_ref_id];

        for(auto cmd = mesh.commands.begin(); cmd != mesh.commands.end(); cmd++)
            cmd->texture_name = _name;
    }
}
