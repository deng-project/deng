// args:
//  false|true (normal)
//  false|true (tangent)
//  k1 (texture_count)
//  k2 (color_mul_count)
//  k3 (joint_count)
//  n (morph_target_count) 
//  n : {
//    false|true (normal)
//    false|true (tangent)
//    k1 (texture_count)
//    k2 (color_mul_count)
//  }

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include "deng/ModelShaderGenerator.h"

int main(int argc, char *argv[]) {
    if(argc != 2 || (std::string(argv[1]) != "vert" && std::string(argv[1]) != "frag")) {
        std::cout << "Usage " << argv[0] << " vert|frag" << std::endl;
        std::exit(0);
    }

    std::string boolstr;
    DENG::MeshPrimitiveAttributeDescriptor desc;
    std::cin >> boolstr;
    desc.normal = boolstr == "true" ? true : false;
    std::cin >> boolstr;
    desc.tangent = boolstr == "true" ? true : false;
    std::cin >> desc.texture_count;
    std::cin >> desc.color_mul_count;
    std::cin >> desc.joint_set_count;

    uint32_t morph_target_count;
    std::cin >> morph_target_count;
    desc.morph_targets.resize(morph_target_count);
    for(uint32_t i = 0; i < morph_target_count; i++) {
        std::cin >> boolstr;
        desc.morph_targets[i].normal = boolstr == "true" ? true : false;
        std::cin >> boolstr;
        desc.morph_targets[i].tangent = boolstr == "true" ? true : false;
        std::cin >> desc.morph_targets[i].texture_count;
        std::cin >> desc.morph_targets[i].color_mul_count;
    }

    DENG::ModelShaderGenerator gen;
    if(std::string(argv[1]) == "vert")
        std::cout << gen.GenerateVertexShaderSource(desc);
    else if(std::string(argv[1]) == "frag")
        std::cout << gen.GenerateFragmentShaderSource(desc);
    return 0;
}
