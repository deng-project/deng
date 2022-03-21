// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: AnimationSampler.h - AnimationSampler class implementation
// author: Karl-Mihkel Ott

#define ANIMATION_SAMPLER_CPP
#include <AnimationSampler.h>

namespace DENG {

    AnimationSampler::AnimationSampler(Libdas::DasAnimationChannel &_channel, Libdas::DasParser &_parser, uint32_t _shader_id, uint32_t _ubo_id) : 
        m_channel(_channel), m_shader_id(_shader_id), m_ubo_id(_ubo_id) 
    {
        // select appropriate target
        switch(_channel.target) {
            case LIBDAS_ANIMATION_TARGET_WEIGHTS:
                m_animation_ubo.target_mask = TARGET_MASK_WEIGHT;
                break;

            case LIBDAS_ANIMATION_TARGET_TRANSLATION:
                m_animation_ubo.target_mask = TARGET_MASK_TRANSLATION;
                break;

            case LIBDAS_ANIMATION_TARGET_ROTATION:
                m_animation_ubo.target_mask = TARGET_MASK_ROTATION;
                break;

            case LIBDAS_ANIMATION_TARGET_SCALE:
                m_animation_ubo.target_mask = TARGET_MASK_SCALE;
                break;

            default:
                break;
        }

        // write targets from buffer to target vector
        size_t current_buffer_offset = 0;
        size_t relative_offset = 0;
        Libdas::DasBuffer &buffer = _parser.AccessBuffer(m_channel.target_value_buffer_id);

        for(auto buffer_it = buffer.data_ptrs.begin(); buffer_it != buffer.data_ptrs.end(); buffer_it++) {
            if(current_buffer_offset + buffer_it->second < m_channel.target_value_buffer_offset) {
                current_buffer_offset += buffer_it->second;
                continue;
            } 

            relative_offset = static_cast<size_t>(m_channel.target_value_buffer_offset) - current_buffer_offset;
            const char *data_ptr = buffer.data_ptrs[relative_offset].first;

            for(uint32_t i = 0; i < m_channel.keyframe_count; i++) {
                switch(m_channel.target) {
                    case LIBDAS_ANIMATION_TARGET_WEIGHTS:
                        std::cerr << "Morph targets not yet implemented" << std::endl;
                        DENG_ASSERT(false);
                        break;

                    case LIBDAS_ANIMATION_TARGET_TRANSLATION:
                        m_interp_values.push_back(reinterpret_cast<Libdas::Vector3<float>*>(data_ptr[relative_offset]));
                        relative_offset += static_cast<uint32_t>(sizeof(Libdas::Vector3<float>));
                        break;

                    case LIBDAS_ANIMATION_TARGET_ROTATION:
                        m_interp_values.push_back(reinterpret_cast<Libdas::Quaternion*>(data_ptr[relative_offset]));
                        relative_offset += static_cast<uint32_t>(sizeof(Libdas::Quaternion));
                        break;

                    case LIBDAS_ANIMATION_TARGET_SCALE:
                        m_interp_values.push_back(*reinterpret_cast<float*>(data_ptr[relative_offset]));
                        relative_offset += static_cast<uint32_t>(sizeof(float));
                        break;

                    default:
                        break;
                }
            }
        }
    }


    void AnimationSampler::_InterpolateWeights(DENG::Renderer &_renderer, float _delta_time) {
        std::cerr << "Morph target animations are not supported by current implementation" << std::endl;
        DENG_ASSERT(false);
    }


    void AnimationSampler::_InterpolateTranslation(DENG::Renderer &_renderer, float _delta_time) {
        const float start = m_timestamps[m_active_timestamp_index], end = m_timestamps[m_active_timestamp_index + 1];
        auto val = LinearInterpolator::ArithmeticInterp(start, _delta_time, end, *std::get<Libdas::Vector3<float>*>(m_interp_values[m_active_timestamp_index]), 
                                                        *std::get<Libdas::Vector3<float>*>(m_interp_values[m_active_timestamp_index + 1]));

        m_animation_ubo.translation_target = Libdas::Vector4<float>(val.first, val.second, val.third, 1.0f);
        m_animation_ubo.target_mask = TARGET_MASK_TRANSLATION;
        _renderer.UpdateUniform(reinterpret_cast<char*>(&m_animation_ubo), m_shader_id, m_ubo_id);
    }


    void AnimationSampler::_InterpolateRotation(DENG::Renderer &_renderer, float _delta_time) {
        const float start = m_timestamps[m_active_timestamp_index], end = m_timestamps[m_active_timestamp_index + 1];
        auto val = LinearInterpolator::InterpolateQuaternionRotation(start, _delta_time, end, *std::get<Libdas::Quaternion*>(m_interp_values[m_active_timestamp_index]), 
                                                                     *std::get<Libdas::Quaternion*>(m_interp_values[m_active_timestamp_index]));

        m_animation_ubo.rotation = val;
        m_animation_ubo.target_mask = TARGET_MASK_ROTATION;
        _renderer.UpdateUniform(reinterpret_cast<char*>(&m_animation_ubo), m_shader_id, m_ubo_id);
    }


    void AnimationSampler::_InterpolateScale(DENG::Renderer &_renderer, float _delta_time) {
        const float start = m_timestamps[m_active_timestamp_index], end = m_timestamps[m_active_timestamp_index + 1];
        float val = LinearInterpolator::ArithmeticInterp(start, _delta_time, end, std::get<float>(m_interp_values[m_active_timestamp_index]),
                                                       std::get<float>(m_interp_values[m_active_timestamp_index + 1]));

        m_animation_ubo.uniform_scale = val;
        m_animation_ubo.target_mask = TARGET_MASK_SCALE;
        _renderer.UpdateUniform(reinterpret_cast<char*>(&m_animation_ubo), m_shader_id, m_ubo_id);
    }


    void AnimationSampler::Update(DENG::Renderer &_renderer) {
        // calculate delta time
        m_active_time = std::chrono::system_clock::now();
        std::chrono::duration<float, std::milli> delta_time = m_active_time - m_beg_time;
        
        // check the current timestamp against keyframe values
        if(delta_time.count() / 1000.0f >= m_timestamps[m_active_timestamp_index + 1]) {
            m_active_timestamp_index++;
            if(m_active_timestamp_index >= m_timestamps.size() - 1)
                m_active_timestamp_index = 0;
        }

        // check the animation target type
        switch(m_channel.target) {
            case LIBDAS_ANIMATION_TARGET_WEIGHTS:
                _InterpolateWeights(_renderer, delta_time.count() / 1000);
                break;

            case LIBDAS_ANIMATION_TARGET_TRANSLATION:
                _InterpolateTranslation(_renderer, delta_time.count() / 1000);
                break;

            case LIBDAS_ANIMATION_TARGET_ROTATION:
                _InterpolateRotation(_renderer, delta_time.count() / 1000);
                break;

            case LIBDAS_ANIMATION_TARGET_SCALE:
                _InterpolateScale(_renderer, delta_time.count() / 1000);
                break;

            default:
                DENG_ASSERT(false);
                break;
        }
    }
}
