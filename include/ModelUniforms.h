// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelUniforms.h - header to contain data about all model uniforms
// author: Karl-Mihkel Ott

#ifndef MODEL_UNIFORMS_H
#define MODEL_UNIFORMS_H

#define MAX_MORPH_TARGETS  16

namespace DENG {
    struct ModelCameraUbo {
        Libdas::Matrix4<float> projection_matrix;
        Libdas::Matrix4<float> view_matrix;
    };

    struct ModelUbo {
        Libdas::Matrix4<float> node_transform;
        Libdas::Vector4<float> color;
        float morph_weights[MAX_MORPH_TARGETS] = {};
        uint32_t use_color = 0;
    };
}

#endif
