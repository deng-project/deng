// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelUniforms.h - header to contain data about all model uniforms
// author: Karl-Mihkel Ott

#ifndef MODEL_UNIFORMS_H
#define MODEL_UNIFORMS_H

namespace DENG {
    struct ModelCameraUbo {
        Libdas::Matrix4<float> projection_matrix;
        Libdas::Matrix4<float> view_matrix;
    };

    struct ModelAnimationUbo {
        // animated properties
        Libdas::Quaternion rotation[2];
        Libdas::Vector4<float> weights[2];
        Libdas::Vector4<float> translation[2];
        float scales[2];

        uint32_t target_mask = 0; // see AnimationSampler.h
        uint32_t interpolation_mode = 0; // see DasStructures.h
        uint32_t animate = 0;

        // timestamp data in seconds
        float timestamps[2] = {};
        float current_time = 0;
        uint32_t used_weights = 0;
    };

    struct ModelUbo {
        Libdas::Matrix4<float> node_transform;
        Libdas::Matrix4<float> skeleton_transform;
        Libdas::Vector4<float> color;
    };
}

#endif
