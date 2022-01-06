#ifndef UNIFORM_DATA_H
#define UNIFORM_DATA_H

namespace DENG {
    
    struct UniformData {
        Libdas::Matrix4<float> camera;
        Libdas::Matrix4<float> view;
    };
};

#endif
