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
    #include <array>
    #include <unordered_map>
    #include <chrono>
    #include <variant>
    #include <stack>
    #include <any>
#ifdef DENG_EDITOR
    #include <algorithm>
#endif
#ifdef __DEBUG
    #include <iostream>
#endif

    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Points.h"
    #include "trs/Quaternion.h"

    #include "mar/AsciiStreamReader.h"
    #include "mar/AsciiLineReader.h"

    #include "das/Api.h"
    #include "das/ErrorHandlers.h"
    #include "das/DasStructures.h"
    #include "das/DasReaderCore.h"
    #include "das/DasParser.h"
    #include "das/stb_image.h"
    #include "das/TextureReader.h"
    #include "das/Hash.h"

    #include "deng/Api.h"
    #include "deng/BaseTypes.h"
    #include "deng/Window.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Renderer.h"
    #include "deng/GPUMemoryManager.h"
    #include "deng/ModelUniforms.h"
    #include "deng/ModelUniforms.h"
    #include "deng/Entity.h"
    #include "deng/ScriptableEntity.h"
    #include "deng/Camera3D.h"
    #include "deng/MeshLoader.h"
    #include "deng/AnimationSampler.h"
    #include "deng/ModelShaderGenerator.h"
    #include "deng/ModelShaderManager.h"
    #include "deng/MeshLoader.h"
    #include "deng/SkeletonLoader.h"
    #include "deng/NodeLoader.h"
    #include "deng/SceneLoader.h"
    #include "deng/Registry.h"
#endif

namespace DENG {

    class DENG_API ModelLoader : public ScriptableEntity {
        private:
            Libdas::DasParser m_parser;
            Renderer &m_renderer;
            std::vector<Animation> m_animations;
            std::vector<SceneLoader> m_scene_loaders;
            static uint32_t m_model_index;
            static uint32_t m_animation_index;
            std::vector<std::string> m_texture_names;
            std::vector<uint32_t> m_buffer_offsets;
            uint32_t m_camera_id;
            const std::string &m_framebuffer_id;

        private:
            void _AttachBuffersAndTextures();

        public:
            ModelLoader(
                Entity *_parent,
                const std::string &_file_name, 
                Renderer &_rend,
                uint32_t _camera_id,
                const std::string &_framebuffer
            );
            ModelLoader(const ModelLoader &_ld) = delete;
            ModelLoader(ModelLoader&& _ld) noexcept;
            ~ModelLoader();

            void Attach();
            void Update();

            inline std::vector<std::string> &GetAttachedTextures() {
                return m_texture_names;
            }

            inline std::vector<SceneLoader> &GetScenes() {
                return m_scene_loaders;
            }

            inline std::vector<Animation> &GetAnimations() {
                return m_animations;
            }

            inline Libdas::DasParser &GetParser() {
                return m_parser;
            }
    };
}

#endif
