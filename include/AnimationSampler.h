// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: AnimationSampler.h - AnimationSampler class header
// author: Karl-Mihkel Ott

#ifndef ANIMATION_SAMPLER_H
#define ANIMATION_SAMPLER_H

#ifdef ANIMATION_SAMPLER_CPP
    #include <cstdint>
    #include <cmath>
    #include <cstring>
    #include <chrono>
    #include <string>
    #include <fstream>
    #include <vector>
    #include <unordered_map>
    #include <variant>
    #include <iostream>
    #include <any>

    #include <libdas/include/Points.h>
    #include <libdas/include/Quaternion.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Api.h>
    #include <libdas/include/DasStructures.h>
    #include <libdas/include/ErrorHandlers.h>
    #include <libdas/include/AsciiStreamReader.h>
    #include <libdas/include/AsciiLineReader.h>
    #include <libdas/include/DasReaderCore.h>
    #include <libdas/include/DasParser.h>

    #include <Api.h>
    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <Window.h>
    #include <Renderer.h>
    #include <Interpolation.h>
#endif

#define TARGET_MASK_WEIGHT          0x00000001
#define TARGET_MASK_ROTATION        0x00000002
#define TARGET_MASK_TRANSLATION     0x00000004
#define TARGET_MASK_SCALE           0x00000008

namespace DENG {

    struct AnimationUniformObject {
        Libdas::Quaternion rotation;
        Libdas::Vector4<float> weight_targets;
        Libdas::Vector4<float> translation_target;
        float uniform_scale = 0.0f;
        uint32_t target_mask = 0;
    };


    class DENG_API AnimationSampler {
        private:
            AnimationUniformObject m_animation_ubo = {};
            Libdas::DasAnimationChannel &m_channel;
            std::chrono::time_point<std::chrono::system_clock> m_beg_time = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock> m_active_time = std::chrono::system_clock::now();
            std::vector<float> m_timestamps;
            uint32_t m_weight_target_count = 0;

            // weight, translation, rotation and scale
            std::vector<std::variant<uint32_t*, Libdas::Vector3<float>*, Libdas::Quaternion*, float>> m_interp_values;
            uint32_t m_active_timestamp_index = 0;
            const uint32_t m_shader_id;
            const uint32_t m_ubo_id;
            bool m_repeat = false;

        private:
            void _InterpolateWeights(DENG::Renderer &_renderer, float _delta_time);
            void _InterpolateTranslation(DENG::Renderer &_renderer, float _delta_time);
            void _InterpolateRotation(DENG::Renderer &_renderer, float _delta_time);
            void _InterpolateScale(DENG::Renderer &_renderer, float _delta_time);

        public:
            AnimationSampler(Libdas::DasAnimationChannel &_channel, Libdas::DasParser &_parser, uint32_t _shader_id, uint32_t _ubo_id);

            void Update(DENG::Renderer &_renderer);

            inline void Animate(bool _repeat) {
                m_repeat = _repeat;
                m_active_timestamp_index = 0;
                m_beg_time = std::chrono::system_clock::now();
            }

            inline void Stop() {
                m_active_timestamp_index = UINT32_MAX;
            }

            inline AnimationUniformObject &GetUniformObject() {
                return m_animation_ubo;
            }

            inline uint32_t GetUsedUboSize() {
                return static_cast<size_t>(sizeof(AnimationUniformObject));
            }
    };
}

#endif
