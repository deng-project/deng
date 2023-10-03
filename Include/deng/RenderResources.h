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
#include "deng/IShader.h"
#include "deng/ResourceEvents.h"

#include <mutex>
#include <unordered_map>
#include <vector>
#include <optional>

#define PBR_TEXTURE_COUNT      6
#define PHONG_TEXTURE_COUNT    4

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

	enum PBRSamplerBits_T : uint32_t {
		PBRSamplerBit_None = 0,
		PBRSamplerBit_AlbedoMap = (1 << 0),
		PBRSamplerBit_EmissionMap = (1 << 1),
		PBRSamplerBit_NormalMap = (1 << 2),
		PBRSamplerBit_MetallicMap = (1 << 3),
		PBRSamplerBit_RoughnessMap = (1 << 4),
		PBRSamplerBit_AmbientOcclusionMap = (1 << 5)
	};

	typedef uint32_t PBRSamplerBits;

	struct alignas(16) MaterialPBR {
		MaterialPBR() = default;
		MaterialPBR(const MaterialPBR&) = default;
		MaterialPBR(MaterialPBR&&) = default;

		TRS::Vector4<float> vAlbedoFactor = { 1.f, 1.f, 1.f, 1.f };
		TRS::Vector4<float> vEmissiveFactor = { 0.f, 0.f, 0.f, 1.f };
		float fRoughness = 0.0f;
		float fMetallic = 0.0f;
		float fAmbientOcclusion = 1.f;
		PBRSamplerBits uSamplerBits = PBRSamplerBit_None;
	};

	enum PhongSamplerBits_T : uint32_t {
		PhongSamplerBit_None = 0,
		PhongSamplerBit_DiffuseMap = (1 << 0),
		PhongSamplerBit_SpecularMap = (1 << 1)
	};

	typedef uint32_t PhongSamplerBits;

	struct alignas(16) MaterialPhong {
		MaterialPhong() = default;
		MaterialPhong(const MaterialPhong&) = default;
		MaterialPhong(MaterialPhong&&) = default;

		TRS::Vector4<float> vDiffuse = { 1.0f, 0.f, 0.f, 1.f };
		TRS::Vector4<float> vSpecular = { 1.0f, 0.f, 0.f, 1.f };
		PhongSamplerBits uSamplerBits = PhongSamplerBit_None;
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

	template<typename T, size_t N>
	struct Material {
		T material;
		std::array<cvar::hash_t, N> textures {};
	};

	class DENG_API ResourceManager {
		private:
			std::mutex m_mutex;

			std::unordered_map<cvar::hash_t, MeshCommands, cvar::NoHash> m_meshes;
			std::unordered_map<cvar::hash_t, Material<MaterialPBR, PBR_TEXTURE_COUNT>, cvar::NoHash> m_pbrMaterials;
			std::unordered_map<cvar::hash_t, Material<MaterialPhong, PHONG_TEXTURE_COUNT>, cvar::NoHash> m_phongMaterials;
			std::unordered_map<cvar::hash_t, IShader*, cvar::NoHash> m_shaders;
			std::unordered_map<cvar::hash_t, Texture, cvar::NoHash> m_textures;

			EventManager& m_eventManager;
			static ResourceManager m_sResourceManager;

		private:
			ResourceManager() : m_eventManager(EventManager::GetInstance()) {}

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
			inline const Material<MaterialPBR, PBR_TEXTURE_COUNT>& AddMaterialPBR(cvar::hash_t _hshMaterial, Args&&... args) {
				std::scoped_lock lock(m_mutex);
				Builder materialBuilder(std::forward<Args>(args)...);
				m_pbrMaterials.insert(std::make_pair(_hshMaterial, materialBuilder.Get()));

				m_eventManager.Dispatch<ResourceAddedEvent>(_hshMaterial, ResourceType::Material_PBR);
				return m_pbrMaterials[_hshMaterial];
			}

			inline const std::unordered_map<cvar::hash_t, Material<MaterialPBR, PBR_TEXTURE_COUNT>, cvar::NoHash>& GetPBRMaterials() const {
				return m_pbrMaterials;
			}

			inline const Material<MaterialPBR, PBR_TEXTURE_COUNT>* GetMaterialPBR(cvar::hash_t _hshMaterial) const {
				auto it = m_pbrMaterials.find(_hshMaterial);
				if (it == m_pbrMaterials.end())
					return nullptr;
				return &it->second;
			}

			Material<MaterialPBR, PBR_TEXTURE_COUNT>* GetMaterialPBR(cvar::hash_t _hshMaterial) {
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
			inline const Material<MaterialPhong, PHONG_TEXTURE_COUNT>& AddMaterialPhong(cvar::hash_t _hshMaterial, Args&&... args) {
				std::scoped_lock lock(m_mutex);
				Builder materialBuilder(std::forward<Args>(args)...);
				m_phongMaterials.insert(std::make_pair(_uHash, materialBuilder.Get()));

				m_eventManager.Dispatch<ResourceAddedEvent>(_hshMaterial, ResourceType::Material_Phong);
				return m_phongMaterials[_uHash];
			}

			inline const std::unordered_map<cvar::hash_t, Material<MaterialPhong, PHONG_TEXTURE_COUNT>, cvar::NoHash>& GetPhongMaterials() const {
				return m_phongMaterials;
			}

			Material<MaterialPhong, PHONG_TEXTURE_COUNT>* GetMaterialPhong(cvar::hash_t _hshMaterial) {
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
			inline const IShader* AddShader(cvar::hash_t _hshShader, Args&&... args) {
				std::scoped_lock lock(m_mutex);
				Builder shaderBuilder(std::forward<Args>(args)...);
				m_shaders.insert(std::make_pair(_hshShader, shaderBuilder.Get()));
				return m_shaders[_hshShader];
			}

			inline const IShader* GetShader(cvar::hash_t _hshShader) const {
				auto it = m_shaders.find(_hshShader);
				if (it == m_shaders.end())
					return nullptr;
				return it->second;
			}

			inline IShader* GetShader(cvar::hash_t _hshShader) {
				auto it = m_shaders.find(_hshShader);
				if (it == m_shaders.end())
					return nullptr;
				return it->second;
			}

			inline const std::unordered_map<std::size_t, IShader*, cvar::NoHash>& GetShaders() const {
				return m_shaders;
			}

			inline bool ExistsShader(cvar::hash_t _hshShader) {
				return m_shaders.find(_hshShader) != m_shaders.end();
			}

			inline void RemoveShader(cvar::hash_t _hshShader) {
				m_eventManager.Dispatch<ResourceRemoveEvent>(_hshShader, ResourceType::Shader);
				
				if (m_shaders.find(_hshShader) != m_shaders.end())
					delete m_shaders[_hshShader];
				m_shaders.erase(_hshShader);
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
