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
    #include <libdas/include/Hash.h>

    #include <Api.h>
    #include <Window.h>
    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <Renderer.h>
    #include <GPUMemoryManager.h>
    #include <ModelUniforms.h>
    #include <MeshLoader.h>
    #include <ModelUniforms.h>
    #include <AnimationSampler.h>
    #include <ModelShaderGenerator.h>
    #include <ModelShaderManager.h>
    #include <MeshLoader.h>
    #include <SkeletonDataManager.h>
    #include <NodeLoader.h>
    #include <SceneLoader.h>
#endif

namespace DENG {

    struct ModelAnimation {
        ModelAnimation(const std::string &_name) : name(_name) {}
        ModelAnimation(const ModelAnimation &_ma) : 
            name(_ma.name),
            samplers(_ma.samplers),
            is_animated(_ma.is_animated) {}
        ModelAnimation(ModelAnimation &&_ma) :
            name(std::move(_ma.name)),
            samplers(std::move(_ma.samplers)),
            is_animated(_ma.is_animated) {}
 
        std::string name = "Unnamed animation";
        std::vector<AnimationSampler> samplers;
        bool is_animated = true;
    };

    class DENG_API ModelLoader {
        private:
            Libdas::DasParser m_parser;
            Renderer &m_renderer;
            std::vector<Animation> m_animations;
            std::vector<SceneLoader> m_scene_loaders;
            static uint32_t m_model_index;
            static uint32_t m_animation_index;
            std::string m_model_name = "Unnamed model";
            std::vector<std::string> m_texture_names;
            std::vector<uint32_t> m_buffer_offsets;

        private:
            void _AttachBuffersAndTextures();

        public:
            ModelLoader(const std::string &_file_name, Renderer &_rend, uint32_t _camera_offset);
            ModelLoader(ModelLoader&& _ld) noexcept;
            void Update();

            inline std::vector<std::string> &GetAttachedTextures() {
                return m_texture_names;
            }

            inline const std::string &GetName() {
                return m_model_name;
            }

            inline std::vector<SceneLoader> &GetScenes() {
                return m_scene_loaders;
            }

            inline std::vector<Animation> &GetAnimations() {
                return m_animations;
            }
    };
}

#endif
