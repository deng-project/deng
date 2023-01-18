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
		const unsigned char *GetRunIcon16x16();
		size_t GetRunIcon16x16Size();
		
		// asset panel icons
		const unsigned char *GetAssetAudioIconPng();
		size_t GetAssetAudioIconPngSize();
		const unsigned char *GetAssetConfigIconPng();
		size_t GetAssetConfigIconPngSize();
		const unsigned char* GetAssetImageIconPng();
		size_t GetAssetImageIconPngSize();
		const unsigned char *GetAssetModelIconPng();
		size_t GetAssetModelIconPngSize();
		const unsigned char *GetAssetPlaybackIconPng();
		size_t GetAssetPlaybackIconPngSize();
		const unsigned char *GetAssetPyScriptIconPng();
		size_t GetAssetPyScriptIconPngSize();
		const unsigned char *GetAssetUnknownIconPng();
		size_t GetAssetUnknownIconPngSize();
		const unsigned char *GetAssetXMLDocumentIconPng();
		size_t GetAssetXMLDocumentIconPngSize();
	}
}

#endif
