// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: AnimationSampler.h - AnimationSampler class implementation
// author: Karl-Mihkel Ott

#define ANIMATION_SAMPLER_CPP
#include <AnimationSampler.h>

namespace DENG {

    AnimationSampler::AnimationSampler(const Libdas::DasAnimationChannel &_channel, Libdas::DasParser &_parser, const std::vector<uint32_t> &_ubo_offsets) : 
        m_channel(_channel), m_parser(_parser), m_ubo_offsets(_ubo_offsets)
    {
        const Libdas::DasBuffer &kf_buffer = m_parser.AccessBuffer(m_channel.keyframe_buffer_id);
        const Libdas::DasBuffer &target_buffer = m_parser.AccessBuffer(m_channel.target_value_buffer_id);

        // get keyframe timestamps and target values
        m_timestamps.reserve(m_channel.keyframe_count);
        m_interp_values.reserve(m_channel.keyframe_count);

        uint32_t rel_timestamp_offset = m_channel.keyframe_buffer_offset;
        uint32_t rel_target_offset = m_channel.target_value_buffer_offset;

        for(uint32_t i = 0; i < m_channel.keyframe_count; i++) {
            DENG_ASSERT(kf_buffer.data_ptrs.size() == 1);
            DENG_ASSERT(target_buffer.data_ptrs.size() == 1);

            // push timestamps
            m_timestamps.push_back(*reinterpret_cast<const float*>(kf_buffer.data_ptrs.back().first + rel_timestamp_offset));
            rel_timestamp_offset += static_cast<uint32_t>(sizeof(float));

            char *data_ptr = const_cast<char*>(target_buffer.data_ptrs.back().first);

            // push target values
            switch(m_channel.target) {
                case LIBDAS_ANIMATION_TARGET_WEIGHTS:
                    m_interp_values.push_back(reinterpret_cast<float*>(data_ptr + rel_target_offset));
                    rel_target_offset += m_channel.weight_count * static_cast<uint32_t>(sizeof(float));
                    break;

                case LIBDAS_ANIMATION_TARGET_TRANSLATION:
                    m_interp_values.push_back(reinterpret_cast<Libdas::Vector3<float>*>(data_ptr + rel_target_offset));
                    rel_target_offset += static_cast<uint32_t>(sizeof(Libdas::Vector3<float>));
                    break;

                case LIBDAS_ANIMATION_TARGET_ROTATION:
                    m_interp_values.push_back(reinterpret_cast<Libdas::Quaternion*>(data_ptr + rel_target_offset));
                    std::cout << "Rotation quaternion: " << 
                                 std::get<Libdas::Quaternion*>(m_interp_values.back())->x << " " << 
                                 std::get<Libdas::Quaternion*>(m_interp_values.back())->y << " " << 
                                 std::get<Libdas::Quaternion*>(m_interp_values.back())->z << " " << 
                                 std::get<Libdas::Quaternion*>(m_interp_values.back())->w << std::endl;
                                  
                    rel_target_offset += static_cast<uint32_t>(sizeof(Libdas::Quaternion));
                    break;

                case LIBDAS_ANIMATION_TARGET_SCALE:
                    m_interp_values.push_back(*reinterpret_cast<float*>(data_ptr + rel_target_offset));
                    rel_target_offset += static_cast<uint32_t>(sizeof(float));
                    break;

                default:
                    DENG_ASSERT(false);
                    break;
            }
        }
    }


    void AnimationSampler::Update(DENG::Renderer &_renderer) {
        // calculate delta time
        m_active_time = std::chrono::system_clock::now();
        std::chrono::duration<float, std::milli> delta_time = m_active_time - m_beg_time;
        
        // check the current timestamp against keyframe values
        if(delta_time.count() / 1000.0f >= m_timestamps[m_active_timestamp_index + 1]) {
            m_active_timestamp_index++;
            if(m_active_timestamp_index >= m_timestamps.size() - 1 && m_repeat && m_animate)
                m_active_timestamp_index = 0;
            else m_ubo.animate = static_cast<uint32_t>(m_animate);
        }

        // set correct interpolation values
        switch(m_interp_values[m_active_timestamp_index].index()) {
            case 0:
                {
                    m_ubo.target_mask = TARGET_MASK_WEIGHT;
                    m_ubo.used_weights = m_weight_target_count;

                    DENG_ASSERT(m_weight_target_count > 4);
                    std::vector<float> weights[2];
                    weights[0].resize(m_weight_target_count);
                    weights[1].resize(m_weight_target_count);

                    for(size_t i = 0; i < m_weight_target_count; i++) {
                        weights[0][i] = std::get<float*>(m_interp_values[m_active_timestamp_index])[i];
                        weights[1][i] = std::get<float*>(m_interp_values[m_active_timestamp_index + 1])[i];
                    }

                    for(struct { Libdas::Vector4<float>::iterator t1; Libdas::Vector4<float>::iterator t2; uint32_t i; } s = {m_ubo.weights[0].Begin(), m_ubo.weights[1].Begin(), 0}; s.i < m_weight_target_count; s.i++, s.t1++, s.t2++) {
                        *s.t1 = weights[0][s.i];
                        *s.t2 = weights[1][s.i];
                    }
                }
                break;

            case 1:
                m_ubo.target_mask = TARGET_MASK_TRANSLATION;
                m_ubo.translation[0] = *std::get<Libdas::Vector3<float>*>(m_interp_values[m_active_timestamp_index]);
                m_ubo.translation[1] = *std::get<Libdas::Vector3<float>*>(m_interp_values[m_active_timestamp_index + 1]);
                break;

            case 2:
                m_ubo.target_mask = TARGET_MASK_ROTATION;
                m_ubo.rotation[0] = *std::get<Libdas::Quaternion*>(m_interp_values[m_active_timestamp_index]);
                m_ubo.rotation[1] = *std::get<Libdas::Quaternion*>(m_interp_values[m_active_timestamp_index + 1]);
                break;

            case 3:
                m_ubo.target_mask = TARGET_MASK_SCALE;
                m_ubo.scales[0] = std::get<float>(m_interp_values[m_active_timestamp_index]);
                m_ubo.scales[1] = std::get<float>(m_interp_values[m_active_timestamp_index]);
                break;

            default:
                DENG_ASSERT(false);
                break;
        }

        m_ubo.timestamps[0] = m_timestamps[m_active_timestamp_index];
        m_ubo.timestamps[1] = m_timestamps[m_active_timestamp_index + 1];
        m_ubo.current_time = delta_time.count() / 1000;

        // set correct interpolation method
        m_ubo.interpolation_mode = static_cast<uint32_t>(m_channel.interpolation);

        // submit uniform data to renderer
        for(uint32_t offset : m_ubo_offsets)
            _renderer.UpdateUniform(reinterpret_cast<const char*>(&m_ubo), sizeof(ModelAnimationUbo), offset);
    }
}
