// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelShaderManager.h - DAS model shader management singleton class header
// author: Karl-Mihkel Ott

#ifndef MODEL_SHADER_MANAGER_H
#define MODEL_SHADER_MANAGER_H

#ifdef MODEL_SHADER_MANAGER_CPP
    #include <cstdint>
    #include <string>
    #include <cstring>
    #include <vector>
    #include <array>
    #include <fstream>
    #include <unordered_map>
    #include <cmath>
#ifdef _DEBUG
    #include <iostream>
#endif
    #include <any>

    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Quaternion.h>
    #include <libdas/include/Api.h>
    #include <libdas/include/DasStructures.h>
    #include <libdas/include/ErrorHandlers.h>
    #include <libdas/include/AsciiStreamReader.h>
    #include <libdas/include/AsciiLineReader.h>
    #include <libdas/include/DasReaderCore.h>
    #include <libdas/include/DasParser.h>

    #include <Api.h>
    #include <Window.h>
    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <Renderer.h>
    #include <MeshLoader.h>
    #include <ModelShaderGenerator.h>

    //static const std::string g_vertex_shader_files[] = {
        //"ModelShaders/PosUVNormalTang.vert",
        //"ModelShaders/PosUVNormal.vert",
        //"ModelShaders/PosUVTang.vert",
        //"ModelShaders/PosNormalTang.vert",
        //"ModelShaders/PosUV.vert",
        //"ModelShaders/PosNormal.vert",
        //"ModelShaders/PosTang.vert",
        //"ModelShaders/Pos.vert"
    //};

    //#define UV_FRAGMENT_SHADER  "ModelShaders/UVMapped.frag"
    //#define COLORED_SHADER      "ModelShaders/Colored.frag"

    #define POS_UV_NORMAL_TANG  0
    #define POS_UV_NORMAL       1
    #define POS_UV_TANG         2
    #define POS_NORMAL_TANG     3
    
    #define POS_UV              4
    #define POS_NORMAL          5
    #define POS_TANG            6
    
    #define POS                 7
#endif

#define POS_ATTR_MASK       0x01
#define UV_ATTR_MASK        0x02
#define NORMAL_ATTR_MASK    0x04
#define TANG_ATTR_MASK      0x08

// uniform across all shaders
#define MODEL_SHADER_SAMPLER_BINDING            3
#define MODEL_SHADER_ANIMATION_UBO_BINDING      0
#define MODEL_SHADER_CAMERA_UBO_BINDING         1
#define MODEL_SHADER_MODEL_UBO_BINDING          2


#ifndef DENG_DEFS_ONLY
namespace DENG {

    struct ModelCameraUbo {
        Libdas::Matrix4<float> projection_matrix;
        Libdas::Matrix4<float> view_matrix;
    };

    struct ModelAnimationUbo {
        // animated properties
        Libdas::Quaternion rotation[2];
        Libdas::Vector4<float> weights[2];
        Libdas::Vector4<float> translation[2];
        float scales[2];

        uint32_t target_mask = 0; // see AnimationSampler.h
        uint32_t interpolation_mode = 0; // see DasStructures.h
        uint32_t animate = 0;

        // timestamp data in seconds
        float timestamps[2] = {};
        float current_time = 0;
        uint32_t used_weights = 0;
    };

    struct ModelUbo {
        Libdas::Matrix4<float> node_transform;
        Libdas::Matrix4<float> skeleton_transform;
        Libdas::Vector4<float> color;
    };

    class ModelShaderManager {
        private:
            static std::array<uint32_t, 8> m_shader_ids;
            static uint32_t m_used_shaders;
            static bool m_set_stage;

        private:
            static void _GetAttributesByType(const Libdas::DasParser &_parser, const Libdas::DasMeshPrimitive &_prim, uint32_t _id, ShaderModule &_module, uint32_t _base_offset);
            static void _GetUniforms(ShaderModule &_module, const uint32_t _camera_ubo_offset, const uint32_t _id);
            static uint32_t _ResolveMask(uint32_t _mask);

        public:
            static uint32_t RequestShaderModule(Renderer &_rend, const Libdas::DasParser &_parser, const Libdas::DasMeshPrimitive &_mesh, 
                                                const uint32_t _base_offset, const uint32_t _camera_ubo_offset);
    };
}
#endif

#endif
