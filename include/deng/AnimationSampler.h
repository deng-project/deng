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
    #include <array>
    #include <unordered_map>
    #include <variant>
    #include <iostream>
    #include <any>

    #include "mar/AsciiStreamReader.h"
    #include "mar/AsciiLineReader.h"

    #include "trs/Iterators.h"
    #include "trs/Points.h"
    #include "trs/Vector.h"
    #include "trs/Matrix.h"
    #include "trs/Quaternion.h"

    #include "das/Api.h"
    #include "das/DasStructures.h"
    #include "das/ErrorHandlers.h"
    #include "das/DasReaderCore.h"
    #include "das/DasParser.h"
    #include "das/Hash.h"

    #include "deng/Api.h"
    #include "deng/BaseTypes.h"
    #include "deng/ErrorDefinitions.h"
    #include "deng/ShaderDefinitions.h"
    #include "deng/Window.h"
    #include "deng/Renderer.h"
    #include "deng/ModelUniforms.h"
    #include "deng/ModelShaderGenerator.h"
    #include "deng/ModelShaderManager.h"
    #include "deng/Entity.h"
    
    #define ZERO_MARGIN 0.000001f
#endif

#define TARGET_MASK_WEIGHT          0x00000001
#define TARGET_MASK_ROTATION        0x00000002
#define TARGET_MASK_TRANSLATION     0x00000004
#define TARGET_MASK_SCALE           0x00000008

namespace DENG {

    class DENG_API AnimationSampler : public Entity {
        private:
            std::array<float, MAX_MORPH_TARGETS> m_morph_weights = {};
            const Libdas::DasAnimationChannel &m_channel;
            const std::vector<uint32_t> m_ubo_offsets;
            std::chrono::time_point<std::chrono::high_resolution_clock> m_beg_time = std::chrono::high_resolution_clock::now();
            std::chrono::time_point<std::chrono::high_resolution_clock> m_active_time = std::chrono::high_resolution_clock::now();

            // needs some fixes later
            const uint32_t m_weight_target_count = 1;

            // weight, translation, rotation and scale
            uint32_t m_active_timestamp_index = 0;

            // boolean config values
            bool m_animate = false;
            bool m_repeat = false;

            static uint32_t m_animation_sampler_index;
            float m_cached_delta_time = 0;

            // animated properties
            TRS::Vector3<float> m_translation = { 0.0f, 0.0f, 0.0f };
            TRS::Quaternion m_rotation = { 0.0f, 0.0f, 0.0f, 1.0f }; // note matrices derived from quaternion rotations need to be transposed !
            float m_scale = 1.0f;

        private:
            void _LinearInterpolation(float _t1, float _tc, float _t2);
            void _StepInterpolation();
            void _CubicSplineInterpolation(float _t1, float _tc, float _t2);

        public:
            AnimationSampler(
                Entity *_parent,
                const std::string &_name,
                const Libdas::DasAnimationChannel &_channel
            );
            AnimationSampler(const AnimationSampler& _as) = delete;
            AnimationSampler(AnimationSampler&& _as) noexcept;

            void Update();

            inline void Animate(bool _repeat) {
                m_animate = true;
                m_repeat = _repeat;
                m_beg_time = std::chrono::high_resolution_clock::now();
                m_active_time = std::chrono::high_resolution_clock::now();
            }

            inline void SetRepeat(bool _repeat) {
                m_repeat = _repeat;
            }

            inline void Stop() {
                m_animate = false;
                m_active_time = std::chrono::high_resolution_clock::now();
                std::chrono::duration<float, std::milli> delta_time = m_active_time - m_beg_time;
                m_cached_delta_time += delta_time.count() / 1000.0f;
            }

            inline AnimationTarget GetAnimationTarget() {
                return m_channel.target;
            }

            inline bool GetAnimationStatus() const {
                return m_animate;
            }

            inline const Libdas::DasAnimationChannel &GetAnimationChannel() {
                return m_channel;
            }

            inline TRS::Vector3<float> GetTranslation() {
                return m_translation;
            }

            inline TRS::Quaternion GetRotation() {
                return m_rotation;
            }

            inline float GetScale() {
                return m_scale;
            }

            inline std::array<float, MAX_MORPH_TARGETS> &GetMorphWeights() {
                return m_morph_weights;
            }
    };

    struct Animation {
        std::string name;
        std::vector<AnimationSampler> samplers;
        bool is_unbound = false;
        bool is_animated = false;
        bool is_repeated = false;
    };

}

#endif
