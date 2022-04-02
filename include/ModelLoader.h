// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelLoader.h - DAS model loader class header
// author: Karl-Mihkel Ott

#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#ifdef MODEL_LOADER_CPP
    #include <fstream>
    #include <string>
    #include <cstring>
    #include <cmath>
    #include <vector>
    #include <unordered_map>
    #include <chrono>
    #include <variant>
    #include <stack>
    #include <any>
#ifdef _DEBUG
    #include <iostream>
#endif

    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Points.h>
    #include <libdas/include/Quaternion.h>
    #include <libdas/include/Api.h>
    #include <libdas/include/AsciiStreamReader.h>
    #include <libdas/include/AsciiLineReader.h>
    #include <libdas/include/ErrorHandlers.h>
    #include <libdas/include/DasStructures.h>
    #include <libdas/include/DasReaderCore.h>
    #include <libdas/include/DasParser.h>
    #include <libdas/include/stb_image.h>
    #include <libdas/include/TextureReader.h>

    #include <Api.h>
    #include <Window.h>
    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <Renderer.h>
    #include <MeshLoader.h>
    #include <ModelUniforms.h>
    #include <ModelShaderManager.h>
    #include <AnimationSampler.h>
#endif

namespace DENG {

    struct ModelAnimation {
        ModelAnimation(const std::string &_name) : name(_name) {}

        const std::string &name;
        std::vector<AnimationSampler> samplers;
    };

    class DENG_API ModelLoader {
        private:
            Libdas::DasParser m_parser;
            Renderer &m_renderer;
            std::vector<uint32_t> m_mesh_ubo_offsets;
            std::vector<MeshLoader> m_mesh_loaders;
            std::vector<ModelAnimation> m_animations;
            std::vector<std::string> m_texture_names;
            uint32_t m_texture_bookmark = 0;
            const uint32_t m_base_buffer_offset;
            const uint32_t m_base_ubo_offset;

        private:
            void _AttachBuffersAndTextures();
            void _AttachNodeAndSkeletonTransforms(uint32_t _scene_id);
            void _FindSceneRootNodes(uint32_t _scene_id);
            void _CheckMeshPrimitives(const Libdas::DasMesh &_mesh); // mesh primitives should all have the same vertex attributes
            std::vector<uint32_t> _GetMeshUboOffsetsFromNodeId(uint32_t _node_id);

        public:
            ModelLoader(const std::string &_file_name, Renderer &_rend, uint32_t _base_buffer_offset, uint32_t _base_ubo_offset);
            void Attach();
            void Update();

            inline std::vector<std::string> &GetAttachedTextures() {
                return m_texture_names;
            }
    };
}

#endif
