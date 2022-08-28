// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: BufferAlignment.h - Header containing buffer alignment inlined function
// author: Karl-Mihkel Ott

#ifndef BUFFER_ALIGNMENT_H
#define BUFFER_ALIGNMENT_H

namespace DENG {

    inline uint32_t AlignData(uint32_t _req_mem, uint32_t _align) {
        return _req_mem + (_req_mem % _align > 0 ? _align - (_req_mem % _align) : 0);
    }
}

#endif
