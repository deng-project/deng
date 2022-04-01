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

    #define VERTEX_SHADER_UBO_SECTION   "// Camera uniform data\n"\
                                        "layout(set = 0, binding = 0) uniform CameraUbo {\n"\
                                        "    mat4 projection;\n"\
                                        "    mat4 view;\n"\
                                        "} camera;\n"\
                                        "\n"\
                                        "#define TARGET_MASK_WEIGHT              0x00000001\n"\
                                        "#define TARGET_MASK_ROTATION            0x00000002\n"\
                                        "#define TARGET_MASK_TRANSLATION         0x00000004\n"\
                                        "#define TARGET_MASK_SCALE               0x00000008\n"\
                                        "\n"\
                                        "#define INTERPOLATION_TYPE_LINEAR       0\n"\
                                        "#define INTERPOLATION_TYPE_STEP         1\n"\
                                        "#define INTERPOLATION_TYPE_CUBICSPLINE  2\n"\
                                        "\n"\
                                        "layout(set = 1, binding = 1) uniform AnimationUbo {\n"\
                                        "    vec4 rotations[2];      // quaternions\n"\
                                        "    vec4 weights[2];        // point structures\n"\
                                        "    vec4 translations[2];   // point structures\n"\
                                        "    float scales[2];        // uniform scales\n"\
                                        "\n"\
                                        "    uint target_mask;\n"\
                                        "    uint interpolation_mode;\n"\
                                        "    uint animate;\n"\
                                        "\n"\
                                        "    float timestamps[2];\n"\
                                        "    float current_time;\n"\
                                        "    uint used_weights;\n"\
                                        "} animation;\n"\
                                        "\n"\
                                        "// Model uniform data\n"\
                                        "layout(set = 1, binding = 2) uniform ModelUbo {\n"\
                                        "    mat4 node;\n"\
                                        "    mat4 skeleton;\n"\
                                        "    vec4 color;\n"\
                                        "} model;\n"

    #define QUATERNION_OPERATIONS       "vec4 QConjugate(vec4 _q1) {\n"\
                                        "    return vec4(-_q1.xyz, _q1.w);\n"\
                                        "}\n"\
                                        "\n"\
                                        "\n"\
                                        "vec4 QInverse(vec4 _q1) {\n"\
                                        "    vec4 conj = QConjugate(_q1);\n"\
                                        "    float len = _q1.length();\n"\
                                        "    return conj / (len * len);\n"\
                                        "}\n"\
                                        "\n"\
                                        "\n"\
                                        "vec4 QGrassman(vec4 _p, vec4 _q) {\n"\
                                        "    return vec4(vec3(_p.w * _q.xyz + _q.w * _p.xyz + cross(_p.xyz, _q.xyz)), (_p.w * _q.w - dot(_p.xyz, _q.xyz)));\n"\
                                        "}\n"\
                                        "\n"\
                                        "\n"\
                                        "mat4 QToMatrix(vec4 _q) {\n"\
                                        "    float x2 = _q.x * _q.x;\n"\
                                        "    float y2 = _q.y * _q.y;\n"\
                                        "    float z2 = _q.z * _q.z;\n"\
                                        "\n"\
                                        "    float xy = _q.x * _q.y;\n"\
                                        "    float xz = _q.x * _q.z;\n"\
                                        "    float xw = _q.x * _q.w;\n"\
                                        "\n"\
                                        "    float yz = _q.y * _q.z;\n"\
                                        "    float yw = _q.y * _q.w;\n"\
                                        "\n"\
                                        "    float zw = _q.z * _q.w;\n"\
                                        "\n"\
                                        "    mat4 m = mat4(\n"\
                                        "        (1 - 2 * y2 - 2 * z2), (2 * xy + 2 * zw), (2 * xz - 2 * yw), 0,\n"\
                                        "        (2 * xy - 2 * zw), (1 - 2 * x2 - 2 * z2), (2 * yz + 2 * xw), 0,\n"\
                                        "        (2 * xz - 2 * yw), (2 * yz - 2 * xw), (1 - 2 * x2 - 2 * y2), 0,\n"\
                                        "        0, 0, 0, 1\n"\
                                        "    );\n"\
                                        "\n\n"\
                                        "    return m;\n"\
                                        "}\n"


    #define INTERP_FUNCTIONS            "mat4 LinearInterpolation() {\n"\
                                        "    mat4 interp_mat = mat4(1.0f);\n"\
                                        "    float t = (animation.current_time - animation.timestamps[0]) / (animation.timestamps[1] - animation.timestamps[0]);\n"\
                                        "\n"\
                                        "    switch(animation.target_mask) {\n"\
                                        "        // not supported\n"\
                                        "        case TARGET_MASK_WEIGHT:\n"\
                                        "            break;\n"\
                                        "\n"\
                                        "        case TARGET_MASK_ROTATION:\n"\
                                        "            {\n"\
                                        "                // first keyframe\n"\
                                        "                float rot1 = acos(animation.rotations[0].w) * 2;\n"\
                                        "                float sinf1 = sin(rot1 / 2);\n"\
                                        "                vec3 scale1 = animation.rotations[0].xyz / sinf1;\n"\
                                        "\n"\
                                        "                float rot2 = acos(animation.rotations[1].w) * 2;\n"\
                                        "                float sinf2 = sin(rot2 / 2);\n"\
                                        "                vec3 scale2 = animation.rotations[1].xyz / sinf2;\n"\
                                        "\n"\
                                        "                float irot = (1 - t) * rot1 + t * rot2;\n"\
                                        "                vec3 iscale = (1 - t) * scale1 + t * scale2;\n"\
                                        "\n"\
                                        "                mat4 x = mat4(\n"\
                                        "                    1, 0, 0, 0,\n"\
                                        "                    0, cos(irot * iscale.x), sin(irot * iscale.x), 0,\n"\
                                        "                    0, -sin(irot * iscale.x), cos(irot * iscale.x), 0,\n"\
                                        "                    0, 0, 0, 1\n"\
                                        "                );\n"\
                                        "\n"\
                                        "                mat4 y = mat4(\n"\
                                        "                    cos(irot * iscale.y), 0, -sin(irot * iscale.y), 0,\n"\
                                        "                    0, 1, 0, 0,\n"\
                                        "                    sin(irot * iscale.y), 0, cos(irot * iscale.y), 0,\n"\
                                        "                    0, 0, 0, 1\n"\
                                        "                );\n"\
                                        "\n"\
                                        "                mat4 z = mat4(\n"\
                                        "                    cos(irot * iscale.z), sin(irot * iscale.z), 0, 0,\n"\
                                        "                    -sin(irot * iscale.z), cos(irot * iscale.z), 0, 0,\n"\
                                        "                    0, 0, 1, 0,\n"\
                                        "                    0, 0, 0, 1\n"\
                                        "                );\n"\
                                        "\n"\
                                        "                interp_mat *= z * y * x;\n"\
                                        "            }\n"\
                                        "            break;\n"\
                                        "\n"\
                                        "        case TARGET_MASK_TRANSLATION:\n"\
                                        "            {\n"\
                                        "                vec4 itranslation = (1 - t) * animation.translations[0] + t * animation.translations[1];\n"\
                                        "                interp_mat[3][0] = itranslation.x;\n"\
                                        "                interp_mat[3][1] = itranslation.y;\n"\
                                        "                interp_mat[3][2] = itranslation.z;\n"\
                                        "            }\n"\
                                        "            break;\n"\
                                        "\n"\
                                        "        case TARGET_MASK_SCALE:\n"\
                                        "            {\n"\
                                        "                float iscale = (1 - t) * animation.scales[0] + t * animation.scales[1];\n"\
                                        "                interp_mat[0][0] = iscale;\n"\
                                        "                interp_mat[1][1] = iscale;\n"\
                                        "                interp_mat[2][2] = iscale;\n"\
                                        "            }\n"\
                                        "            break;\n"\
                                        "    }\n"\
                                        "\n"\
                                        "    return interp_mat;\n"\
                                        "}\n"\
                                        "\n"\
                                        "\n"\
                                        "mat4 StepInterpolation() {\n"\
                                        "    mat4 interp_mat = mat4(1.0f);\n"\
                                        "\n"\
                                        "    switch(animation.target_mask) {\n"\
                                        "        case TARGET_MASK_WEIGHT:\n"\
                                        "            // not supported\n"\
                                        "            break;\n"\
                                        "\n"\
                                        "        case TARGET_MASK_ROTATION:\n"\
                                        "            interp_mat = QToMatrix(animation.rotations[0]);\n"\
                                        "            break;\n"\
                                        "\n"\
                                        "        case TARGET_MASK_TRANSLATION:\n"\
                                        "            interp_mat[3][0] = animation.translations[0].x;\n"\
                                        "            interp_mat[3][1] = animation.translations[0].y;\n"\
                                        "            interp_mat[3][2] = animation.translations[0].z;\n"\
                                        "            break;\n"\
                                        "\n"\
                                        "        case TARGET_MASK_SCALE:\n"\
                                        "            interp_mat[0][0] = animation.scales[0];\n"\
                                        "            interp_mat[1][1] = animation.scales[0];\n"\
                                        "            interp_mat[2][2] = animation.scales[0];\n"\
                                        "            break;\n"\
                                        "\n"\
                                        "        default:\n"\
                                        "            break;\n"\
                                        "    }\n"\
                                        "\n"\
                                        "    return interp_mat;\n"\
                                        "}\n"\
                                        "\n"\
                                        "\n"\
                                        "mat4 CalculateAnimation() {\n"\
                                        "    mat4 ani_mat = mat4(1.0f);\n"\
                                        "    float t = (animation.current_time - animation.timestamps[0]) / (animation.timestamps[1] - animation.timestamps[0]);\n"\
                                        "        \n"\
                                        "    // for now only linear and step interpolations are supported\n"\
                                        "    if(animation.animate == 1) {\n"\
                                        "        switch(animation.interpolation_mode) {\n"\
                                        "            case INTERPOLATION_TYPE_LINEAR:\n"\
                                        "                ani_mat = LinearInterpolation();\n"\
                                        "                break;\n"\
                                        "\n"\
                                        "            case INTERPOLATION_TYPE_CUBICSPLINE:\n"\
                                        "                // not supported yet\n"\
                                        "                break;\n"\
                                        "            \n"\
                                        "            case INTERPOLATION_TYPE_STEP:\n"\
                                        "                ani_mat = StepInterpolation();\n"\
                                        "                break;\n"\
                                        "\n"\
                                        "            default:\n"\
                                        "                break;\n"\
                                        "        }\n"\
                                        "    }\n"\
                                        "\n"\
                                        "    return ani_mat;\n"\
                                        "}\n"

    #define VERTEX_MAIN                 "void main() {\n"\
                                        "    // TODO: Calculate transformation matrices for each uniform block\n"\
                                        "    mat4 a = CalculateAnimation();\n"\
                                        "    mat4 c = camera.projection * camera.view;\n"\
                                        "    mat4 m = model.skeleton * model.node;\n"\
                                        "\n"\
                                        "    gl_Position = a * c * m * vec4(in_pos, 1.0f);\n"\
                                        "    ${CUSTOM_CODE}\n"\
                                        "}\n"

    #define SAMPLED_FRAG_SRC            "#version 450\n"\
                                        "#extension GL_ARB_separate_shader_objects : enable\n"\
                                        "\n"\
                                        "/* VULKAN ONLY */\n"\
                                        "\n"\
                                        "// input data\n"\
                                        "layout(location = 0) in vec2 i_uv;\n"\
                                        "\n"\
                                        "layout(set = 0, binding = 3) uniform sampler2D tex_sampler;\n"\
                                        "\n"\
                                        "// output data\n"\
                                        "layout(location = 0) out vec4 o_color;\n"\
                                        "\n"\
                                        "void main() {\n"\
                                        "    o_color = texture(tex_sampler, i_uv);\n"\
                                        "}\n"

    #define COLORED_FRAG_SRC            "#version 450\n"\
                                        "#extension GL_ARB_separate_shader_objects : enable\n"\
                                        "\n"\
                                        "layout(location = 0) in vec4 i_color;\n"\
                                        "layout(location = 0) out vec4 o_color;\n"\
                                        "\n"\
                                        "void main() {\n"\
                                        "    o_color = i_color;\n"\
                                        "}\n";

#endif

namespace DENG {

    struct ModelShaderGenerator {
        static std::string GenerateVertexShaderSource(uint32_t _mask );
        static const std::string GetSampledFragmentShaderSource();
        static const std::string GetColoredFragmentShaderSource();
    };
}

#endif
