// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ShaderDefinitions.h - Shader module definitions header
// author: Karl-Mihkel Ott


#ifndef SHADER_DEFINITIONS_H
#define SHADER_DEFINITIONS_H

#ifdef SHADER_DEFINITIONS_CPP
    #include <cstdint>
    #include <vector>
    #include <string>
    #include <ErrorDefinitions.h>
#endif


namespace DENG {
    enum AttributeType {
        // single element attribute
        ATTRIBUTE_TYPE_FLOAT,
        ATTRIBUTE_TYPE_DOUBLE,
        ATTRIBUTE_TYPE_BYTE,
        ATTRIBUTE_TYPE_UBYTE,
        ATTRIBUTE_TYPE_SHORT,
        ATTRIBUTE_TYPE_USHORT,
        ATTRIBUTE_TYPE_UINT,
        ATTRIBUTE_TYPE_INT,

        // two element attribute
        ATTRIBUTE_TYPE_VEC2_FLOAT,
        ATTRIBUTE_TYPE_VEC2_DOUBLE,
        ATTRIBUTE_TYPE_VEC2_BYTE,
        ATTRIBUTE_TYPE_VEC2_UBYTE,
        ATTRIBUTE_TYPE_VEC2_SHORT,
        ATTRIBUTE_TYPE_VEC2_USHORT,
        ATTRIBUTE_TYPE_VEC2_UINT,
        ATTRIBUTE_TYPE_VEC2_INT,

        // three element attribute
        ATTRIBUTE_TYPE_VEC3_FLOAT,
        ATTRIBUTE_TYPE_VEC3_DOUBLE,
        ATTRIBUTE_TYPE_VEC3_BYTE,
        ATTRIBUTE_TYPE_VEC3_UBYTE,
        ATTRIBUTE_TYPE_VEC3_SHORT,
        ATTRIBUTE_TYPE_VEC3_USHORT,
        ATTRIBUTE_TYPE_VEC3_UINT,
        ATTRIBUTE_TYPE_VEC3_INT,

        // four element attribute
        ATTRIBUTE_TYPE_VEC4_FLOAT,
        ATTRIBUTE_TYPE_VEC4_DOUBLE,
        ATTRIBUTE_TYPE_VEC4_BYTE,
        ATTRIBUTE_TYPE_VEC4_UBYTE,
        ATTRIBUTE_TYPE_VEC4_SHORT,
        ATTRIBUTE_TYPE_VEC4_USHORT,
        ATTRIBUTE_TYPE_VEC4_UINT,
        ATTRIBUTE_TYPE_VEC4_INT
    };


    enum UniformDataType {
        UNIFORM_DATA_TYPE_BUFFER,
        UNIFORM_DATA_TYPE_IMAGE_SAMPLER
    };


    enum UniformUsage {
        UNIFORM_USAGE_PER_SHADER,
        UNIFORM_USAGE_PER_MESH
    };


    typedef uint8_t ShaderStage;
    #define SHADER_STAGE_VERTEX     0x01
    #define SHADER_STAGE_GEOMETRY   0x02
    #define SHADER_STAGE_FRAGMENT   0x04

    struct UniformBufferBlock {
        uint32_t binding = 0;
        uint32_t size = 0;
        uint32_t offset = 0;
    };

    // Structure to be passed into the renderer
    struct UniformDataLayout {
        UniformBufferBlock block = {};
        UniformDataType type;
        ShaderStage stage;
        UniformUsage usage = UNIFORM_USAGE_PER_SHADER;
    };

    enum CullMode {
        CULL_MODE_NONE,
        CULL_MODE_COUNTER_CLOCKWISE,
        CULL_MODE_CLOCKWISE
    };


    // this structure defines shaders that should be used in a pipeline
    // NOTE: geometry shaders are completely optional and not required
    struct ShaderModule {
        std::vector<AttributeType> attributes;
        std::vector<std::size_t> offsets;
        std::vector<UniformDataLayout> ubo_data_layouts; // per shader uniforms
        std::string vertex_shader_file = "";
        std::string geometry_shader_file = "";
        std::string fragment_shader_file = "";
        std::string vertex_shader_src;
        std::string geometry_shader_src;
        std::string fragment_shader_src;
        bool use_texture_mapping = false;
        bool load_shaders_from_file = false;
        bool enable_scissor = false;
        bool enable_depth_testing = false;
        bool enable_stencil_testing = false;
        bool enable_blend = false;
        bool use_seperate_attribute_strides = false;
        CullMode cull_mode = CULL_MODE_NONE;
    };

    std::size_t CalculatePackedStride(const ShaderModule &_module);
    std::size_t CalculateAttributeStride(const AttributeType _attr);
}

#endif
