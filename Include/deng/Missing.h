// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Missing.h - Missing texture data header
// author: Karl-Mihkel Ott

#ifndef MISSING_H
#define MISSING_H

#ifdef MISSING_CPP
    #include "deng/Api.h"
    #include <cstdint>
#endif

#define MISSING_TEXTURE_NAME    "__none__"
#define MISSING_3D_TEXTURE_NAME "__none3d__"

namespace DENG {
    DENG_API char *GetMissingTextureRGBA(int &_x, int &_y, int &_size);
    DENG_API char* GetMissingTextureRGB(int& _x, int& _y, int& _size);
}

#endif
