// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VertexAttributes.cpp - Vertex attribute structures header file
// author: Karl-Mihkel Ott

#ifndef VERTEX_ATTRIBUTES_H
#define VERTEX_ATTRIBUTES_H

namespace DENG {

    struct UnmappedVertexAttribute {
        Libdas::Vector3<float> pos;
        Libdas::Vector3<float> norm;
    };
};

#endif
