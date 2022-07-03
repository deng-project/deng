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
#ifdef DENG_EDITOR
    #include <algorithm>
#endif
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
    #include <SkeletonLoader.h>
    #include <NodeLoader.h>
    #include <SceneLoader.h>
#endif

namespace DENG {

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
            const std::string &m_framebuffer_id;

        private:
            void _AttachBuffersAndTextures();

        public:
            ModelLoader(
                const std::string &_file_name, 
                Renderer &_rend, 
                uint32_t _camera_offset,
                const std::string &_framebuffer
            );
            ModelLoader(const ModelLoader &_ld) = delete;
            ModelLoader(ModelLoader&& _ld) noexcept;
            ~ModelLoader();

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

            inline Libdas::DasParser &GetParser() {
                return m_parser;
            }
    };
}

#endif
