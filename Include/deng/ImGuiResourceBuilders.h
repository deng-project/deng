// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ImGuiResourceBuilders.h - ImGui resource builder class headers
// author: Karl-Mihkel Ott

#pragma once

#include "deng/Api.h"
#include "deng/ResourceBuilders.h"

#ifdef IMGUI_RESOURCE_BUILDERS_CPP
#include <imgui.h>
#include "deng/FileSystemGraphicsShader.h"
#endif

namespace DENG {

	class DENG_API ImGuiShaderBuilder : public IGraphicsShaderBuilder {
		private:
			size_t m_uUniformOffset = 0;
			cvar::hash_t m_hshTexture = 0;

		public:
			ImGuiShaderBuilder(IGraphicsShaderCompiler* _pGraphicsShaderCompiler, size_t _uUniformOffset, cvar::hash_t _hshTexture) :
				IGraphicsShaderBuilder(_pGraphicsShaderCompiler),
				m_uUniformOffset(_uUniformOffset),
				m_hshTexture(_hshTexture) {}

			IGraphicsShader* Get();
	};

	class DENG_API ImGuiTextureBuilder : public ITextureBuilder {
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

	class DENG_API ImGuiMeshBuilder : public IMeshBuilder {
		public:
			ImGuiMeshBuilder() = default;
			MeshCommands Get();
	};
}