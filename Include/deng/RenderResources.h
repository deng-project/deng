// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: RenderResources.h - header for all possible renderer resources
// author: Karl-Mihkel Ott

#ifndef RENDER_RESOURCES_H
#define RENDER_RESOURCES_H

#include "trs/Points.h"
#include "trs/Vector.h"

#include "deng/Api.h"
#include "deng/IShader.h"
#include "deng/SID.h"

#include <mutex>
#include <unordered_map>
#include <vector>
#include <optional>

namespace DENG {

	template<typename... Attrs>
	struct Vertex {};

	template<typename Attr>
	struct Vertex<Attr> {
		Attr val;
		
		Vertex() = default;
		Vertex(const Attr& _val) : val(_val) {}
	};

	template<typename Attr, typename... Next>
	struct Vertex<Attr, Next...> {
		Attr val;
		Vertex<Next...> next;

		Vertex() = default;
		Vertex(const Attr& _val, Next&&... _next) :
			val(_val),
			next(std::forward<Next>(_next)...) {}
	};

	template<typename T>
	struct Mesh {
		std::vector<T> vertices;
		std::vector<uint32_t> indices;
	};

	
	struct Scissor {
		TRS::Point2D<int32_t> offset;
		TRS::Point2D<uint32_t> extent;
	};


	struct MeshDrawCommand {
		std::vector<std::size_t> attributeOffsets;
		Scissor scissor;
		std::size_t uIndicesOffset = 0;
		uint32_t uDrawCount = 0;
	};


	struct MeshCommands {
		std::string sName = "MyMesh";
		std::vector<MeshDrawCommand> drawCommands;
	};


	struct alignas(16) Material {
		Material() = default;
		Material(const Material&) = default;
		Material(Material&&) = default;

		TRS::Vector4<float> vAlbedoFactor = {1.f, 1.f, 1.f, 1.f};
		TRS::Vector4<float> vEmissiveFactor = { 0.f, 0.f, 0.f, 1.f };
		float fRoughness = 0.7f;
		float fMetallic = 0.4f;
	};

	enum class TextureType {
		None,
		Image_1D,
		Image_2D,
		Image_3D_Array,
		Image_3D,
		InternalFramebufferImage_2D,
		InternalFramebufferImage_3D,
		SwapchainImage
	};

	enum TextureLoadType {
		None,
		Embedded,
		External_JPEG,
		External_PNG,
		External_TIFF,
		External_BMP,
		External_TGA,
		External_GIF
	};

	struct Texture {
		uint32_t uWidth = 0;
		uint32_t uHeight = 0;
		uint32_t uBitDepth = 0;

		TextureType eResourceType = TextureType::None;
		TextureLoadType eLoadType = TextureLoadType::None;

		char* pRGBAData = nullptr;
		bool bHeapAllocationFlag = false;
	};

	enum class ResourceType {
		None,
		Mesh,
		Shader,
		Material,
		Texture
	};

	class DENG_API ResourceManager {
		private:
			std::mutex m_mutex;

			std::unordered_map<hash_t, MeshCommands, NoHash> m_meshes;
			std::unordered_map<hash_t, Material, NoHash> m_materials;
			std::unordered_map<hash_t, IShader*, NoHash> m_shaders;
			std::unordered_map<hash_t, Texture, NoHash> m_textures;

			static ResourceManager m_sResourceManager;

		private:
			ResourceManager() = default;

		public:
			~ResourceManager() {
				for (auto it = m_shaders.begin(); it != m_shaders.end(); it++) {
					delete it->second;
				}
			}

			static ResourceManager& GetInstance() {
				return m_sResourceManager;
			}

			template<typename Builder, typename... Args>
			inline const MeshCommands& AddMesh(hash_t _uHash, Args&&... args) {
				std::scoped_lock lock(m_mutex);
				Builder meshBuilder(std::forward<Args>(args)...);
				m_meshes.insert(std::make_pair(_uHash, meshBuilder.Get()));
				return m_meshes[_uHash];
			}

			inline const std::unordered_map<std::size_t, MeshCommands, NoHash>& GetMeshes() const {
				return m_meshes;
			}

			inline const MeshCommands* GetMesh(hash_t _uHash) const {
				auto it = m_meshes.find(_uHash);
				if (it == m_meshes.end())
					return nullptr;
				return &it->second;
			}

			inline MeshCommands* GetMesh(hash_t _hshMesh) {
				auto it = m_meshes.find(_hshMesh);
				if (it == m_meshes.end())
					return nullptr;
				return &it->second;
			}


			template<typename Builder, typename... Args>
			inline const Material& AddMaterial(hash_t _uHash, Args&&... args) {
				std::scoped_lock lock(m_mutex);
				Builder materialBuilder(std::forward<Args>(args)...);
				m_materials.insert(std::make_pair(_uHash, materialBuilder.Get()));
				return m_materials[_uHash];
			}

			inline const std::unordered_map<std::size_t, Material, NoHash>& GetMaterials() const {
				return m_materials;
			}

			inline const Material* GetMaterial(hash_t _uHash) const {
				auto it = m_materials.find(_uHash);
				if (it == m_materials.end())
					return nullptr;
				return &it->second;
			}

			Material* GetMaterial(hash_t _hshMaterial) {
				auto it = m_materials.find(_hshMaterial);
				if (it == m_materials.end())
					return nullptr;
				return &it->second;
			}

			template<typename Builder, typename... Args>
			inline const IShader* AddShader(hash_t _uHash, Args&&... args) {
				std::scoped_lock lock(m_mutex);
				Builder shaderBuilder(std::forward<Args>(args)...);
				m_shaders.insert(std::make_pair(_uHash, shaderBuilder.Get()));
				return m_shaders[_uHash];
			}

			inline const IShader* GetShader(hash_t _uHash) const {
				auto it = m_shaders.find(_uHash);
				if (it == m_shaders.end())
					return nullptr;
				return it->second;
			}

			inline IShader* GetShader(hash_t _hshShader) {
				auto it = m_shaders.find(_hshShader);
				if (it == m_shaders.end())
					return nullptr;
				return it->second;
			}

			inline const std::unordered_map<std::size_t, IShader*, NoHash>& GetShaders() const {
				return m_shaders;
			}


			template<typename Builder, typename... Args>
			inline const Texture& AddTexture(hash_t _uHash, Args&&... args) {
				std::scoped_lock lock(m_mutex);
				Builder textureBuilder(std::forward<Args>(args)...);
				m_textures.insert(std::make_pair(_uHash, textureBuilder.Get()));
				return m_textures[_uHash];
			}

			inline const Texture* GetTexture(hash_t _uHash) const {
				auto it = m_textures.find(_uHash);
				if (it == m_textures.end())
					return nullptr;
				return &it->second;
			}

			inline const std::unordered_map<hash_t, Texture, NoHash>& GetTextures() const {
				return m_textures;
			}

			inline void FreeTextureHeapData(hash_t _hshTexture) {
				delete[] m_textures[_hshTexture].pRGBAData;
			}
	};
}

#endif
