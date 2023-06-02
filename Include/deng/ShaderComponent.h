// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ShaderDefinitions.h - Shader module definitions header
// author: Karl-Mihkel Ott


#ifndef SHADER_COMPONENT_H
#define SHADER_COMPONENT_H

#ifdef SHADER_COMPONENT_CPP
    #include <cstdint>
    #include <vector>
    #include <string>
    #include <variant>
    #include <unordered_map>

    #include "trs/Vector.h"
    #include "trs/Quaternion.h"
    #include "trs/Points.h"
    #include "trs/Matrix.h"

    #include "das/DasStructures.h"

    #include "deng/Api.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ProgramFilesManager.h"
#endif

#include "deng/Shader.h"

// about 16MB of memory
#define DEFAULT_BUFFER_SIZE            (1 << 24)

// about 4MB of memory
// this should be enough memory to hold one 1k texture
#define DEFAULT_STAGING_BUFFER_SIZE    (1 << 22) 


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
        UNIFORM_DATA_TYPE_PUSH_CONSTANT,
        UNIFORM_DATA_TYPE_STORAGE_BUFFER,
        UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER,
        UNIFORM_DATA_TYPE_3D_IMAGE_SAMPLER
    };

    enum UniformUsage {
        UNIFORM_USAGE_PER_SHADER,
        UNIFORM_USAGE_PER_MESH
    };

    enum CullMode {
        CULL_MODE_NONE,
        CULL_MODE_COUNTER_CLOCKWISE,
        CULL_MODE_CLOCKWISE
    };


    enum PrimitiveMode {
        PRIMITIVE_MODE_POINTS,
        PRIMITIVE_MODE_LINES,
        PRIMITIVE_MODE_TRIANGLES
    };


    typedef uint8_t ShaderStage;
    #define SHADER_STAGE_NULL       0x00
    #define SHADER_STAGE_VERTEX     0x01
    #define SHADER_STAGE_GEOMETRY   0x02
    #define SHADER_STAGE_FRAGMENT   0x04

    struct StorageBlock {
        uint32_t uBinding = 0;
        uint32_t uSize = 0;
        uint32_t uOffset = 0;
    };

    // Structure to be passed into the renderer
    struct UniformDataLayout {
        StorageBlock block = {};
        UniformDataType eType = UNIFORM_DATA_TYPE_BUFFER;
        UniformUsage eUsage = UNIFORM_USAGE_PER_SHADER;
        ShaderStage iStage = SHADER_STAGE_NULL;
        uint32_t uPurposeBits = 0; // purpose bits are only useful in scene framework
    };

    struct Viewport {
        uint32_t uX = 0;
        uint32_t uY = 0;
        uint32_t uWidth = 0;
        uint32_t uHeight = 0;
    };


    // this structure defines shaders that should be used in a pipeline
    // NOTE: geometry shaders are completely optional and not required
    struct ShaderComponent {
        ShaderComponent() = default;
        ShaderComponent(const ShaderComponent&) = default;

        std::vector<AttributeType> attributes;
        std::vector<std::size_t> attributeStrides;
        std::vector<UniformDataLayout> uboDataLayouts;
        Shader* pShader = nullptr;
        uint32_t uShaderComponentId = 0;
        Viewport viewport;
        CullMode eCullMode = CULL_MODE_NONE;
        PrimitiveMode ePrimitiveMode = PRIMITIVE_MODE_TRIANGLES;
        
        uint32_t uPushConstantDataLength = 0;
        ShaderStage iPushConstantShaderStage = 0;
        const void* pPushConstantData = nullptr;
        bool bEnablePushConstants = false;
        
        bool bEnableCustomViewport = false;
        bool bEnable2DTextures = false;
        bool bEnable3DTextures = false;
        bool bEnableScissor = false;
        bool bEnableDepthTesting = false;
        bool bEnableStencilTesting = false;
        bool bEnableBlend = false;
        bool bEnableIndexing = true;
    };
}

#endif
