// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: MeshLoader.cpp - DAS data MeshLoader class implementation
// author: Karl-Mihkel Ott

#define MESH_LOADER_CPP
#include <MeshLoader.h>

namespace DENG {

    uint32_t MeshLoader::m_mesh_index = 0;

    MeshLoader::MeshLoader(
        const Libdas::DasMesh &_mesh, 
        Libdas::DasParser &_parser, 
        Renderer &_renderer, 
        const std::vector<uint32_t> &_main_buffer_offsets, 
        uint32_t _camera_offset, 
        uint32_t _skeleton_joint_count,
        const std::string &_framebuffer_id
    ) : 
        m_parser(_parser),
        m_mesh(_mesh),
        m_renderer(_renderer),
        m_mesh_buffer_offsets(_main_buffer_offsets),
        m_skeleton_joint_count(_skeleton_joint_count),
        m_framebuffer_id(_framebuffer_id)
    {
        if(m_mesh.name != "") {
            m_name = m_mesh.name;
        } 

        // request shader id to use for current mesh
        _CheckMeshPrimitives();
        mp_prim = &m_parser.AccessMeshPrimitive(m_mesh.primitives[0]);
        m_shader_id = ModelShaderManager::RequestShaderModule(m_renderer, m_parser, *mp_prim, _camera_offset, m_skeleton_joint_count, m_framebuffer_id);

        for(uint32_t i = 0; i < mp_prim->morph_target_count; i++)
            m_morph_weights[i] = mp_prim->morph_weights[i];

#ifdef DENG_EDITOR
        std::string id = m_name;
        std::transform(id.begin(), id.end(), id.begin(), [](unsigned char c){ return std::tolower(c); });
        m_inspector_name = "Mesh: " + m_name + "##" + id;
        m_color_checkbox_id = "Use a color##" + id;
        m_color_picker_id = "Pick a color##" + id;
        m_texture_button_id = "Select textures##" + id;
        m_texture_picker_id = "Texture picker##" + id;
        m_texture_save_id = "Save##" + id;
#endif
    }


    MeshLoader::MeshLoader(MeshLoader &&_ml) noexcept :
        m_mesh_ref_id(_ml.m_mesh_ref_id),
        m_parser(_ml.m_parser),
        m_mesh(_ml.m_mesh),
        m_renderer(_ml.m_renderer),
        m_name(std::move(_ml.m_name)),
        m_is_animation_target(_ml.m_is_animation_target),
        m_shader_id(_ml.m_shader_id),
        m_mesh_ubo_offset(_ml.m_mesh_ubo_offset),
        m_mesh_joints_ubo_offset(_ml.m_mesh_joints_ubo_offset),
        m_mesh_buffer_offsets(_ml.m_mesh_buffer_offsets),
        m_skeleton_joint_count(_ml.m_skeleton_joint_count),
        mp_prim(_ml.mp_prim),
        m_disable_joint_transforms(_ml.m_disable_joint_transforms),
        m_supported_texture_count(_ml.m_supported_texture_count),
        m_color(_ml.m_color),
#ifdef DENG_EDITOR
        m_framebuffer_id(_ml.m_framebuffer_id),
        m_is_colored(_ml.m_is_colored),
        m_used_textures(_ml.m_used_textures),
        m_inspector_name(std::move(_ml.m_inspector_name)),
        m_color_checkbox_id(std::move(_ml.m_color_checkbox_id)),
        m_color_picker_id(std::move(_ml.m_color_picker_id)),
        m_texture_picker_id(std::move(_ml.m_texture_picker_id)),
        m_texture_table(std::move(_ml.m_texture_table)),
        m_texture_save_id(std::move(_ml.m_texture_save_id))
#else
        m_framebuffer_id(_ml.m_framebuffer_id)
#endif
    {
        std::memcpy(m_morph_weights, _ml.m_morph_weights, sizeof(float) * MAX_MORPH_TARGETS);
    }


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
            const Libdas::DasMeshPrimitive &prim = m_parser.AccessMeshPrimitive(m_mesh.primitives[i]);
            if(!i) {
                attr_desc.normal = (prim.vertex_normal_buffer_id != UINT32_MAX);
                attr_desc.tangent = (prim.vertex_tangent_buffer_id != UINT32_MAX);
                attr_desc.texture_count = prim.texture_count;
                attr_desc.color_mul_count = prim.color_mul_count;
                attr_desc.joint_set_count = prim.joint_set_count;

                for(uint32_t j = 0; j < prim.morph_target_count; j++) {
                    const Libdas::DasMorphTarget &morph = m_parser.AccessMorphTarget(prim.morph_targets[i]);
                    attr_desc.morph_targets.emplace_back();
                    attr_desc.morph_targets.back().normal = (morph.vertex_normal_buffer_id != UINT32_MAX);
                    attr_desc.morph_targets.back().tangent = (morph.vertex_tangent_buffer_id != UINT32_MAX);
                    attr_desc.morph_targets.back().texture_count = morph.texture_count;
                    attr_desc.morph_targets.back().color_mul_count = morph.color_mul_count;
                }
            } else {
                MeshPrimitiveAttributeDescriptor cattr_desc;
                cattr_desc.normal = (prim.vertex_normal_buffer_id != UINT32_MAX);
                cattr_desc.tangent = (prim.vertex_tangent_buffer_id != UINT32_MAX);
                cattr_desc.texture_count = prim.texture_count;
                cattr_desc.color_mul_count = prim.color_mul_count;
                cattr_desc.joint_set_count = prim.joint_set_count;

                for(uint32_t j = 0; j < prim.morph_target_count; j++) {
                    const Libdas::DasMorphTarget &morph = m_parser.AccessMorphTarget(prim.morph_targets[i]);
                    cattr_desc.morph_targets.emplace_back();
                    cattr_desc.morph_targets.back().normal = (morph.vertex_normal_buffer_id != UINT32_MAX);
                    cattr_desc.morph_targets.back().tangent = (morph.vertex_tangent_buffer_id != UINT32_MAX);
                    cattr_desc.morph_targets.back().texture_count = morph.texture_count;
                    cattr_desc.morph_targets.back().color_mul_count = morph.color_mul_count;
                }

                // check if created attribute descriptor matches the original descriptor
                if(cattr_desc != attr_desc) {
                    WARNME("Mesh '" << m_mesh.name << "' contains primitives with different attributes");
                    std::exit(LIBDAS_ERROR_INVALID_FILE);
                }
            }
        }

        m_supported_texture_count = attr_desc.texture_count;
    }


    void MeshLoader::Attach() {
        m_mesh_ref_id = m_renderer.NewMeshReference(m_framebuffer_id);
        MeshReference &mesh = m_renderer.GetMesh(m_mesh_ref_id, m_framebuffer_id);
        mesh.name = m_mesh.name;
        mesh.shader_module_id = m_shader_id;

        GPUMemoryManager *mem_manager = GPUMemoryManager::GetInstance();

        // ModelUbo
        uint32_t binding_id = 1;
        mesh.ubo_blocks.reserve(3);
        mesh.ubo_blocks.emplace_back();
        mesh.ubo_blocks.back().binding = binding_id++;
        mesh.ubo_blocks.back().size = static_cast<uint32_t>(sizeof(ModelUbo));
        mesh.ubo_blocks.back().offset = mem_manager->RequestUniformMemoryLocationP(m_renderer, static_cast<uint32_t>(sizeof(ModelUbo)));
        m_mesh_ubo_offset = mesh.ubo_blocks.back().offset;
        m_renderer.UpdateUniform(nullptr, static_cast<uint32_t>(sizeof(ModelUbo)), mesh.ubo_blocks.back().offset);

        // check if joint matrix ubos should be created
        if(m_parser.AccessMeshPrimitive(m_mesh.primitives[0]).joint_set_count) {
            mesh.ubo_blocks.emplace_back();
            mesh.ubo_blocks.back().binding = binding_id++;
            mesh.ubo_blocks.back().size = m_skeleton_joint_count * static_cast<uint32_t>(sizeof(Libdas::Matrix4<float>));
            mesh.ubo_blocks.back().offset = mem_manager->RequestUniformMemoryLocationP(m_renderer, m_skeleton_joint_count * static_cast<uint32_t>(sizeof(Libdas::Matrix4<float>)));
            m_mesh_joints_ubo_offset = mesh.ubo_blocks.back().offset;
            m_renderer.UpdateUniform(nullptr, m_skeleton_joint_count * static_cast<uint32_t>(sizeof(Libdas::Matrix4<float>)), mesh.ubo_blocks.back().offset);
        }

        // create mesh draw commands
        mesh.commands.reserve(m_mesh.primitive_count);
        for(uint32_t i = 0; i < m_mesh.primitive_count; i++) {
            mesh.commands.emplace_back();
            const Libdas::DasMeshPrimitive &prim = m_parser.AccessMeshPrimitive(m_mesh.primitives[i]);
            uint32_t abs = m_mesh_buffer_offsets[prim.index_buffer_id] + prim.index_buffer_offset;
            mesh.commands.back().indices_offset = abs;
            mesh.commands.back().draw_count = prim.indices_count;
            mesh.commands.back().attribute_offsets.reserve(3 + prim.texture_count + prim.color_mul_count + prim.joint_set_count * 2);
            abs = m_mesh_buffer_offsets[prim.vertex_buffer_id] + prim.vertex_buffer_offset;
            mesh.commands.back().attribute_offsets.push_back(abs);
            if(prim.vertex_normal_buffer_id != UINT32_MAX) {
                abs = m_mesh_buffer_offsets[prim.vertex_normal_buffer_id] + prim.vertex_normal_buffer_offset;
                mesh.commands.back().attribute_offsets.push_back(abs);
            }
            if(prim.vertex_tangent_buffer_id != UINT32_MAX) {
                abs = m_mesh_buffer_offsets[prim.vertex_tangent_buffer_id] + prim.vertex_tangent_buffer_offset;
                mesh.commands.back().attribute_offsets.push_back(abs);
            }
            for(uint32_t j = 0; j < prim.texture_count; j++) {
                abs = m_mesh_buffer_offsets[prim.uv_buffer_ids[j]] + prim.uv_buffer_offsets[j];
                mesh.commands.back().attribute_offsets.push_back(abs);
                mesh.commands.back().texture_names.push_back(MISSING_TEXTURE_NAME);
            }
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

            // add morph targets
            for(uint32_t j = 0; j < prim.morph_target_count; j++) {
                const Libdas::DasMorphTarget &morph = m_parser.AccessMorphTarget(prim.morph_targets[j]);
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
    }


    void MeshLoader::UseTextures(const std::vector<std::string> &_names) {
        MeshReference &mesh = m_renderer.GetMesh(m_mesh_ref_id, m_framebuffer_id);

        for(auto cmd = mesh.commands.begin(); cmd != mesh.commands.end(); cmd++) {
            cmd->texture_names = _names;
        }
    }


    void MeshLoader::UpdateJointMatrices(const std::vector<Libdas::Matrix4<float>> &_matrices) {
        if(!m_disable_joint_transforms) {
            m_renderer.UpdateUniform(reinterpret_cast<const char*>(_matrices.data()), static_cast<uint32_t>(_matrices.size() * sizeof(Libdas::Matrix4<float>)), m_mesh_joints_ubo_offset);
        }
        else {
            std::vector<Libdas::Matrix4<float>> rmat(_matrices.size());
            m_renderer.UpdateUniform(reinterpret_cast<const char*>(rmat.data()), static_cast<uint32_t>(rmat.size() * sizeof(Libdas::Matrix4<float>)), m_mesh_joints_ubo_offset);
        }
    }
}
