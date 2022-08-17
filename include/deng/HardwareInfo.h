// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: HardwareInfo.h - API neutral hardware info structure definition
// author: Karl-Mihkel Ott

#ifndef HARDWARE_INFO_H
#define HARDWARE_INFO_H

#define MAX_DEVICE_NAME_SIZE 256

namespace DENG {
    struct HardwareInfo {
        std::string gpu_name;
        std::string api_version;
        enum Type {
            INTEGRATED_GPU,
            DISCRETE_GPU,
            VIRTUAL_GPU,
            CPU,
            OTHER
        } type;
    };
};

#endif
