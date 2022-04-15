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


    void AnimationSampler::_LinearInterpolation(float _t1, float _tc, float _t2) {
        const uint32_t next = (m_active_timestamp_index + 1) % m_timestamps.size();
        const float t = (_tc - _t1) / (_t2 - _t1);

        switch(m_interp_values[m_active_timestamp_index].index()) {
            // weights
            case 0:
                { 
                    // not yet implemented
                }
                break;

            // translation
            case 1:
                {
                    Libdas::Vector3<float> *tr1 = std::get<Libdas::Vector3<float>*>(m_interp_values[m_active_timestamp_index]);
                    Libdas::Vector3<float> *tr2 = std::get<Libdas::Vector3<float>*>(m_interp_values[next]);

                    Libdas::Vector3<float> interp = *tr1 * (1 - t) + *tr2 * t;
                    m_ubo.translation.row4.first = interp.first;
                    m_ubo.translation.row4.second = interp.second;
                    m_ubo.translation.row4.third = interp.third;
                }
                break;

            // rotation
            case 2:
                {
                    Libdas::Quaternion *q1 = std::get<Libdas::Quaternion*>(m_interp_values[m_active_timestamp_index]);
                    Libdas::Quaternion *q2 = std::get<Libdas::Quaternion*>(m_interp_values[next]);
                    float dot = Libdas::Quaternion::Dot(*q1, *q2);

                    float sign = dot < 0.0f ? -1.0f : 1.0f;
                    float a = acosf(dot);

                    Libdas::Quaternion interp = *q1 * sinf(a * (1 - t)) / sinf(a) + *q2 * sign * sinf(a * t) / sinf(a);
                    m_ubo.rot = interp.ExpandToMatrix4().Transpose();
                }
                break;

            // uniform scale
            case 3:
                {
                    float s1 = std::get<float>(m_interp_values[m_active_timestamp_index]);
                    float s2 = std::get<float>(m_interp_values[m_active_timestamp_index]);

                    float interp = (1 - t) * s1 + s2 * t;
                    m_ubo.scale.row1.first = interp;;
                    m_ubo.scale.row2.second = interp;
                    m_ubo.scale.row3.third = interp;
                }
                break;
        }
    }


    void AnimationSampler::_StepInterpolation() {
        switch(m_interp_values[m_active_timestamp_index].index()) {
            // weight
            case 0:
                {
                    // not yet implemented
                }
                break;

            // translation
            case 1:
                {
                    Libdas::Vector3<float> *tr = std::get<Libdas::Vector3<float>*>(m_interp_values[m_active_timestamp_index]);
                    m_ubo.translation.row4.first = tr->first;
                    m_ubo.translation.row4.second = tr->second;
                    m_ubo.translation.row4.third = tr->third;
                }
                break;

            // rotation
            case 2:
                {
                    Libdas::Quaternion *rot = std::get<Libdas::Quaternion*>(m_interp_values[m_active_timestamp_index]);
                    m_ubo.rot = rot->ExpandToMatrix4().Transpose();
                }
                break;

            // uniform scale
            case 3:
                {
                    float scale = std::get<float>(m_interp_values[m_active_timestamp_index]);
                    m_ubo.scale.row1.first = scale;
                    m_ubo.scale.row2.second = scale;
                    m_ubo.scale.row3.third = scale;
                }
                break;
        }
    }


    //void AnimationSampler::_CubicSplineInterpolation(const Libdas::Vector3<float> &_t1, float _tc, const Libdas::Vector3<float> &_t2) {
        //const float td = _t2.second - _t1.second;
        //const float t = (_tc - _t1.second) / td;

        //// tangents
        //const float a1 = _t1.first;
        //const float b1 = _t1.third;

        //const float a2 = _t2.first;
        //const float b2 = _t2.third;

        //switch(m_interp_values[m_active_timestamp_index].index()) {
            //// weights
            //case 0:
                //// not yet implemented
                //break;

            //// translation
            //case 1:
                //{
                    //Libdas::Vector3<float> *tr1 = std::get<Libdas::Vector3<float>*>(m_interp_values[m_active_timestamp_index]);
                    //Libdas::Vector3<float> *tr2 = std::get<Libdas::Vector3<float>*>(m_interp_values[m_active_timestamp_index + 1]);

                    //Libdas::Vector3<float> interp = *tr1 * (2 * CUBE(t) - 3 * SQR(t) + 1) + b1(td * (CUBE(t) - 2 * SQR(t) + t))
                //}
                //break;
        //}
    //}


    void AnimationSampler::Update(DENG::Renderer &_renderer) {
        // calculate delta time
        m_active_time = std::chrono::system_clock::now();
        std::chrono::duration<float, std::milli> delta_time = m_active_time - m_beg_time;
        const uint32_t next = (m_active_timestamp_index + 1) % static_cast<uint32_t>(m_timestamps.size());

        // check the current timestamp against keyframe values
        if(delta_time.count() / 1000.0f >= m_timestamps[next]) {
            m_active_timestamp_index++;
            if(m_active_timestamp_index >= m_timestamps.size()) {
                m_active_timestamp_index = 0;
                m_beg_time = std::chrono::system_clock::now();
            }
        }

        // set correct interpolation values
        if(m_animate) {
            switch(m_channel.interpolation) {
                case LIBDAS_INTERPOLATION_VALUE_LINEAR:
                    {
                        float t1 = m_timestamps[m_active_timestamp_index];
                        float t2 = m_timestamps[next];
                        _LinearInterpolation(t1, delta_time.count() / 1000.0f, t2);
                    }
                    break;

                case LIBDAS_INTERPOLATION_VALUE_CUBICSPLINE:
                    {
                        //const Libdas::Vector3<float> *t1 = std::get<const Libdas::Vector3<float>*>(m_timestamps[m_active_timestamp_index]);
                        //const Libdas::Vector3<float> *t2 = std::get<const Libdas::Vector3<float>*>(m_timestamps[m_active_timestamp_index]);
                        //_CubicSplineInterpolation(*t1, delta_time.count() / 1000.f, *t2);
                    }
                    break;

                case LIBDAS_INTERPOLATION_VALUE_STEP:
                    _StepInterpolation();
                    break;

                default:
                    break;
            }
        }

        // submit uniform data to renderer
        for(uint32_t offset : m_ubo_offsets)
            _renderer.UpdateUniform(reinterpret_cast<const char*>(&m_ubo), sizeof(ModelAnimationUbo), offset);
    }
}
