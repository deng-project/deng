// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: MeshLoader.cpp - DAS data MeshLoader class implementation
// author: Karl-Mihkel Ott

#define MESH_LOADER_CPP
#include <MeshLoader.h>

namespace DENG {

    uint32_t MeshLoader::m_mesh_index = 0;
    uint32_t MeshLoader::m_ubo_offset = 0;
    uint32_t MeshLoader::m_main_buffer_offset = 0;

    MeshLoader::MeshLoader(const Libdas::DasMesh &_mesh, Libdas::DasParser &_parser, Renderer &_renderer, uint32_t _camera_offset) : 
        m_parser(_parser), 
        m_mesh(_mesh),
        m_renderer(_renderer)
    {
        m_mesh_ubo_offset = m_ubo_offset;
        if(m_mesh.name != "") {
            m_name = m_mesh.name;
        } else {
            m_name += std::to_string(m_mesh_index++);
        }

        // request shader id to use for current mesh
        _CheckMeshPrimitives();
        const Libdas::DasMeshPrimitive &prim = m_parser.AccessMeshPrimitive(m_mesh.primitives[0]);
        m_shader_id = ModelShaderManager::RequestShaderModule(m_renderer, prim, m_mesh_ubo_offset, _camera_offset);
    }


    void MeshLoader::_CheckMeshPrimitives() {
        // if mesh is valid then each mesh primitive must contain same vertex attributes
        DENG_ASSERT(m_mesh.primitive_count);
        MeshPrimitiveAttributeDescriptor attr_desc;
        for(uint32_t i = 0; i < m_mesh.primitive_count; i++) {
            const Libdas::DasMeshPrimitive &prim = m_parser.AccessMeshPrimitive(m_mesh.primitives[i]);
            if(!i) {
                attr_desc.normal = (prim.vertex_normal_buffer_id != UINT32_MAX);
                attr_desc.tangent = (prim.vertex_tangent_buffer_id != UINT32_MAX);
                attr_desc.texture_count = prim.texture_count;
                attr_desc.color_mul_count = prim.color_mul_count;
                attr_desc.joint_set_count = prim.joint_set_count;
            } else {
                MeshPrimitiveAttributeDescriptor cattr_desc;
                cattr_desc.normal = (prim.vertex_normal_buffer_id != UINT32_MAX);
                cattr_desc.tangent = (prim.vertex_tangent_buffer_id != UINT32_MAX);
                cattr_desc.texture_count = prim.texture_count;
                cattr_desc.color_mul_count = prim.color_mul_count;
                cattr_desc.joint_set_count = prim.joint_set_count;

                // check if created attribute descriptor matches the original descriptor
                if(cattr_desc != attr_desc) {
                    WARNME("Mesh '" << m_mesh.name << "' contains primitives with different attributes");
                    std::exit(LIBDAS_ERROR_INVALID_FILE);
                }
            }
        }
    }


    uint32_t MeshLoader::CalculateAbsoluteOffset(const Libdas::DasParser &_parser, uint32_t _buffer_id, uint32_t _buffer_offset) {
        uint32_t abs = m_main_buffer_offset;
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

        // ModelUbo
        uint32_t binding_id = 1;
        mesh.ubo_blocks.reserve(3);
        mesh.ubo_blocks.emplace_back();
        mesh.ubo_blocks.back().binding = binding_id++;
        mesh.ubo_blocks.back().size = static_cast<uint32_t>(sizeof(ModelUbo));
        mesh.ubo_blocks.back().offset = m_ubo_offset;
        m_ubo_offset += m_renderer.AlignUniformBufferOffset(static_cast<uint32_t>(sizeof(ModelUbo)));

        // check if joint matrix ubos should be created
        if(m_parser.AccessMeshPrimitive(m_mesh.primitives[0]).joint_set_count) {
            const Libdas::DasMeshPrimitive &prim = m_parser.AccessMeshPrimitive(m_mesh.primitives[0]);
            mesh.ubo_blocks.emplace_back();
            mesh.ubo_blocks.back().binding = binding_id++;
            mesh.ubo_blocks.back().size = static_cast<uint32_t>(sizeof(Libdas::Matrix4<float>)) * 4 * prim.joint_set_count;
            mesh.ubo_blocks.back().offset = m_ubo_offset;
            m_ubo_offset += m_renderer.AlignUniformBufferOffset(static_cast<uint32_t>(sizeof(Libdas::Matrix4<float>)) * 4 * prim.joint_set_count);
        }

        // create mesh draw commands
        mesh.commands.reserve(m_mesh.primitive_count);
        for(uint32_t i = 0; i < m_mesh.primitive_count; i++) {
            mesh.commands.emplace_back();
            const Libdas::DasMeshPrimitive &prim = m_parser.AccessMeshPrimitive(m_mesh.primitives[i]);
            const uint32_t index_offset = CalculateAbsoluteOffset(m_parser, prim.index_buffer_id, prim.index_buffer_offset);

            mesh.commands.back().indices_offset = index_offset;
            mesh.commands.back().indices_count = prim.indices_count;
            mesh.commands.back().attribute_offsets.reserve(3 + prim.texture_count + prim.color_mul_count + prim.joint_set_count * 2);
            mesh.commands.back().attribute_offsets.push_back(CalculateAbsoluteOffset(m_parser, prim.vertex_buffer_id, prim.vertex_buffer_offset));
            if(prim.vertex_normal_buffer_id != UINT32_MAX)
                mesh.commands.back().attribute_offsets.push_back(CalculateAbsoluteOffset(m_parser, prim.vertex_normal_buffer_id, prim.vertex_normal_buffer_offset));
            if(prim.vertex_tangent_buffer_id != UINT32_MAX)
                mesh.commands.back().attribute_offsets.push_back(CalculateAbsoluteOffset(m_parser, prim.vertex_tangent_buffer_id, prim.vertex_tangent_buffer_offset));
            for(uint32_t j = 0; j < prim.texture_count; j++) {
                mesh.commands.back().attribute_offsets.push_back(CalculateAbsoluteOffset(m_parser, prim.uv_buffer_ids[j], prim.uv_buffer_offsets[j]));
                mesh.commands.back().texture_names.push_back(MISSING_TEXTURE_NAME);
            }
            for(uint32_t j = 0; j < prim.color_mul_count; j++)
                mesh.commands.back().attribute_offsets.push_back(CalculateAbsoluteOffset(m_parser, prim.color_mul_buffer_ids[j], prim.color_mul_buffer_offsets[j]));
            for(uint32_t j = 0; j < prim.joint_set_count; j++) {
                mesh.commands.back().attribute_offsets.push_back(CalculateAbsoluteOffset(m_parser, prim.joint_index_buffer_ids[j], prim.joint_index_buffer_offsets[j]));
                mesh.commands.back().attribute_offsets.push_back(CalculateAbsoluteOffset(m_parser, prim.joint_weight_buffer_ids[j], prim.joint_weight_buffer_offsets[j]));
            }

            // add morph targets
            for(uint32_t j = 0; j < prim.morph_target_count; j++) {
                const Libdas::DasMorphTarget &morph = m_parser.AccessMorphTarget(prim.morph_targets[j]);
                mesh.commands.back().attribute_offsets.push_back(CalculateAbsoluteOffset(m_parser, morph.vertex_buffer_id, morph.vertex_buffer_offset));
                if(morph.vertex_normal_buffer_id != UINT32_MAX)
                    mesh.commands.back().attribute_offsets.push_back(CalculateAbsoluteOffset(m_parser, morph.vertex_normal_buffer_id, morph.vertex_normal_buffer_offset));
                if(morph.vertex_tangent_buffer_id != UINT32_MAX)
                    mesh.commands.back().attribute_offsets.push_back(CalculateAbsoluteOffset(m_parser, morph.vertex_tangent_buffer_id, morph.vertex_tangent_buffer_offset));

                for(uint32_t k = 0; k < morph.texture_count; k++)
                    mesh.commands.back().attribute_offsets.push_back(CalculateAbsoluteOffset(m_parser, morph.uv_buffer_ids[k], morph.uv_buffer_offsets[k]));
                for(uint32_t k = 0; k < morph.color_mul_count; k++)
                    mesh.commands.back().attribute_offsets.push_back(CalculateAbsoluteOffset(m_parser, morph.color_mul_buffer_ids[k], morph.color_mul_buffer_offsets[k]));
            }
        }
    }


    void MeshLoader::UseTextures(const std::vector<std::string> &_names) {
        MeshReference &mesh = m_renderer.GetMeshes()[m_mesh_ref_id];

        for(auto cmd = mesh.commands.begin(); cmd != mesh.commands.end(); cmd++)
            cmd->texture_names = _names;
    }


    void MeshLoader::UpdateJointMatrices(const std::vector<Libdas::Matrix4<float>> &_matrices) {
#ifdef _DEBUG
        const Libdas::DasMeshPrimitive &prim = m_parser.AccessMeshPrimitive(m_mesh.primitives[0]);
        DENG_ASSERT(prim.joint_set_count * 4 >= _matrices.size());
#endif
        const uint32_t rel_offset = m_mesh_ubo_offset + m_renderer.AlignUniformBufferOffset(sizeof(ModelUbo));
        m_renderer.UpdateUniform(reinterpret_cast<const char*>(_matrices.data()), _matrices.size() * sizeof(Libdas::Matrix4<float>), rel_offset);
    }


    // setters / getters
    void MeshLoader::SetUniformBufferOffset(uint32_t _ubo_offset) {
        m_ubo_offset = _ubo_offset;
    }


    uint32_t MeshLoader::GetUboOffset() {
        return m_ubo_offset;
    }


    void MeshLoader::SetMainBufferOffset(uint32_t _main_offset) {
        m_main_buffer_offset = _main_offset;
    }

    uint32_t MeshLoader::GetMainBufferOffset() {
        return m_main_buffer_offset;
    }
}
