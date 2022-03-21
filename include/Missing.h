// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Missing.h - Missing texture data header
// author: Karl-Mihkel Ott

#ifndef MISSING_H
#define MISSING_H

#ifdef MISSING_CPP
    #include <cstdint>
#endif

namespace DENG {
    const char *GetMissingTexture(int &_x, int &_y, int &_size);
}

#endif
