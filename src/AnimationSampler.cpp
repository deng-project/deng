// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: AnimationSampler.h - AnimationSampler class implementation
// author: Karl-Mihkel Ott

#define ANIMATION_SAMPLER_CPP
#include <AnimationSampler.h>

namespace DENG {

    uint32_t AnimationSampler::m_animation_sampler_index = 0;

    AnimationSampler::AnimationSampler(const Libdas::DasAnimationChannel &_channel, Libdas::DasParser &_parser) : 
        m_channel(_channel), 
        m_parser(_parser) 
    {}


    void AnimationSampler::_LinearInterpolation(float _t1, float _tc, float _t2) {
        const size_t curr = static_cast<size_t>(m_active_timestamp_index);
        const size_t next = static_cast<size_t>((m_active_timestamp_index + 1) % m_channel.keyframe_count);
        const float t = (_tc - _t1) / (_t2 - _t1);

        switch(m_channel.target) {
            // weights
            case LIBDAS_ANIMATION_TARGET_WEIGHTS:
                { 
                    DENG_ASSERT(m_channel.weight_count <= MAX_MORPH_TARGETS);
                    const size_t size = static_cast<size_t>(m_channel.weight_count) * sizeof(float);
                    float *w1 = reinterpret_cast<float*>(m_channel.target_values + curr * size);
                    float *w2 = reinterpret_cast<float*>(m_channel.target_values + next * size);

                    for(uint32_t i = 0; i < m_channel.weight_count; i++)
                        m_morph_weights[i] = w1[i] * (1 - t) + w2[i] * t;
                }
                break;

            // translation
            case LIBDAS_ANIMATION_TARGET_TRANSLATION:
                {
                    const size_t size = sizeof(Libdas::Vector3<float>);
                    Libdas::Vector3<float> *tr1 = reinterpret_cast<Libdas::Vector3<float>*>(m_channel.target_values + curr * size);
                    Libdas::Vector3<float> *tr2 = reinterpret_cast<Libdas::Vector3<float>*>(m_channel.target_values + next * size);

                    m_translation = (*tr1) * (1 - t) + (*tr2) * t;
                }
                break;

            // rotation
            case LIBDAS_ANIMATION_TARGET_ROTATION:
                {
                    const size_t size = sizeof(Libdas::Quaternion);
                    Libdas::Quaternion *q1 = reinterpret_cast<Libdas::Quaternion*>(m_channel.target_values + curr * size);
                    Libdas::Quaternion *q2 = reinterpret_cast<Libdas::Quaternion*>(m_channel.target_values + next * size);
                    const float dot = Libdas::Quaternion::Dot(*q1, *q2);

                    const float a = acosf(std::abs(dot));
                    const float s = dot / std::abs(dot);

                    if(a > ZERO_MARGIN) {
                        const float k1 = sinf(a * (1 - t)) / sinf(a);
                        const float k2 = s * sinf(a * t) / sinf(a);
                        m_rotation = *q1 * k1 + *q2 * k2;
                    } else {
                        m_rotation = *q1 * (1 - t) + *q2 * t; 
                    }

                    m_rotation.x = -m_rotation.x;
                    m_rotation.y = -m_rotation.y;
                    m_rotation.z = -m_rotation.z;
                    break;
                }

            // uniform scale
            case LIBDAS_ANIMATION_TARGET_SCALE:
                {
                    const size_t size = sizeof(float);
                    float s1 = *reinterpret_cast<float*>(m_channel.target_values + curr * size);
                    float s2 = *reinterpret_cast<float*>(m_channel.target_values + next * size);

                    m_scale = (1 - t) * s1 + s2 * t;
                }
                break;
        }
    }


    void AnimationSampler::_StepInterpolation() {
        const size_t curr = static_cast<size_t>(m_active_timestamp_index);

        switch(m_channel.target) {
            // weight
            case LIBDAS_ANIMATION_TARGET_WEIGHTS:
                {
                    const size_t size = static_cast<size_t>(m_channel.weight_count) * sizeof(float);
                    float *w = reinterpret_cast<float*>(m_channel.target_values + curr * size);

                    for(uint32_t i = 0; i < m_channel.weight_count; i++)
                        m_morph_weights[i] = w[i];
                }
                break;

            // translation
            case LIBDAS_ANIMATION_TARGET_TRANSLATION:
                {
                    const size_t size = sizeof(Libdas::Vector3<float>);
                    m_translation = *reinterpret_cast<Libdas::Vector3<float>*>(m_channel.target_values + curr * size);
                }
                break;

            // rotation
            case LIBDAS_ANIMATION_TARGET_ROTATION:
                {
                    const size_t size = sizeof(Libdas::Quaternion);
                    m_rotation = *reinterpret_cast<Libdas::Quaternion*>(m_channel.target_values + curr * size);
                }
                break;

            // uniform scale
            case LIBDAS_ANIMATION_TARGET_SCALE:
                {
                    const size_t size = sizeof(float);
                    m_scale = *reinterpret_cast<float*>(m_channel.target_values + curr * size);
                }
                break;
        }
    }


    void AnimationSampler::_CubicSplineInterpolation(float _t1, float _tc, float _t2) {
        const float td = _t2 - _t1;
        const float t = (_tc - _t1) / td;
        const size_t curr = static_cast<size_t>(m_active_timestamp_index);
        const size_t next = static_cast<size_t>((m_active_timestamp_index + 1) % m_channel.keyframe_count);

        switch(m_channel.target) {
            // weights
            case LIBDAS_ANIMATION_TARGET_WEIGHTS:
                {
                    const size_t size = static_cast<size_t>(m_channel.weight_count) * sizeof(float);
                    float *v1 = reinterpret_cast<float*>(m_channel.target_values + curr * size);
                    float *v2 = reinterpret_cast<float*>(m_channel.target_values + next * size);

                    // in and out tangents
                    float *a2 = reinterpret_cast<float*>(m_channel.tangents + next * 2 * size);
                    float *b1 = reinterpret_cast<float*>(m_channel.tangents + (curr * 2 + 1) * size);

                    for(uint32_t i = 0; i < m_channel.weight_count; i++)
                        m_morph_weights[i] = (2 * CUBE(t) - 3 * SQR(t) + 1) * v1[i] + td * (CUBE(t) - 2 * SQR(t) + 1) * b1[i] + (-2 * CUBE(t) + 3 * SQR(t)) * v2[i] + td * (CUBE(t) - SQR(t)) * a2[i];
                }
                break;

            // translation
            case LIBDAS_ANIMATION_TARGET_TRANSLATION:
                {
                    const size_t size = sizeof(Libdas::Vector3<float>);
                    Libdas::Vector3<float> *v1 = reinterpret_cast<Libdas::Vector3<float>*>(m_channel.target_values + curr * size);
                    Libdas::Vector3<float> *v2 = reinterpret_cast<Libdas::Vector3<float>*>(m_channel.target_values + next * size);

                    Libdas::Vector3<float> *a2 = reinterpret_cast<Libdas::Vector3<float>*>(m_channel.tangents + next * 2 * size);
                    Libdas::Vector3<float> *b1 = reinterpret_cast<Libdas::Vector3<float>*>(m_channel.tangents + (curr * 2 + 1) * size);

                    m_translation = (*v1) * (2 * CUBE(t) - 3 * SQR(t) + 1) + (*b1) * td * (CUBE(t) - 2 * SQR(t) + 1) + (*v2) * (-2 * CUBE(t) + 3 * SQR(t)) + (*a2) * td * (CUBE(t) - SQR(t));
                }
                break;

            // rotation
            case LIBDAS_ANIMATION_TARGET_ROTATION:
                {
                    const size_t size = sizeof(Libdas::Quaternion);
                    Libdas::Quaternion *v1 = reinterpret_cast<Libdas::Quaternion*>(m_channel.target_values + curr * size);
                    Libdas::Quaternion *v2 = reinterpret_cast<Libdas::Quaternion*>(m_channel.target_values + next * size);

                    Libdas::Quaternion *a2 = reinterpret_cast<Libdas::Quaternion*>(m_channel.tangents + next * 2 * size);
                    Libdas::Quaternion *b1 = reinterpret_cast<Libdas::Quaternion*>(m_channel.tangents + (curr * 2 + 1) * size);

                    m_rotation = (*v1) * (2 * CUBE(t) - 3 * SQR(t) + 1) + (*b1) * td * (CUBE(t) - 2 * SQR(t) + 1) + (*v2) * (-2 * CUBE(t) + 3 * SQR(t)) + (*a2) * td * (CUBE(t) - SQR(t));
                }
                break;

            case LIBDAS_ANIMATION_TARGET_SCALE:
                {
                    const size_t size = sizeof(float);
                    float v1 = *reinterpret_cast<float*>(m_channel.target_values + curr * size);
                    float v2 = *reinterpret_cast<float*>(m_channel.target_values + next * size);

                    float a2 = *reinterpret_cast<float*>(m_channel.tangents + next * 2 * size);
                    float b1 = *reinterpret_cast<float*>(m_channel.tangents + (curr * 2 + 1) * size);

                    m_scale = v1 * (2 * CUBE(t) - 3 * SQR(t) + 1) + b1 * td * (CUBE(t) - 2 * SQR(t) + 1) + v2 * (-2 * CUBE(t) + 3 * SQR(t)) + a2 * td * (CUBE(t) - SQR(t));
                }
                break;

            default:
                DENG_ASSERT(false);
                break;
        }
    }


    void AnimationSampler::Update() {
        std::memset(m_morph_weights, 0, sizeof(float[MAX_MORPH_TARGETS]));

        // calculate delta time
        if(m_animate) m_active_time = std::chrono::system_clock::now();
        std::chrono::duration<float, std::milli> delta_time = m_active_time - m_beg_time;
        const float kf = delta_time.count() / 1000 + m_cached_delta_time;
        const uint32_t next = (m_active_timestamp_index + 1) % m_channel.keyframe_count;

        // check the current timestamp against keyframe values
        if(kf >= m_channel.keyframes[next]) {
            m_active_timestamp_index++;
            if(m_active_timestamp_index >= m_channel.keyframe_count) {
                m_active_timestamp_index = 0;
                m_beg_time = std::chrono::system_clock::now();
                m_cached_delta_time = 0;
                if(!m_repeat) m_animate = false;
            }
        }

        // set correct interpolation values
        float t1 = m_channel.keyframes[m_active_timestamp_index];
        float t2 = m_channel.keyframes[next];
        switch(m_channel.interpolation) {
            case LIBDAS_INTERPOLATION_VALUE_LINEAR:
                _LinearInterpolation(t1, kf, t2);
                break;

            case LIBDAS_INTERPOLATION_VALUE_CUBICSPLINE:
                _CubicSplineInterpolation(t1, kf, t2);
                break;

            case LIBDAS_INTERPOLATION_VALUE_STEP:
                _StepInterpolation();
                break;

            default:
                break;
        }
    }
}
