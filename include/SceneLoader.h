// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SceneLoader.h - Scene loader class header
// author: Karl-Mihkel Ott

#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#ifdef SCENE_LOADER_CPP
    #include <vector>
    #include <string>
    #include <fstream>
    #include <chrono>
    #include <unordered_map>
    #include <any>
    #include <cmath>
    #include <cstring>
#ifdef _DEBUG
    #include <iostream>
#endif

    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
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
    #include <ShaderDefinitions.h>
    #include <Window.h>
    #include <Renderer.h>
    #include <MeshLoader.h>
    #include <ModelUniforms.h>
    #include <AnimationSampler.h>
    #include <SkeletonDataManager.h>
    #include <NodeLoader.h>
#endif

namespace DENG {

    class DENG_API SceneLoader {
        private:
            Renderer &m_renderer;
            Libdas::DasParser &m_parser;
            const Libdas::DasScene &m_scene;
            std::vector<NodeLoader> m_node_loaders;
            std::vector<uint32_t> m_node_id_lookup;
            std::vector<Libdas::Matrix4<float>> m_node_root_transforms;
            std::vector<float*> m_morph_weights;
            std::vector<AnimationSampler*> m_scene_root_node_animation_samplers;
            static uint32_t m_scene_index;
            std::string m_scene_name = "Unnamed scene";

        public:
            SceneLoader(Renderer &_rend, Libdas::DasParser &_parser, const Libdas::DasScene &_scene, const std::vector<uint32_t> &_main_buffer_offsets, uint32_t _camera_offset, std::vector<Animation> &_animation_samplers, std::vector<std::string> &_texture_names);
            void Update();

            inline const std::string &GetName() const {
                return m_scene_name;
            }

            inline std::vector<NodeLoader> &GetNodes() {
                return m_node_loaders;
            }
    };
}

#endif
