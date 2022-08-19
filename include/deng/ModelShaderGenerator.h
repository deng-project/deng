// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelShaderGenerator.h - DAS model shader generation singleton class header
// author: Karl-Mihkel Ott

#ifndef MODEL_SHADER_GENERATOR_H
#define MODEL_SHADER_GENERATOR_H

#ifdef MODEL_SHADER_GENERATOR_CPP
    #include <string>
    #include <vector>
    #include <queue>
    #include <utility>
    #include <cstring>
    #include <unordered_map>

#ifdef __DEBUG
    #include <iostream>
#endif

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"

    #include "deng/Api.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ModelUniforms.h"
#define DENG_DEFS_ONLY
    #include "deng/ModelShaderManager.h"
#undef DENG_DEFS_ONLY
    #include "deng/ShaderDefinitions.h"
    #include "deng/Window.h"
    #include "deng/RenderState.h"
    #include "deng/Renderer.h"

    // Declarations ${ID} and ${CUSTOM_CODE} specify layout id and custom code in main() correspondingly
    #define SHADER_HEADING              "#version 450\n"\
                                        "#extension GL_ARB_separate_shader_objects : enable\n"\
                                        "layout(std140, ${SET} binding = 0) uniform CameraUbo {\n"\
                                        "    mat4 projection;\n"\
                                        "    mat4 view;\n"\
                                        "} camera;\n"\
                                        "\n"\
                                        "// Model uniform data\n"\
                                        "layout(std140, ${SET} binding = 1) uniform ModelUbo {\n"\
                                        "   mat4 node;\n"\
                                        "   vec4 color;\n"\
                                        "   vec4 morph_weights[" + std::to_string(MAX_MORPH_TARGETS / 4) + "];\n"\
                                        "   uint is_color;\n"\
                                        "} model;\n"

    #define VERTEX_MAIN                 "void main() {\n"\
                                        "    mat4 custom = mat4(1.0f);\n"\
                                        "    mat4 cam = camera.projection * camera.view;\n"\
                                        "    mat4 m = model.node;\n"\
                                        "\n"\
                                        "${CUSTOM_CODE}\n"\
                                        "}\n"
#endif

namespace DENG {

    struct MeshPrimitiveAttributeDescriptor {
        const bool pos = true;
        bool normal = false;
        bool tangent = false;
        uint32_t texture_count = 0;
        uint32_t color_mul_count = 0;
        uint32_t joint_set_count = 0;
        uint32_t skeleton_joint_count = 0;
        std::vector<MeshPrimitiveAttributeDescriptor> morph_targets;

        inline bool operator==(const MeshPrimitiveAttributeDescriptor &_m1) const {
            return _m1.pos == pos &&
                   _m1.normal == normal &&
                   _m1.texture_count == texture_count &&
                   _m1.color_mul_count == color_mul_count &&
                   _m1.joint_set_count == joint_set_count &&
                   _m1.morph_targets == morph_targets;
        }

        inline bool operator!=(const MeshPrimitiveAttributeDescriptor &_m1) const {
            return !(_m1 == *this);
        }
    };

    class ModelShaderGenerator {
        private:
            static uint32_t m_in_id;
            static uint32_t m_out_id;
            static uint32_t m_binding_id;

        private:
            static void _WriteBaseAttributes(const MeshPrimitiveAttributeDescriptor &_mesh_attr_desc, std::string &_shader, std::string &_custom_code);
            static void _WriteMorphTargets(const MeshPrimitiveAttributeDescriptor &_mesh_attr_desc, std::string &_shader, std::string &_custom_code);
            static void _MixTextures(const MeshPrimitiveAttributeDescriptor &_mesh_attr_desc, std::string &_shader, std::string &_custom_code);

        public:
            static std::string GenerateVertexShaderSource(const MeshPrimitiveAttributeDescriptor &_mesh_attr_desc);
            static std::string GenerateFragmentShaderSource(const MeshPrimitiveAttributeDescriptor &_mesh_attr_desc);
    };
}

#endif
