// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SceneLoader.h - Scene loader class header
// author: Karl-Mihkel Ott

#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#ifdef SCENE_LOADER_CPP
    #include <vector>
    #include <array>
    #include <string>
    #include <fstream>
    #include <chrono>
    #include <unordered_map>
    #include <any>
    #include <cmath>
    #include <cstring>
    #include <algorithm>
#ifdef __DEBUG
    #include <iostream>
#endif

    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Quaternion.h"

    #include "mar/AsciiStreamReader.h"
    #include "mar/AsciiLineReader.h"

    #include "das/Api.h"
    #include "das/ErrorHandlers.h"
    #include "das/DasStructures.h"
    #include "das/DasReaderCore.h"
    #include "das/DasParser.h"

    #include "deng/Api.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/Window.h"
    #include "deng/Renderer.h"
    #include "deng/ModelUniforms.h"
    #include "deng/Entity.h"
    #include "deng/MeshLoader.h"
    #include "deng/AnimationSampler.h"
    #include "deng/SkeletonLoader.h"
    #include "deng/NodeLoader.h"
#endif

namespace DENG {

    class SceneLoader : public Entity {
        private:
            static uint32_t m_scene_index;
            std::vector<NodeLoader> m_root_node_loaders;

        private:
            // recursive parser setting method
            // meant for ModelLoader class's move constructor
            friend class ModelLoader;
            void _SetModel(Libdas::DasModel &_model);

        public:
            SceneLoader(
                Entity *_parent,
                Renderer &_rend,
                Libdas::DasModel &_model, 
                const Libdas::DasScene &_scene, 
                const std::vector<uint32_t> &_main_buffer_offsets, 
                uint32_t _camera_offset, 
                std::vector<Animation> &_animations,
                const std::string &_framebuffer_id
            );
            SceneLoader(const SceneLoader &_sl) = delete;
            SceneLoader(SceneLoader&& _sl) noexcept;
            void Update();

            inline std::vector<NodeLoader> &GetRootNodes() {
                return m_root_node_loaders;
            }
    };
}

#endif
