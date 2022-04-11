// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelShaderGenerator.h - DAS model shader generation singleton class header
// author: Karl-Mihkel Ott

#ifndef MODEL_SHADER_GENERATOR_H
#define MODEL_SHADER_GENERATOR_H

#ifdef MODEL_SHADER_GENERATOR_CPP
    #include <string>

#define DENG_DEFS_ONLY
    #include<ModelShaderManager.h>
#undef DENG_DEFS_ONLY

    // Declarations ${ID} and ${CUSTOM_CODE} specify layout id and custom code in main() correspondingly
    #define SHADER_HEADING              "#version 450\n" \
                                        "#extension GL_ARB_separate_shader_objects : enable\n"

    // input attributes
    #define POSITION_VERTEX_INPUT       "layout(location = ${ID}) in vec3 in_pos;\n"
    #define UV_VERTEX_INPUT             "layout(location = ${ID})in vec2 in_uv;\n"
    #define NORM_VERTEX_INPUT           "layout(location = ${ID}) in vec3 in_norm;\n"
    #define TANG_VERTEX_INPUT           "layout(location = ${ID}) in vec4 in_tang;\n"

    // output attributes
    #define UV_VERTEX_OUTPUT            "layout(location = ${ID}) out vec2 out_uv;\n"
    #define COLOR_VERTEX_OUTPUT         "layout(location = ${ID}) out vec4 out_color;\n"
    #define IS_COLOR_OUTPUT             "layout(location = ${ID}) out uint out_is_colored;\n"

    #define VERTEX_SHADER_UBO_SECTION   "// Camera uniform data\n"\
                                        "layout(set = 0, binding = 0) uniform CameraUbo {\n"\
                                        "    mat4 projection;\n"\
                                        "    mat4 view;\n"\
                                        "} camera;\n"\
                                        "\n"\
                                        "layout(std140, set = 1, binding = 1) uniform AnimationUbo {\n"\
                                        "   mat4 rotation;\n"\
                                        "   mat4 scale;\n"\
                                        "   mat4 translation;\n"\
                                        "} animation;\n"\
                                        "\n"\
                                        "// Model uniform data\n"\
                                        "layout(set = 1, binding = 2) uniform ModelUbo {\n"\
                                        "   mat4 node;\n"\
                                        "   mat4 skeleton;\n"\
                                        "   vec4 color;\n"\
                                        "   uint is_color;\n"\
                                        "} model;\n"


    #define VERTEX_MAIN                 "void main() {\n"\
                                        "    mat4 a = animation.translation * animation.rotation * animation.scale;\n"\
                                        "    mat4 c = camera.projection * camera.view;\n"\
                                        "    mat4 m = model.skeleton * model.node;\n"\
                                        "\n"\
                                        "    gl_Position = c * a * m * vec4(in_pos, 1.0f);\n"\
                                        "    ${CUSTOM_CODE}\n"\
                                        "}\n"

    #define SAMPLED_FRAG_SRC            "#version 450\n"\
                                        "#extension GL_ARB_separate_shader_objects : enable\n"\
                                        "\n"\
                                        "/* VULKAN ONLY */\n"\
                                        "\n"\
                                        "// input data\n"\
                                        "layout(location = 0) in vec2 i_uv;\n"\
                                        "layout(location = 1) in vec4 i_color;\n"\
                                        "layout(location = 2) in flat uint i_is_colored;\n"\
                                        "\n"\
                                        "layout(set = 0, binding = 3) uniform sampler2D tex_sampler;\n"\
                                        "\n"\
                                        "// output data\n"\
                                        "layout(location = 0) out vec4 o_color;\n"\
                                        "\n"\
                                        "void main() {\n"\
                                        "   if(i_is_colored == 0)\n"\
                                        "       o_color = texture(tex_sampler, i_uv);\n"\
                                        "   else o_color = i_color;\n"\
                                        "}\n"

    #define COLORED_FRAG_SRC            "#version 450\n"\
                                        "#extension GL_ARB_separate_shader_objects : enable\n"\
                                        "\n"\
                                        "layout(location = 0) in vec4 i_color;\n"\
                                        "layout(location = 0) out vec4 o_color;\n"\
                                        "\n"\
                                        "void main() {\n"\
                                        "    o_color = i_color;\n"\
                                        "}\n"

#endif

namespace DENG {

    struct ModelShaderGenerator {
        static std::string GenerateVertexShaderSource(uint32_t _mask );
        static const std::string GetSampledFragmentShaderSource();
        static const std::string GetColoredFragmentShaderSource();
    };
}

#endif
