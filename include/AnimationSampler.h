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

    #include <libdas/include/Iterators.h>
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
    #include <libdas/include/Hash.h>

    #include <Api.h>
    #include <BaseTypes.h>
    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <Window.h>
    #include <Renderer.h>
    #include <ModelUniforms.h>
    #include <ModelShaderGenerator.h>
    #include <ModelShaderManager.h>
    
    #define ZERO_MARGIN 0.0001f
#endif

#define TARGET_MASK_WEIGHT          0x00000001
#define TARGET_MASK_ROTATION        0x00000002
#define TARGET_MASK_TRANSLATION     0x00000004
#define TARGET_MASK_SCALE           0x00000008

namespace DENG {

    class DENG_API AnimationSampler {
        private:
            float m_morph_weights[MAX_MORPH_TARGETS] = {};
            const Libdas::DasAnimationChannel &m_channel;
            Libdas::DasParser &m_parser;
            const std::vector<uint32_t> m_ubo_offsets;
            std::chrono::time_point<std::chrono::system_clock> m_beg_time = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock> m_active_time = std::chrono::system_clock::now();

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
            Libdas::Vector3<float> m_translation = { 0.0f, 0.0f, 0.0f };
            Libdas::Quaternion m_rotation = { 0.0f, 0.0f, 0.0f, 1.0f }; // note matrices derived from quaternion rotations need to be transposed !
            float m_scale = 1.0f;

        private:
            void _LinearInterpolation(float _t1, float _tc, float _t2);
            void _StepInterpolation();
            void _CubicSplineInterpolation(float _t1, float _tc, float _t2);

        public:
            AnimationSampler(const Libdas::DasAnimationChannel &_channel, Libdas::DasParser &_parser);

            void Update();

            inline void Animate(bool _repeat) {
                m_animate = true;
                m_repeat = _repeat;
                m_beg_time = std::chrono::system_clock::now();
                m_active_time = std::chrono::system_clock::now();
            }

            inline void Stop() {
                m_animate = false;
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

            inline Libdas::Vector3<float> GetTranslation() {
                return m_translation;
            }

            inline Libdas::Quaternion GetRotation() {
                return m_rotation;
            }

            inline float GetScale() {
                return m_scale;
            }

            inline float *GetMorphWeights() {
                return m_morph_weights;
            }
    };

    typedef std::pair<std::string, std::vector<AnimationSampler>> Animation;

}

#endif
