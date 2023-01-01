// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelShaderManager.cpp - DAS model shader management singleton class implementation
// author: Karl-Mihkel Ott

#define MODEL_SHADER_MANAGER_CPP
#include "deng/ModelShaderManager.h"

namespace DENG {
    ModelShaderManager::map_t ModelShaderManager::m_shader_map = {};

    uint32_t ModelShaderManager::_GenerateShaderModule(
        Renderer &_rend, 
        const MeshPrimitiveAttributeDescriptor &_mesh_attr_desc, 
        uint32_t _camera_offset
    ) {
        ShaderModule module;
        module.enable_depth_testing = true;
        module.enable_blend = true;
        module.enable_indexing = _mesh_attr_desc.index;
        uint32_t binding_id = 0;

        // push some uniform data layouts
        module.ubo_data_layouts.reserve(5);
        module.ubo_data_layouts.push_back({
            { binding_id++, static_cast<uint32_t>(sizeof(ModelCameraUbo)), _camera_offset },
            UNIFORM_DATA_TYPE_BUFFER,
            SHADER_STAGE_VERTEX | SHADER_STAGE_FRAGMENT | SHADER_STAGE_GEOMETRY,
            UNIFORM_USAGE_PER_SHADER
        });

        // ModelUbo
        binding_id++;
        //module.ubo_data_layouts.push_back({
        //    { binding_id++, static_cast<uint32_t>(sizeof(ModelUbo)), 0 },
        //    UNIFORM_DATA_TYPE_BUFFER,
        //    SHADER_STAGE_VERTEX | SHADER_STAGE_FRAGMENT | SHADER_STAGE_FRAGMENT,
        //    UNIFORM_USAGE_PER_MESH
        //});

        module.attributes.reserve(3 + _mesh_attr_desc.texture_2d_count + _mesh_attr_desc.color_mul_count + 2 * _mesh_attr_desc.joint_set_count);
        module.attribute_strides.reserve(3 + _mesh_attr_desc.texture_2d_count + _mesh_attr_desc.color_mul_count + 2 * _mesh_attr_desc.joint_set_count);
        module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
        module.attribute_strides.push_back(sizeof(TRS::Vector3<float>));

        if(_mesh_attr_desc.normal) {
            module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
            module.attribute_strides.push_back(sizeof(TRS::Vector3<float>));
        }
        if(_mesh_attr_desc.tangent) {
            module.attributes.push_back(ATTRIBUTE_TYPE_VEC4_FLOAT);
            module.attribute_strides.push_back(sizeof(TRS::Vector4<float>));
        }

        for(uint32_t i = 0; i < _mesh_attr_desc.texture_2d_count; i++) {
            module.attributes.push_back(ATTRIBUTE_TYPE_VEC2_FLOAT);
            module.attribute_strides.push_back(sizeof(TRS::Vector2<float>));
        }

        for(uint32_t i = 0; i < _mesh_attr_desc.color_mul_count; i++) {
            module.attributes.push_back(ATTRIBUTE_TYPE_VEC4_FLOAT);
            module.attribute_strides.push_back(sizeof(TRS::Vector4<float>));
        }

        if(_mesh_attr_desc.joint_set_count) {
            for(uint32_t i = 0; i < _mesh_attr_desc.joint_set_count; i++) {
                module.attributes.push_back(ATTRIBUTE_TYPE_VEC4_USHORT);
                module.attribute_strides.push_back(sizeof(TRS::Vector4<uint16_t>));
                module.attributes.push_back(ATTRIBUTE_TYPE_VEC4_FLOAT);
                module.attribute_strides.push_back(sizeof(TRS::Vector4<float>));
            }
        }

        for(auto it = _mesh_attr_desc.morph_targets.begin(); it != _mesh_attr_desc.morph_targets.end(); it++) {
            module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
            module.attribute_strides.push_back(sizeof(TRS::Vector3<float>));
            if(it->normal) {
                module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
                module.attribute_strides.push_back(sizeof(TRS::Vector3<float>));
            }

            if(it->tangent) {
                module.attributes.push_back(ATTRIBUTE_TYPE_VEC4_FLOAT);
                module.attribute_strides.push_back(sizeof(TRS::Vector4<float>));
            }

            for(uint32_t i = 0; i < it->texture_2d_count; i++) {
                module.attributes.push_back(ATTRIBUTE_TYPE_VEC2_FLOAT);
                module.attribute_strides.push_back(sizeof(TRS::Vector2<float>));
            }

            for(uint32_t i = 0; i < it->color_mul_count; i++) {
                module.attributes.push_back(ATTRIBUTE_TYPE_VEC4_FLOAT);
                module.attribute_strides.push_back(sizeof(TRS::Vector4<float>));
            }
        }

        module.enable_2d_textures = _mesh_attr_desc.texture_2d_count > 0;
        module.enable_3d_textures = true;
        module.vertex_shader_src = ModelShaderGenerator::GenerateVertexShaderSource(_mesh_attr_desc);
        module.fragment_shader_src = ModelShaderGenerator::GenerateFragmentShaderSource(_mesh_attr_desc);
        return _rend.PushShaderModule(module);
    }


    uint32_t ModelShaderManager::RequestShaderModule(
        Renderer &_rend, 
        Libdas::DasModel &_model, 
        const Libdas::DasMeshPrimitive &_prim,
        uint32_t _camera_offset, 
        uint32_t _skeleton_joint_count
    ) {
        // assemble MeshPrimitiveAttributeDescriptor object
        MeshPrimitiveAttributeDescriptor attr_desc;
        attr_desc.normal = (_prim.vertex_normal_buffer_id != UINT32_MAX);
        attr_desc.tangent = (_prim.vertex_tangent_buffer_id != UINT32_MAX);
        attr_desc.index = (_prim.index_buffer_id != UINT32_MAX);
        attr_desc.texture_2d_count = _prim.texture_count;
        attr_desc.color_mul_count = _prim.color_mul_count;
        attr_desc.joint_set_count = _prim.joint_set_count;
        attr_desc.skeleton_joint_count = _skeleton_joint_count;

        for(uint32_t i = 0; i < _prim.morph_target_count; i++) {
            const Libdas::DasMorphTarget &morph = _model.morph_targets[_prim.morph_targets[i]];
            attr_desc.morph_targets.emplace_back();
            attr_desc.morph_targets.back().normal = (morph.vertex_normal_buffer_id != UINT32_MAX);
            attr_desc.morph_targets.back().tangent = (morph.vertex_tangent_buffer_id != UINT32_MAX);
            attr_desc.morph_targets.back().texture_2d_count = morph.texture_count;
            attr_desc.morph_targets.back().color_mul_count = morph.color_mul_count;
        }
        
        // check if current MeshPrimitiveAttributeDescriptor object is present
        if(m_shader_map.find(attr_desc) == m_shader_map.end()) {
            m_shader_map[attr_desc] = _GenerateShaderModule(_rend, attr_desc, _camera_offset);
        }

        return m_shader_map[attr_desc];
    }
}
