// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLTextureData.h - OpenGL texture data structure definitions
// author: Karl-Mihkel Ott

#ifndef OPENGL_TEXTURE_DATA_H
#define OPENGL_TEXTURE_DATA_H

namespace DENG {
	namespace OpenGL {
        
        struct TextureData {
            GLuint texture = 0;
            uint32_t width = 0;
            uint32_t height = 0;
            uint32_t depth = 0;
        };

	}
}

#endif