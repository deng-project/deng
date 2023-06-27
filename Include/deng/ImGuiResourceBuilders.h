// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ImGuiResourceBuilders.h - ImGui resource builder class headers
// author: Karl-Mihkel Ott

#ifndef IMGUI_RESOURCE_BUILDERS_H
#define IMGUI_RESOURCE_BUILDERS_H

#include "deng/Api.h"
#include "deng/IShader.h"
#include "deng/RenderResources.h"

#ifdef IMGUI_RESOURCE_BUILDERS_CPP
#include <imgui.h>
#include "deng/FileSystemShader.h"
#endif

namespace DENG {

	class DENG_API ImGuiShaderBuilder {
		private:
			size_t m_uUniformOffset = 0;
			hash_t m_hshTexture = 0;

		public:
			ImGuiShaderBuilder(size_t _uUniformOffset, hash_t _hshTexture) :
				m_uUniformOffset(_uUniformOffset),
				m_hshTexture(_hshTexture) {}

			IShader* Get();
	};

	class DENG_API ImGuiTextureBuilder {
		private:
			int m_iWidth;
			int m_iHeight;
			unsigned char* m_pTexels;

		public:
			ImGuiTextureBuilder(int _iWidth, int _iHeight, unsigned char* _pTexels) :
				m_iWidth(_iWidth),
				m_iHeight(_iHeight),
				m_pTexels(_pTexels) {}

			Texture Get();
	};

	class DENG_API ImGuiMeshBuilder {
		public:
			ImGuiMeshBuilder() = default;
			MeshCommands Get();
	};
}

#endif
