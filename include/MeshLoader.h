// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: MeshLoader.h - DAS data MeshLoader class header
// author: Karl-Mihkel Ott

#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#ifdef MESH_LOADER_CPP
    #include <any>
    #include <vector>
    #include <string>
    #include <fstream>
#ifdef _DEBUG
    #include <iostream>
#endif
    #include <cstdint>
    #include <cstring>
    #include <cmath>
    #include <unordered_map>
    #include <vulkan/vulkan.h>

    #include <libdas/include/Vector.h>
    #include <libdas/include/Points.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Quaternion.h>
    #include <libdas/include/Api.h>
    #include <libdas/include/ErrorHandlers.h>
    #include <libdas/include/AsciiStreamReader.h>
    #include <libdas/include/AsciiLineReader.h>
    #include <libdas/include/DasStructures.h>
    #include <libdas/include/DasReaderCore.h>
    #include <libdas/include/DasParser.h>

    #include <Api.h>
    #include <Window.h>
    #include <ShaderDefinitions.h>
    #include <Renderer.h>
    #include <ModelShaderManager.h>
#endif

namespace DENG {

    class MeshLoader {
        private:
            uint32_t m_mesh_ref_id = UINT32_MAX;
            Libdas::DasParser &m_parser;
            const Libdas::DasMesh &m_mesh;
            Renderer &m_renderer;
            const uint32_t m_shader_id;
            const uint32_t m_ubo_offset;

        public:
            MeshLoader(const Libdas::DasMesh &_mesh, Libdas::DasParser &_parser, Renderer &_renderer, uint32_t _shader_id, uint32_t _base_ubo_offset);
            void Attach();
            void UseTexture(const std::string &_name);

            inline uint32_t GetModelUboOffset() {
                return m_ubo_offset;
            }

            inline uint32_t &GetMeshReferenceId() {
                return m_mesh_ref_id;
            }

            inline uint32_t GetShaderId() {
                return m_shader_id;
            }

            inline const std::string &GetName() {
                return m_mesh.name;
            }

            static uint32_t CalculateAbsoluteOffset(const Libdas::DasParser &_parser, uint32_t _buffer_id, uint32_t _buffer_offset);

    };
}

#endif

