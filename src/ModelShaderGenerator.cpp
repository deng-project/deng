// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelShaderGenerator.cpp - DAS model shader generation singleton class implementation
// author: Karl-Mihkel Ott

#define MODEL_SHADER_GENERATOR_CPP
#include "deng/ModelShaderGenerator.h"

namespace DENG {

    uint32_t ModelShaderGenerator::m_in_id = 0;
    uint32_t ModelShaderGenerator::m_out_id = 0;
    uint32_t ModelShaderGenerator::m_binding_id = 0;

    void ModelShaderGenerator::_WriteBaseAttributes(const MeshPrimitiveAttributeDescriptor &_mesh_attr_desc, std::string &_shader, std::string &_custom_code) {
        if(_mesh_attr_desc.pos) {
            _shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec3 in_pos;\n";
            _shader += "layout(location = " + std::to_string(m_out_id++) + ") out vec3 out_pos;\n";
            _custom_code += "\tvec3 pos = in_pos;\n";
        }
        if(_mesh_attr_desc.normal) {
            _shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec3 in_norm;\n";
            _shader += "layout(location = " + std::to_string(m_out_id++) + ") out vec3 out_norm;\n";
            _custom_code += "\tvec3 norm = in_norm;\n";
        }
        if(_mesh_attr_desc.tangent) {
            _shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec4 in_tang;\n";
            _shader += "layout(location = " + std::to_string(m_out_id++) + ") out vec4 out_tang;\n";
            _custom_code += "\tvec4 tang = in_tang;\n";
        }

        // texture coordinates
        for(uint32_t i = 0; i < _mesh_attr_desc.texture_2d_count; i++) {
            _shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec2 in_uv" + std::to_string(i) + ";\n";
            _shader += "layout(location = " + std::to_string(m_out_id++) + ") out vec2 out_uv" + std::to_string(i) + ";\n"; 
            _custom_code += "\tvec2 uv" + std::to_string(i) +" = in_uv" + std::to_string(i) + ";\n"; 
        }

        // color multipliers
        for(uint32_t i = 0; i < _mesh_attr_desc.color_mul_count; i++) {
            const std::string in_color = "in_col" + std::to_string(i);
            const std::string out_color = "out_col" + std::to_string(i);
            _shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec4 " + in_color + ";\n";
            _shader += "layout(location = " + std::to_string(m_out_id++) + ") out vec4 " + out_color + ";\n";
            _custom_code += "\tvec4 color" + std::to_string(i) + " = " + in_color + ";\n"; 
        }

        // write joint transformations
        if(_mesh_attr_desc.joint_set_count) {
            _shader += "layout(std140, ${SET} binding = 2) uniform JointMatrices { mat4 mat[" + std::to_string(_mesh_attr_desc.skeleton_joint_count) + "]; } joint_matrices;\n";
            _custom_code += "\tcustom = \n";
            for(uint32_t i = 0; i < _mesh_attr_desc.joint_set_count; i++) {
                _shader += "layout(location = " + std::to_string(m_in_id++) + ") in uvec4 joints" + std::to_string(i) + ";\n";
                _shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec4 weights" + std::to_string(i) + ";\n";
                if(i == _mesh_attr_desc.joint_set_count - 1) {
                    _custom_code += "\t\tweights" + std::to_string(i) + ".x * joint_matrices.mat[int(joints" + std::to_string(i) + ".x)] +\n"\
                                    "\t\tweights" + std::to_string(i) + ".y * joint_matrices.mat[int(joints" + std::to_string(i) + ".y)] +\n"\
                                    "\t\tweights" + std::to_string(i) + ".z * joint_matrices.mat[int(joints" + std::to_string(i) + ".z)] +\n"\
                                    "\t\tweights" + std::to_string(i) + ".w * joint_matrices.mat[int(joints" + std::to_string(i) + ".w)];\n";
                } else {
                    _custom_code += "\t\tweights" + std::to_string(i) + ".x * joint_matrices.mat[int(joints" + std::to_string(i) + ".x)] +\n"\
                                    "\t\tweights" + std::to_string(i) + ".y * joint_matrices.mat[int(joints" + std::to_string(i) + ".y)] +\n"\
                                    "\t\tweights" + std::to_string(i) + ".z * joint_matrices.mat[int(joints" + std::to_string(i) + ".z)] +\n"\
                                    "\t\tweights" + std::to_string(i) + ".w * joint_matrices.mat[int(joints" + std::to_string(i) + ".w)] +\n";
                }
            }
        }
    }


    void ModelShaderGenerator::_WriteMorphTargets(const MeshPrimitiveAttributeDescriptor &_mesh_attr_desc, std::string &_shader, std::string &_custom_code) {
        // write morph targets
        if(_mesh_attr_desc.morph_targets.size()) {
            const uint32_t morphed_uv_count = _mesh_attr_desc.morph_targets.front().texture_2d_count;
            const uint32_t morphed_color_count = _mesh_attr_desc.morph_targets.front().color_mul_count;
            const bool is_normal = _mesh_attr_desc.morph_targets.front().normal;
            const bool is_tang = _mesh_attr_desc.morph_targets.front().tangent;
            DENG_ASSERT(_mesh_attr_desc.morph_targets.size() <= MAX_MORPH_TARGETS);

            for(auto it = _mesh_attr_desc.morph_targets.begin(); it != _mesh_attr_desc.morph_targets.end(); it++) {
                const size_t index = it - _mesh_attr_desc.morph_targets.begin();
                DENG_ASSERT(is_normal == it->normal);
                DENG_ASSERT(is_tang == it->tangent);
                DENG_ASSERT(morphed_uv_count == it->texture_2d_count);
                DENG_ASSERT(morphed_color_count == it->color_mul_count);
                DENG_ASSERT(!it->texture_2d_count || it->texture_2d_count == _mesh_attr_desc.texture_2d_count);
                DENG_ASSERT(!it->color_mul_count || it->color_mul_count == _mesh_attr_desc.color_mul_count);

                std::string str;

                if(it->pos)
                    _shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec3 in_morph_pos" + std::to_string(index) + ";\n";
                if(it->normal)
                    _shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec3 in_morph_normal" + std::to_string(index) + ";\n";
                if(it->tangent)
                    _shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec4 in_morph_tangent" + std::to_string(index) + ";\n";
                for(uint32_t i = 0; i < it->texture_2d_count; i++)
                    _shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec2 in_morph_uv" + std::to_string(index) + "_" + std::to_string(i) + ";\n";
                for(uint32_t i = 0; i < it->color_mul_count; i++)
                    _shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec4 in_morph_color" + std::to_string(index) + "_" + std::to_string(i) + ";\n";
            }

            // write morphed position attributes
            _custom_code += "\tpos += ";
            for(auto it = _mesh_attr_desc.morph_targets.begin(); it != _mesh_attr_desc.morph_targets.end(); it++) {
                const size_t index = it - _mesh_attr_desc.morph_targets.begin();
                const size_t arr_index = index / 4;

                char suf = 'x' + (index % 4);
                if((index % 4) == 3)
                    suf = 'w';

                if(it != _mesh_attr_desc.morph_targets.end() - 1)
                    _custom_code += "model.morph_weights[" + std::to_string(arr_index) + "]." + suf + " * in_morph_pos" + std::to_string(index) + " + ";
                else _custom_code += "model.morph_weights[" + std::to_string(arr_index) + "]." + suf + " * in_morph_pos" + std::to_string(index) + ";\n";
            }

            // write morphed normal attributes if possible
            if(is_normal) {
                _custom_code += "\tnorm += ";
                for(auto it = _mesh_attr_desc.morph_targets.begin(); it != _mesh_attr_desc.morph_targets.end(); it++) {
                    const size_t index = it - _mesh_attr_desc.morph_targets.begin();
                    const size_t arr_index = index / 4;

                    char suf = 'x' + (index % 4);
                    if((index % 4) == 3)
                        suf = 'w';

                    if(it != _mesh_attr_desc.morph_targets.end() - 1)
                        _custom_code += "model.morph_weights[" + std::to_string(arr_index) + "]." + suf + " * in_morph_normal" + std::to_string(index) + " + ";
                    else _custom_code += "model.morph_weights[" + std::to_string(arr_index) + "]." + suf + " * in_morph_normal" + std::to_string(index) + ";\n";
                }
            }

            // write morphed vertex tangents if possible
            if(is_tang) {
                _custom_code += "\ttang += ";
                for(auto it = _mesh_attr_desc.morph_targets.begin(); it != _mesh_attr_desc.morph_targets.end(); it++) {
                    const size_t index = it - _mesh_attr_desc.morph_targets.begin();
                    const size_t arr_index = index / 4;

                    char suf = 'x' + (index % 4);
                    if((index % 4) == 3)
                        suf = 'w';

                    if(it != _mesh_attr_desc.morph_targets.end() - 1)
                        _custom_code += "model.morph_weights[" + std::to_string(arr_index) + "]." + suf + " * in_morph_tangent" + std::to_string(index) + " + ";
                    else _custom_code += "model.morph_weights[" + std::to_string(arr_index) + "]." + suf + " * in_morph_tangent" + std::to_string(index) + ";\n";
                }
            }

            // write morphed uv attributes
            if(morphed_uv_count) {
                for(uint32_t i = 0; i < morphed_uv_count; i++) {
                    _custom_code += "\tuv" + std::to_string(i) + " += ";
                    for(auto it = _mesh_attr_desc.morph_targets.begin(); it != _mesh_attr_desc.morph_targets.end(); it++) {
                        const size_t index = it - _mesh_attr_desc.morph_targets.begin();
                        const size_t arr_index = index / 4;

                        char elem = (index % 4) + 'x';
                        if((index % 4) == 3)
                            elem = 'w';

                        if(it != _mesh_attr_desc.morph_targets.end() - 1)
                            _custom_code += "model.morph_weights[" + std::to_string(arr_index) + "]." + elem + " * in_morph_uv" + std::to_string(index) + "_" + std::to_string(i) + " + ";
                        else _custom_code += "model.morph_weights[" + std::to_string(arr_index) + "]." + elem + " * in_morph_uv" + std::to_string(index) + "_" + std::to_string(i) + ";\n";
                    }
                }
            }

            // write morphed color attributes
            if(morphed_color_count) {
                for(uint32_t i = 0; i < morphed_color_count; i++) {
                    _custom_code += "\tcolor" + std::to_string(i) + "+= ";
                    for(auto it = _mesh_attr_desc.morph_targets.begin(); it != _mesh_attr_desc.morph_targets.end(); it++) {
                        const size_t index = it - _mesh_attr_desc.morph_targets.begin();
                        if(it != _mesh_attr_desc.morph_targets.end() - 1)
                            _custom_code += "model.morph_weights[" + std::to_string(index) + "] * in_morph_color" + std::to_string(index) + "_" + std::to_string(i) + " + ";
                        else _custom_code += "model.morph_weights[" + std::to_string(index) + "] * in_morph_color" + std::to_string(index) + "_" + std::to_string(i) + ";\n";
                    }
                }
            }
        } 
    }


    void ModelShaderGenerator::_MixTextures(const MeshPrimitiveAttributeDescriptor& _mesh_attr_desc, std::string& _shader, std::string& _custom_code) {
        DENG_ASSERT(!_mesh_attr_desc.color_mul_count || 
                    !_mesh_attr_desc.texture_2d_count || 
                    _mesh_attr_desc.color_mul_count == _mesh_attr_desc.texture_2d_count);

        // write texture coordinate and color multiplier attributes
        RenderState* rs = RenderState::GetInstance();
        for (uint32_t i = 0; i < _mesh_attr_desc.texture_2d_count; i++) {
            _shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec2 uv" + std::to_string(i) + ";\n";
            const std::string id = std::to_string(i);
            _shader += "layout(${SET} binding = " + std::to_string(m_binding_id++) + ") uniform sampler2D smp" + id + ";\n";
        }

        _shader += "layout(${SET} binding = " + std::to_string(m_binding_id++) + ") uniform samplerCube env;\n";

        for(uint32_t i = 0; i < _mesh_attr_desc.color_mul_count; i++)
            _shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec4 col" + std::to_string(i) + ";\n";

        if(_mesh_attr_desc.texture_2d_count) {
            _custom_code += "\t\tfloat tex_alpha = " + std::to_string(1.0f / static_cast<float>(_mesh_attr_desc.texture_2d_count)) + ";\n";
            for(uint32_t i = 0; i < _mesh_attr_desc.texture_2d_count; i++) {
                if(i < _mesh_attr_desc.color_mul_count)
                    _custom_code += "\t\tvec4 tcol" + std::to_string(i) + " = col" + std::to_string(i) + " * texture(smp" + std::to_string(i) + ", uv" + std::to_string(i) + ");\n";
                else _custom_code += "\t\tvec4 tcol" + std::to_string(i) + " = texture(smp" + std::to_string(i) + ", uv" + std::to_string(i) + ");\n";
            }

            // texture mixing part
            // c_n - nth color 
            // m_n - nth mixed color
            std::queue<std::pair<char, uint32_t>> mix_ids;
            for(int32_t i = static_cast<int32_t>(_mesh_attr_desc.texture_2d_count - 1); i >= 0; i--)
                mix_ids.push(std::make_pair('c', i));

            uint32_t max_mix = 0;
            while(!mix_ids.empty()) {
                std::pair<char, uint32_t> cpair = mix_ids.front();
                mix_ids.pop();
                if(mix_ids.empty()) {
                    if(cpair.first == 'c')
                        _custom_code += "\t\tout_color = tcol" + std::to_string(cpair.second) + ";\n";
                    else if(cpair.first == 'm')
                        _custom_code += "\t\tout_color = m" + std::to_string(cpair.second) + ";\n";
                }
                else {
                    std::pair<char, uint32_t> npair = mix_ids.front();
                    mix_ids.pop();
                    const std::string var1 = (cpair.first == 'c' ? "tcol" : "m") + std::to_string(cpair.second);
                    const std::string var2 = (npair.first == 'c' ? "tcol" : "m") + std::to_string(npair.second);
                    _custom_code += "\t\tvec4 m" + std::to_string(max_mix) + " = mix(" + var1 + ", " + var2 + ", tex_alpha);\n";
                    mix_ids.push(std::pair('m', max_mix++));
                }
            }
        } else if(_mesh_attr_desc.color_mul_count) {
            _custom_code += "\t\tout_color = ";
            for(uint32_t i = 0; i < _mesh_attr_desc.color_mul_count; i++) {
                if(i != _mesh_attr_desc.color_mul_count - 1)
                    _custom_code += "col" + std::to_string(i) + " * ";
                else _custom_code += "col" + std::to_string(i) + ";\n";
            }
        } else {
            _custom_code += "\t\tout_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n";
        }
    }


    std::string ModelShaderGenerator::GenerateVertexShaderSource(const MeshPrimitiveAttributeDescriptor &_mesh_attr_desc) {
        m_in_id = 0;
        m_out_id = 0;
        std::string shader = SHADER_HEADING;
        std::string body = VERTEX_MAIN;
        std::string custom_code = "";

        _WriteBaseAttributes(_mesh_attr_desc, shader, custom_code);
        _WriteMorphTargets(_mesh_attr_desc, shader, custom_code);

        // finalize outputs
        custom_code += "\tout_pos = vec3(custom * vec4(pos, 1.f));\n";
        if(_mesh_attr_desc.normal)
            custom_code += "\tout_norm = norm;\n";
        if(_mesh_attr_desc.tangent)
            custom_code += "\tout_tang = tang;\n";
        for(uint32_t i = 0; i < _mesh_attr_desc.texture_2d_count; i++)
            custom_code += "\tout_uv" + std::to_string(i) + " = uv" + std::to_string(i) + ";\n";
        for(uint32_t i = 0; i < _mesh_attr_desc.color_mul_count; i++)
            custom_code += "\tout_col" + std::to_string(i) + " = color" + std::to_string(i) + ";\n";

        // position calculation
        custom_code += "\tgl_Position = cam * m * custom * vec4(pos, 1.0f);\n";

        // ${SET} variable calculations
        std::size_t pos = 0, fpos = 0, i = 0;
        while((fpos = shader.find("${SET}", pos)) != std::string::npos) {
            RenderState *rs = RenderState::GetInstance();
            switch(rs->GetPrimary()) {
                case RENDERER_TYPE_VULKAN:
                    if(i < SHADER_SET_COUNT) {
                        shader = shader.replace(fpos, std::strlen("${SET}"), "set = 0,");
                    } else {
                        shader = shader.replace(fpos, std::strlen("${SET}"), "set = 1,");
                    }
                    break;

                case RENDERER_TYPE_OPENGL:
                    shader = shader.replace(fpos, std::strlen("${SET}"), "");
                    break;

                default:
                    break;
            }

            i++;
            pos = fpos;
        }

        fpos = body.find("${CUSTOM_CODE}");
        const std::string mod_body = body.replace(fpos, std::strlen("${CUSTOM_CODE}"), custom_code);
        shader += mod_body;
        return shader;
    }


    std::string ModelShaderGenerator::GenerateFragmentShaderSource(const MeshPrimitiveAttributeDescriptor &_mesh_attr_desc) {
        m_in_id = 0;
        m_out_id = 0;
        m_binding_id = _mesh_attr_desc.joint_set_count ? 3 : 2;
        std::string shader = SHADER_HEADING;  

        std::string body;
        if (_mesh_attr_desc.normal) {
            body = "void main() {\n"\
                   "\tif(model.is_color != 0 && model.use_environment_map == 0)\n"\
                   "\t\tout_color = model.color;\n"\
                   "\telse if (model.use_environment_map == 0) {\n"\
                   "${CUSTOM_CODE}"\
                   "\t}\n"\
                   "\telse {\n"\
                   "${CUSTOM_CODE}\n"\
                   "\t}\n"\
                   "}";
        } else {
            body = "void main() {\n"\
                   "\tif(model.is_color != 0)\n"\
                   "\t\tout_color = model.color;\n"\
                   "\telse {\n"\
                   "${CUSTOM_CODE}\n"\
                   "\t}\n"\
                   "}";
        }
        std::string custom_code = "";

        shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec3 in_pos;\n";
        if(_mesh_attr_desc.normal)
            shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec3 in_norm;\n";
        if(_mesh_attr_desc.tangent)
            shader += "layout(location = " + std::to_string(m_in_id++) + ") in vec4 in_tang;\n";
        shader += "layout(location = 0) out vec4 out_color;\n";

        _MixTextures(_mesh_attr_desc, shader, custom_code);

        // ${SET} variable configuration
        {
            std::size_t pos = 0, fpos = 0, i = 0;
            while ((fpos = shader.find("${SET}", pos)) != std::string::npos) {
                RenderState* rs = RenderState::GetInstance();
                switch (rs->GetPrimary()) {
                case RENDERER_TYPE_VULKAN:
                    if (i < SHADER_SET_COUNT) {
                        shader = shader.replace(fpos, std::strlen("${SET}"), "set = 0,");
                    }
                    else {
                        shader = shader.replace(fpos, std::strlen("${SET}"), "set = 1,");
                    }
                    break;

                case RENDERER_TYPE_OPENGL:
                    shader = shader.replace(fpos, std::strlen("${SET}"), "");
                    break;

                default:
                    break;
                }

                i++;
                pos = fpos;
            }
        }

        auto pos = body.find("${CUSTOM_CODE}");
        std::string mod_body = body.replace(pos, std::strlen("${CUSTOM_CODE}"), custom_code);

        if (_mesh_attr_desc.normal) {
            custom_code =
                "\t\tvec3 I = normalize(in_pos - camera.pos.xyz);\n"\
                "\t\tvec3 R = reflect(I, normalize(in_norm));\n"\
                "\t\tout_color = vec4(texture(env, R).rgb, 1.f);\n";
            pos = body.find("${CUSTOM_CODE}");
            mod_body = mod_body.replace(pos, std::strlen("${CUSTOM_CODE}"), custom_code);
        }
        shader += mod_body;
        return shader;
    }
}
