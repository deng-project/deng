// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: RenderResources.h - header for all possible renderer resources
// author: Karl-Mihkel Ott

#ifndef RENDER_RESOURCES_H
#define RENDER_RESOURCES_H

#include "trs/Points.h"
#include "trs/Vector.h"

#include <cvar/SID.h>

#include "deng/Api.h"
#include "deng/IGraphicsShader.h"
#include "deng/ResourceEvents.h"

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

	template<typename T>
	struct Material {
		T material;
		std::array<cvar::hash_t, T::s_uTextureCount> textures {};
	};

	class DENG_API ResourceManager {
		private:
			std::mutex m_mutex;

			std::unordered_map<cvar::hash_t, MeshCommands, cvar::NoHash> m_meshes;
			std::unordered_map<cvar::hash_t, Material<MaterialPBR>, cvar::NoHash> m_pbrMaterials;
			std::unordered_map<cvar::hash_t, Material<MaterialPhong>, cvar::NoHash> m_phongMaterials;
			std::unordered_map<cvar::hash_t, IGraphicsShader*, cvar::NoHash> m_graphicsShaders;
			std::unordered_map<cvar::hash_t, Texture, cvar::NoHash> m_textures;

			EventManager& m_eventManager;
			static ResourceManager m_sResourceManager;

		private:
			ResourceManager() : m_eventManager(EventManager::GetInstance()) {}

		public:
			~ResourceManager() {
				for (auto it = m_graphicsShaders.begin(); it != m_graphicsShaders.end(); it++) {
					delete it->second;
				}
			}

			static ResourceManager& GetInstance() {
				return m_sResourceManager;
			}

			template<typename Builder, typename... Args>
			inline const MeshCommands& AddMesh(cvar::hash_t _hshMesh, Args&&... args) {
				std::scoped_lock lock(m_mutex);
				Builder meshBuilder(std::forward<Args>(args)...);
				m_meshes.insert(std::make_pair(_hshMesh, meshBuilder.Get()));

				m_eventManager.Dispatch<ResourceAddedEvent>(_hshMesh, ResourceType::Mesh);
				return m_meshes[_hshMesh];
			}

			inline const std::unordered_map<cvar::hash_t, MeshCommands, cvar::NoHash>& GetMeshes() const {
				return m_meshes;
			}

			inline const MeshCommands* GetMesh(cvar::hash_t _uHash) const {
				auto it = m_meshes.find(_uHash);
				if (it == m_meshes.end())
					return nullptr;
				return &it->second;
			}

			inline MeshCommands* GetMesh(cvar::hash_t _hshMesh) {
				auto it = m_meshes.find(_hshMesh);
				if (it == m_meshes.end())
					return nullptr;
				return &it->second;
			}

			inline bool ExistsMesh(cvar::hash_t _hshMesh) const {
				return m_meshes.find(_hshMesh) != m_meshes.end();
			}

			inline void RemoveMesh(cvar::hash_t _hshMesh) {
				m_eventManager.Dispatch<ResourceRemoveEvent>(_hshMesh, ResourceType::Mesh);
				m_meshes.erase(_hshMesh);
			}

			template<typename Builder, typename... Args>
			inline const Material<MaterialPBR>& AddMaterialPBR(cvar::hash_t _hshMaterial, Args&&... args) {
				std::scoped_lock lock(m_mutex);
				Builder materialBuilder(std::forward<Args>(args)...);
				m_pbrMaterials.insert(std::make_pair(_hshMaterial, materialBuilder.Get()));

				m_eventManager.Dispatch<ResourceAddedEvent>(_hshMaterial, ResourceType::Material_PBR);
				return m_pbrMaterials[_hshMaterial];
			}

			inline const std::unordered_map<cvar::hash_t, Material<MaterialPBR>, cvar::NoHash>& GetPBRMaterials() const {
				return m_pbrMaterials;
			}

			inline const Material<MaterialPBR>* GetMaterialPBR(cvar::hash_t _hshMaterial) const {
				auto it = m_pbrMaterials.find(_hshMaterial);
				if (it == m_pbrMaterials.end())
					return nullptr;
				return &it->second;
			}

			Material<MaterialPBR>* GetMaterialPBR(cvar::hash_t _hshMaterial) {
				auto it = m_pbrMaterials.find(_hshMaterial);
				if (it == m_pbrMaterials.end())
					return nullptr;
				return &it->second;
			}

			inline bool ExistsMaterialPBR(cvar::hash_t _hshMaterial) {
				return m_pbrMaterials.find(_hshMaterial) != m_pbrMaterials.end();
			}

			inline void RemoveMaterialPBR(cvar::hash_t _hshMaterial) {
				m_eventManager.Dispatch<ResourceRemoveEvent>(_hshMaterial, ResourceType::Material_PBR);
				m_pbrMaterials.erase(_hshMaterial);
			}

			template<typename Builder, typename... Args>
			inline const Material<MaterialPhong>& AddMaterialPhong(cvar::hash_t _hshMaterial, Args&&... args) {
				std::scoped_lock lock(m_mutex);
				Builder materialBuilder(std::forward<Args>(args)...);
				m_phongMaterials.insert(std::make_pair(_uHash, materialBuilder.Get()));

				m_eventManager.Dispatch<ResourceAddedEvent>(_hshMaterial, ResourceType::Material_Phong);
				return m_phongMaterials[_uHash];
			}

			inline const std::unordered_map<cvar::hash_t, Material<MaterialPhong>, cvar::NoHash>& GetPhongMaterials() const {
				return m_phongMaterials;
			}

			Material<MaterialPhong>* GetMaterialPhong(cvar::hash_t _hshMaterial) {
				auto it = m_phongMaterials.find(_hshMaterial);
				if (it == m_phongMaterials.end())
					return nullptr;
				return &it->second;
			}

			inline bool ExistsMaterialPhong(cvar::hash_t _hshMaterial) {
				return m_phongMaterials.find(_hshMaterial) != m_phongMaterials.end();
			}

			inline void RemoveMaterialPhong(cvar::hash_t _hshMaterial) {
				m_eventManager.Dispatch<ResourceRemoveEvent>(_hshMaterial, ResourceType::Material_Phong);
				m_phongMaterials.erase(_hshMaterial);
			}

			template<typename Builder, typename... Args>
			inline const IGraphicsShader* AddGraphicsShader(cvar::hash_t _hshShader, Args&&... args) {
				std::scoped_lock lock(m_mutex);
				Builder shaderBuilder(std::forward<Args>(args)...);
				m_graphicsShaders.insert(std::make_pair(_hshShader, shaderBuilder.Get()));
				return m_graphicsShaders[_hshShader];
			}

			inline const IGraphicsShader* GetGraphicsShader(cvar::hash_t _hshShader) const {
				auto it = m_graphicsShaders.find(_hshShader);
				if (it == m_graphicsShaders.end())
					return nullptr;
				return it->second;
			}

			inline IGraphicsShader* GetGraphicsShader(cvar::hash_t _hshShader) {
				auto it = m_graphicsShaders.find(_hshShader);
				if (it == m_graphicsShaders.end())
					return nullptr;
				return it->second;
			}

			inline const std::unordered_map<std::size_t, IGraphicsShader*, cvar::NoHash>& GetGraphicsShaders() const {
				return m_graphicsShaders;
			}

			inline bool ExistsGraphicsShader(cvar::hash_t _hshShader) {
				return m_graphicsShaders.find(_hshShader) != m_graphicsShaders.end();
			}

			inline void RemoveGraphicsShader(cvar::hash_t _hshShader) {
				m_eventManager.Dispatch<ResourceRemoveEvent>(_hshShader, ResourceType::Shader);
				
				if (m_graphicsShaders.find(_hshShader) != m_graphicsShaders.end())
					delete m_graphicsShaders[_hshShader];
				m_graphicsShaders.erase(_hshShader);
			}

			template<typename Builder, typename... Args>
			inline const Texture& AddTexture(cvar::hash_t _hshTexture, Args&&... args) {
				std::scoped_lock lock(m_mutex);
				Builder textureBuilder(std::forward<Args>(args)...);
				m_textures.insert(std::make_pair(_hshTexture, textureBuilder.Get()));
				
				m_eventManager.Dispatch<ResourceAddedEvent>(_hshTexture, ResourceType::Texture);
				return m_textures[_hshTexture];
			}

			inline const Texture* GetTexture(cvar::hash_t _hshTexture) const {
				auto it = m_textures.find(_hshTexture);
				if (it == m_textures.end())
					return nullptr;
				return &it->second;
			}

			inline const std::unordered_map<cvar::hash_t, Texture, cvar::NoHash>& GetTextures() const {
				return m_textures;
			}

			inline bool ExistsTexture(cvar::hash_t _hshTexture) {
				return m_textures.find(_hshTexture) != m_textures.end();
			}

			inline void RemoveTexture(cvar::hash_t _hshTexture) {
				m_eventManager.Dispatch<ResourceRemoveEvent>(_hshTexture, ResourceType::Texture);
				m_textures.erase(_hshTexture);
			}

			inline void FreeTextureHeapData(cvar::hash_t _hshTexture) {
				delete[] m_textures[_hshTexture].pRGBAData;
			}
	};
}

#endif
