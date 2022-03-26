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
        // write targets from buffer to target vector
        size_t current_buffer_offset = 0;
        size_t relative_offset = 0;
        const Libdas::DasBuffer &buffer = m_parser.AccessBuffer(m_channel.target_value_buffer_id);

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
                m_ubo.target_mask = TARGET_MASK_WEIGHT;
                // incomplete
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
