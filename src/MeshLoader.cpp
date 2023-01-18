// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: MeshLoader.cpp - DAS data MeshLoader class implementation
// author: Karl-Mihkel Ott

#define MESH_LOADER_CPP
#include "deng/MeshLoader.h"

namespace DENG {

    uint32_t MeshLoader::m_mesh_index = 0;

    MeshLoader::MeshLoader(
        Entity *_parent,
        const Libdas::DasMesh &_mesh, 
        Libdas::DasModel *_model, 
        Renderer &_renderer, 
        const std::vector<uint32_t> &_main_buffer_offsets, 
        uint32_t _camera_offset, 
        uint32_t _skeleton_joint_count,
        const std::vector<uint32_t>& _framebuffers
    ) : 
        Entity(_parent, "", ENTITY_TYPE_MESH),
        mp_model(_model),
        m_mesh(_mesh),
        m_renderer(_renderer),
        m_mesh_buffer_offsets(_main_buffer_offsets),
        m_skeleton_joint_count(_skeleton_joint_count),
        m_framebuffer_ids(_framebuffers)
    {
        std::string name = "Unnamed mesh";
        if(m_mesh.name != "") {
            name = m_mesh.name;
        } else {
            name += std::to_string(m_mesh_index++);
        }
        SetName(name);

        // request shader id to use for current mesh
        _CheckMeshPrimitives();
        mp_prim = &mp_model->mesh_primitives[m_mesh.primitives[0]];
        m_shader_id = ModelShaderManager::RequestShaderModule(m_renderer, *mp_model, *mp_prim, _camera_offset, m_skeleton_joint_count);

        for(uint32_t i = 0; i < mp_prim->morph_target_count; i++)
            m_morph_weights[i] = mp_prim->morph_weights[i];
    }


    MeshLoader::MeshLoader(MeshLoader &&_ml) noexcept :
        Entity(std::move(_ml)),
        m_mesh_ref_id(_ml.m_mesh_ref_id),
        mp_model(_ml.mp_model),
        m_mesh(_ml.m_mesh),
        m_renderer(_ml.m_renderer),
        m_is_animation_target(_ml.m_is_animation_target),
        m_shader_id(_ml.m_shader_id),
        m_mesh_ubo_offset(_ml.m_mesh_ubo_offset),
        m_mesh_joints_ubo_offset(_ml.m_mesh_joints_ubo_offset),
        m_mesh_buffer_offsets(_ml.m_mesh_buffer_offsets),
        m_skeleton_joint_count(_ml.m_skeleton_joint_count),
        mp_prim(_ml.mp_prim),
        m_disable_joint_transforms(_ml.m_disable_joint_transforms),
        m_supported_texture_count(_ml.m_supported_texture_count),
        m_morph_weights(std::move(_ml.m_morph_weights)),
        m_color(_ml.m_color),
        m_framebuffer_ids(std::move(_ml.m_framebuffer_ids)),
        m_is_colored(_ml.m_is_colored) {}


    MeshLoader::~MeshLoader() {
        GPUMemoryManager *mem_manager = GPUMemoryManager::GetInstance();
        // remove joint ubo region from memory manager
        if(m_mesh_joints_ubo_offset != UINT32_MAX)
            mem_manager->DeleteUniformMemoryLocation(m_mesh_joints_ubo_offset);

        // remove mesh ubo region from memory manager
        mem_manager->DeleteUniformMemoryLocation(m_mesh_ubo_offset);
    }


    void MeshLoader::_CheckMeshPrimitives() {
        // if mesh is valid then each mesh primitive must contain same vertex attributes
        DENG_ASSERT(m_mesh.primitive_count);
        MeshPrimitiveAttributeDescriptor attr_desc;
        for(uint32_t i = 0; i < m_mesh.primitive_count; i++) {
            const Libdas::DasMeshPrimitive &prim = mp_model->mesh_primitives[m_mesh.primitives[i]];
            if(!i) {
                attr_desc.normal = (prim.vertex_normal_buffer_id != UINT32_MAX);
                attr_desc.tangent = (prim.vertex_tangent_buffer_id != UINT32_MAX);
                attr_desc.index = (prim.index_buffer_id != UINT32_MAX);
                attr_desc.texture_2d_count = prim.texture_count;
                attr_desc.color_mul_count = prim.color_mul_count;
                attr_desc.joint_set_count = prim.joint_set_count;

                for(uint32_t j = 0; j < prim.morph_target_count; j++) {
                    const Libdas::DasMorphTarget &morph = mp_model->morph_targets[prim.morph_targets[i]];
                    attr_desc.morph_targets.emplace_back();
                    attr_desc.morph_targets.back().normal = (morph.vertex_normal_buffer_id != UINT32_MAX);
                    attr_desc.morph_targets.back().tangent = (morph.vertex_tangent_buffer_id != UINT32_MAX);
                    attr_desc.morph_targets.back().texture_2d_count = morph.texture_count;
                    attr_desc.morph_targets.back().color_mul_count = morph.color_mul_count;
                }
            } else {
                MeshPrimitiveAttributeDescriptor cattr_desc;
                cattr_desc.normal = (prim.vertex_normal_buffer_id != UINT32_MAX);
                cattr_desc.tangent = (prim.vertex_tangent_buffer_id != UINT32_MAX);
                cattr_desc.index = (prim.index_buffer_id != UINT32_MAX);
                cattr_desc.texture_2d_count = prim.texture_count;
                cattr_desc.color_mul_count = prim.color_mul_count;
                cattr_desc.joint_set_count = prim.joint_set_count;

                for(uint32_t j = 0; j < prim.morph_target_count; j++) {
                    const Libdas::DasMorphTarget &morph = mp_model->morph_targets[prim.morph_targets[i]];
                    cattr_desc.morph_targets.emplace_back();
                    cattr_desc.morph_targets.back().normal = (morph.vertex_normal_buffer_id != UINT32_MAX);
                    cattr_desc.morph_targets.back().tangent = (morph.vertex_tangent_buffer_id != UINT32_MAX);
                    cattr_desc.morph_targets.back().texture_2d_count = morph.texture_count;
                    cattr_desc.morph_targets.back().color_mul_count = morph.color_mul_count;
                }

                // check if created attribute descriptor matches the original descriptor
                if(cattr_desc != attr_desc) {
                    WARNME("Mesh '" << m_mesh.name << "' contains primitives with different attributes");
                    std::exit(LIBDAS_ERROR_INVALID_FILE);
                }
            }
        }

        m_supported_texture_count = attr_desc.texture_2d_count;
    }


    void MeshLoader::Attach() {
        m_environment_map = m_renderer.GetMissing3DTextureHandle();

        MeshReference mesh;
        mesh.name = m_mesh.name;
        mesh.shader_module_id = m_shader_id;

        GPUMemoryManager *mem_manager = GPUMemoryManager::GetInstance();

        uint32_t binding_id = 1;
        mesh.ubo_data_layouts.reserve(3);

        // ModelUbo
        mesh.ubo_data_layouts.emplace_back();
        mesh.ubo_data_layouts.back().block.binding = binding_id++;
        mesh.ubo_data_layouts.back().block.size = static_cast<uint32_t>(sizeof(ModelUbo));
        mesh.ubo_data_layouts.back().block.offset = mem_manager->RequestUniformMemoryLocationP(m_renderer, static_cast<uint32_t>(sizeof(ModelUbo)));
        mesh.ubo_data_layouts.back().stage = SHADER_STAGE_VERTEX | SHADER_STAGE_GEOMETRY | SHADER_STAGE_FRAGMENT;
        mesh.ubo_data_layouts.back().type = UNIFORM_DATA_TYPE_BUFFER;
        mesh.ubo_data_layouts.back().usage = UNIFORM_USAGE_PER_MESH;

        mesh.framebuffer_ids.push_back(0);
        mesh.enable = true;
        m_mesh_ubo_offset = mesh.ubo_data_layouts.back().block.offset;
        m_renderer.UpdateUniform(nullptr, static_cast<uint32_t>(sizeof(ModelUbo)), mesh.ubo_data_layouts.back().block.offset);

        // check if joint matrix ubos should be created
        if(mp_model->mesh_primitives[m_mesh.primitives[0]].joint_set_count) {
            mesh.ubo_data_layouts.emplace_back();
            mesh.ubo_data_layouts.back().block.binding = binding_id++;
            mesh.ubo_data_layouts.back().block.size = m_skeleton_joint_count * static_cast<uint32_t>(sizeof(TRS::Matrix4<float>));
            mesh.ubo_data_layouts.back().block.offset = mem_manager->RequestUniformMemoryLocationP(m_renderer, m_skeleton_joint_count * static_cast<uint32_t>(sizeof(TRS::Matrix4<float>)));
            mesh.ubo_data_layouts.back().stage = SHADER_STAGE_VERTEX | SHADER_STAGE_GEOMETRY | SHADER_STAGE_FRAGMENT;
            mesh.ubo_data_layouts.back().type = UNIFORM_DATA_TYPE_BUFFER;
            mesh.ubo_data_layouts.back().usage = UNIFORM_USAGE_PER_MESH;

            m_mesh_joints_ubo_offset = mesh.ubo_data_layouts.back().block.offset;
            m_renderer.UpdateUniform(nullptr, m_skeleton_joint_count * static_cast<uint32_t>(sizeof(TRS::Matrix4<float>)), mesh.ubo_data_layouts.back().block.offset);
        }

        // 2D texture samplers
        for (uint32_t i = 0; i < mp_model->mesh_primitives[m_mesh.primitives[0]].texture_count; i++) {
            mesh.ubo_data_layouts.emplace_back();
            mesh.ubo_data_layouts.back().block.binding = binding_id++;
            mesh.ubo_data_layouts.back().stage = SHADER_STAGE_FRAGMENT;
            mesh.ubo_data_layouts.back().type = UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER;
            mesh.ubo_data_layouts.back().usage = UNIFORM_USAGE_PER_MESH;
        }

        // 3D texture sampler
        mesh.ubo_data_layouts.emplace_back();
        mesh.ubo_data_layouts.back().block.binding = binding_id++;
        mesh.ubo_data_layouts.back().stage = SHADER_STAGE_FRAGMENT;
        mesh.ubo_data_layouts.back().type = UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER;
        mesh.ubo_data_layouts.back().usage = UNIFORM_USAGE_PER_MESH;

        // create mesh draw commands
        mesh.commands.reserve(m_mesh.primitive_count);
        m_normal_visualizers.reserve(m_mesh.primitive_count);
        
        for(uint32_t i = 0; i < m_mesh.primitive_count; i++) {
            mesh.commands.emplace_back();
            const Libdas::DasMeshPrimitive &prim = mp_model->mesh_primitives[m_mesh.primitives[i]];
            
            uint32_t abs = 0;
            uint32_t ioff = UINT32_MAX;
            if (prim.index_buffer_id != UINT32_MAX) {
                abs = m_mesh_buffer_offsets[prim.index_buffer_id] + prim.index_buffer_offset;
                ioff = abs;
                mesh.commands.back().indices_offset = abs;
            }
            mesh.commands.back().draw_count = prim.draw_count;
            mesh.commands.back().attribute_offsets.reserve(3 + prim.texture_count + prim.color_mul_count + prim.joint_set_count * 2);
            
            abs = m_mesh_buffer_offsets[prim.vertex_buffer_id] + prim.vertex_buffer_offset;
            uint32_t voff = abs;
            mesh.commands.back().attribute_offsets.push_back(abs);
            if(prim.vertex_normal_buffer_id != UINT32_MAX) {
                abs = m_mesh_buffer_offsets[prim.vertex_normal_buffer_id] + prim.vertex_normal_buffer_offset;
                uint32_t noff = abs;
                mesh.commands.back().attribute_offsets.push_back(abs);
                m_normal_visualizers.emplace_back(this, "__nviz__", m_renderer, voff, noff, ioff, prim.draw_count, m_framebuffer_ids);
            }
            if(prim.vertex_tangent_buffer_id != UINT32_MAX) {
                abs = m_mesh_buffer_offsets[prim.vertex_tangent_buffer_id] + prim.vertex_tangent_buffer_offset;
                mesh.commands.back().attribute_offsets.push_back(abs);
            }
            for(uint32_t j = 0; j < prim.texture_count; j++) {
                abs = m_mesh_buffer_offsets[prim.uv_buffer_ids[j]] + prim.uv_buffer_offsets[j];
                mesh.commands.back().attribute_offsets.push_back(abs);
                mesh.commands.back().texture_ids.push_back(m_renderer.GetMissing2DTextureHandle());
            }
            mesh.commands.back().texture_ids.push_back(m_renderer.GetMissing3DTextureHandle());
            for(uint32_t j = 0; j < prim.color_mul_count; j++) {
                abs = m_mesh_buffer_offsets[prim.color_mul_buffer_ids[j]] + prim.color_mul_buffer_offsets[j];
                mesh.commands.back().attribute_offsets.push_back(abs);
            }
            for(uint32_t j = 0; j < prim.joint_set_count; j++) {
                abs = m_mesh_buffer_offsets[prim.joint_index_buffer_ids[j]] + prim.joint_index_buffer_offsets[j];
                mesh.commands.back().attribute_offsets.push_back(abs);
                abs = m_mesh_buffer_offsets[prim.joint_weight_buffer_ids[j]] + prim.joint_weight_buffer_offsets[j];
                mesh.commands.back().attribute_offsets.push_back(abs);
            }

            // add morph target attributes
            for(uint32_t j = 0; j < prim.morph_target_count; j++) {
                const Libdas::DasMorphTarget &morph = mp_model->morph_targets[prim.morph_targets[j]];
                abs = m_mesh_buffer_offsets[morph.vertex_buffer_id] + morph.vertex_buffer_offset;
                mesh.commands.back().attribute_offsets.push_back(abs);
                if(morph.vertex_normal_buffer_id != UINT32_MAX) {
                    abs = m_mesh_buffer_offsets[morph.vertex_normal_buffer_id] + morph.vertex_normal_buffer_offset;
                    mesh.commands.back().attribute_offsets.push_back(abs);
                }
                if(morph.vertex_tangent_buffer_id != UINT32_MAX) {
                    abs = m_mesh_buffer_offsets[morph.vertex_tangent_buffer_id] + morph.vertex_tangent_buffer_offset;
                    mesh.commands.back().attribute_offsets.push_back(abs);
                }

                for(uint32_t k = 0; k < morph.texture_count; k++) {
                    abs = m_mesh_buffer_offsets[morph.uv_buffer_ids[k]] + morph.uv_buffer_offsets[k];
                    mesh.commands.back().attribute_offsets.push_back(abs);
                }
                for(uint32_t k = 0; k < morph.color_mul_count; k++) {
                    abs = m_mesh_buffer_offsets[morph.color_mul_buffer_ids[k]] + morph.color_mul_buffer_offsets[k];
                    mesh.commands.back().attribute_offsets.push_back(abs);
                }
            }
        }

        m_mesh_ref_id = m_renderer.PushMeshReference(mesh);
    }


    void MeshLoader::UseTextures(const std::vector<uint32_t>& _handles) {
        MeshReference &mesh = m_renderer.GetMesh(m_mesh_ref_id);

        for(auto cmd = mesh.commands.begin(); cmd != mesh.commands.end(); cmd++) {
            cmd->texture_ids = _handles;
            cmd->texture_ids.push_back(m_environment_map);
            cmd->swap_textures_bit = true;
        }
    }


    void MeshLoader::SetEnvironmentMap(uint32_t _handle) {
        m_environment_map = _handle;
        MeshReference& ref = m_renderer.GetMesh(m_mesh_ref_id);

        for (auto it = ref.commands.begin(); it != ref.commands.end(); it++) {
            if (mp_prim->texture_count > it->texture_ids.size()) {
                it->texture_ids.resize(mp_prim->texture_count + 1, UINT32_MAX);
            }
            it->texture_ids.back() = m_environment_map;
            it->swap_textures_bit = true;
        }
    }


    void MeshLoader::UpdateJointMatrices(const std::vector<TRS::Matrix4<float>> &_matrices) {
        VertexNormalVisualizer::Update(m_renderer);
        if(!m_disable_joint_transforms) {
            m_renderer.UpdateUniform(reinterpret_cast<const char*>(_matrices.data()), static_cast<uint32_t>(_matrices.size() * sizeof(TRS::Matrix4<float>)), m_mesh_joints_ubo_offset);
        }
        else {
            std::vector<TRS::Matrix4<float>> rmat(_matrices.size());
            m_renderer.UpdateUniform(reinterpret_cast<const char*>(rmat.data()), static_cast<uint32_t>(rmat.size() * sizeof(TRS::Matrix4<float>)), m_mesh_joints_ubo_offset);
        }
    }
}
