// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: StaticResources.h - Header for embedded image resources
// author: Karl-Mihkel Ott

#ifndef STATIC_RESOURCES_H
#define STATIC_RESOURCES_H

#ifdef STATIC_RESOURCES_CPP
    #include <cstddef>
#endif

namespace DENG {
	namespace Editor {
		
		const unsigned char *GetSmallLogo();
        size_t GetSmallLogoSize();
		const unsigned char *GetLogoIcon32x32();
        size_t GetLogoIcon32x32Size();
	
	}
}

#endif
