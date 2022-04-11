// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelShaderGenerator.cpp - DAS model shader generation singleton class implementation
// author: Karl-Mihkel Ott

#define MODEL_SHADER_GENERATOR_CPP
#include <ModelShaderGenerator.h>

namespace DENG {

    std::string ModelShaderGenerator::GenerateVertexShaderSource(uint32_t _mask) {
        std::string shader = SHADER_HEADING;

        const size_t id_var_len = std::string("${ID}").size();
        uint32_t layout_id = 0;

        // input attributes
        if(_mask & POS_ATTR_MASK) {
            std::string pos_attr = POSITION_VERTEX_INPUT;
            pos_attr.replace(pos_attr.find("${ID}"), id_var_len, std::to_string(layout_id++));
            shader += pos_attr;
        } 
        if(_mask & UV_ATTR_MASK) {
            std::string uv_attr = UV_VERTEX_INPUT;
            uv_attr.replace(uv_attr.find("${ID}"), id_var_len, std::to_string(layout_id++));
            shader += uv_attr;
        }
        if(_mask & NORMAL_ATTR_MASK) {
            std::string norm_attr = NORM_VERTEX_INPUT;
            norm_attr.replace(norm_attr.find("${ID}"), id_var_len, std::to_string(layout_id++));
            shader += norm_attr;
        }
        if(_mask & TANG_ATTR_MASK) {
            std::string tang_attr = TANG_VERTEX_INPUT;
            tang_attr.replace(tang_attr.find("${ID}"), id_var_len, std::to_string(layout_id++));
            shader += tang_attr;
        }

        // output attributes
        std::string out_code;
        layout_id = 0;
        if(_mask & UV_ATTR_MASK) {
            std::string uv_attr = UV_VERTEX_OUTPUT; 
            std::string color_attr = COLOR_VERTEX_OUTPUT;
            std::string is_colored_attr = IS_COLOR_OUTPUT;

            uv_attr.replace(uv_attr.find("${ID}"), id_var_len, std::to_string(layout_id++));
            color_attr.replace(color_attr.find("${ID}"), id_var_len, std::to_string(layout_id++));
            is_colored_attr.replace(is_colored_attr.find("${ID}"), id_var_len, std::to_string(layout_id++));

            shader += uv_attr;
            shader += color_attr;
            shader += is_colored_attr;

            out_code = "out_uv = in_uv;\nout_color = model.color;\nout_is_colored = model.is_color;\n";
        } else {
            std::string color_attr = COLOR_VERTEX_OUTPUT;
            color_attr.replace(color_attr.find("${ID}"), id_var_len, std::to_string(layout_id++));
            shader += color_attr;

            out_code = "out_color = model.color;";
        }

        // add uniform blocks and functions
        shader += VERTEX_SHADER_UBO_SECTION;

        // main function
        std::string main = VERTEX_MAIN;
        main.replace(main.find("${CUSTOM_CODE}"), 14, out_code);
        shader += main;

        return shader;
    }


    const std::string ModelShaderGenerator::GetSampledFragmentShaderSource() {
        return SAMPLED_FRAG_SRC;
    }

    const std::string ModelShaderGenerator::GetColoredFragmentShaderSource() {
        return COLORED_FRAG_SRC;
    }
}
