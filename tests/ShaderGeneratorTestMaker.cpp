// generate n tests to use
// args: 
//   out (output directory)
//   n (generatable test count)
#include <iostream>
#include <fstream>
#include <random>
#include <functional>

int main() {
    std::string path;
    std::cin >> path;
    uint32_t n;
    std::cin >> n;

    for(uint32_t i = 0; i < n; i++) {
        std::ofstream s(path + "/in." + std::to_string(i));
        std::random_device generator;
        std::uniform_int_distribution<int> bool_dist(0, 1);

        std::string bool_str;
        bool normal = bool_dist(generator);
        bool_str = normal ? "true" : "false";
        s << bool_str << std::endl;

        bool tangent = bool_dist(generator);
        bool_str = tangent ? "true\n" : "false\n";
        s.write(bool_str.c_str(), bool_str.size());

        std::uniform_int_distribution<uint32_t> int_dist(0, 16);
        uint32_t tex_count = int_dist(generator);
        uint32_t color_count = tex_count;
        if(!tex_count)
            color_count = int_dist(generator);
        uint32_t joint_count = int_dist(generator);
        uint32_t morph_target_count = int_dist(generator);
        s << tex_count << std::endl;
        s << color_count << std::endl;
        s << joint_count << std::endl;
        s << morph_target_count << std::endl;

        // generate morph targets
        // morph target properties:
        bool is_morph_normal = normal ? bool_dist(generator) : false;
        bool is_morph_tangent = tangent ? bool_dist(generator) : false;
        bool is_morph_uv = tex_count ? bool_dist(generator) : false;
        bool is_morph_color = color_count ? bool_dist(generator) : false;
        for(uint32_t i = 0; i < morph_target_count; i++) {
            bool_str = is_morph_normal ? "true" : "false"; 
            s << bool_str << std::endl;
            bool_str = is_morph_tangent ? "true" : "false";
            s << bool_str << std::endl;

            s << (is_morph_uv ? tex_count : 0) << std::endl;
            s << (is_morph_color ? color_count : 0) << std::endl;
        }
        s.close();
    }
    return 0;
}
