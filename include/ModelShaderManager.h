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
    #include <ModelUniforms.h>

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
