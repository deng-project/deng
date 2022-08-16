// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: TextureDump.cpp - Read contents of a image file and dump raw data to stdout
// author: Karl-Mihkel Ott

#include <cstdint>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>

#include "trs/Vector.h"
#include "trs/Matrix.h"
#include "trs/Points.h"
#include "trs/Quaternion.h"

#include "das/Api.h"
#include "das/DasStructures.h"
#include "das/TextureReader.h"

int main(int argc, char *argv[]) {
    if(argc < 2) {
        std::cerr << "Please provide a texture file name to use as an argument" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    Libdas::TextureReader reader(argv[1]);

    int x, y;
    size_t s;
    const char *data = reader.GetRawBuffer(x, y, s);

    std::cout << x << "x" << y << std::endl;
    for(size_t i = 0; i < s; i++)
        std::cout << data[i];

    return EXIT_SUCCESS;
}
