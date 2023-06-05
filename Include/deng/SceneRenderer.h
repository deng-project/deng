// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: SceneRenderer.h - scene renderer class header
// author: Karl-Mihkel Ott

#ifndef SCENE_RENDERER_H
#define SCENE_RENDERER_H

#include "deng/Api.h"
#include "deng/IRenderer.h"
#include "deng/Components.h"

#ifdef SCENE_RENDERER_CPP
	#include "trs/Vector.h"
#endif

namespace DENG {

	struct DrawableData {
		const MeshComponent* pMeshComponent = nullptr;
		ShaderComponent* pShaderComponent = nullptr;

		bool operator==(const DrawableData& _drawable) const {
			return pMeshComponent == _drawable.pMeshComponent && pShaderComponent == _drawable.pShaderComponent;
		}

		struct Hash {
			std::size_t operator()(const DrawableData& _batch) const {
				std::size_t uSeed = 0;
				std::hash<void*> hasher;
				uSeed ^= hasher((void*)_batch.pMeshComponent) + 0x9e3779b9 + (uSeed << 6) + (uSeed >> 2);
				uSeed ^= hasher((void*)_batch.pShaderComponent) + 0x9e3779b9 + (uSeed << 6) + (uSeed >> 2);

				return uSeed;
			}
		};
	};

	struct Batch {
		Batch() = default;
		Batch(const Batch&) = default;
		Batch(Batch&& _batch) noexcept :
			pMeshComponent(_batch.pMeshComponent),
			pShaderComponent(_batch.pShaderComponent),
			materials(std::move(_batch.materials)),
			transforms(std::move(_batch.transforms)),
			uInstanceCount(_batch.uInstanceCount) {}


		const MeshComponent* pMeshComponent = nullptr;
		ShaderComponent* pShaderComponent = nullptr;
		std::vector<MaterialComponent> materials = {};
		std::vector<TransformComponent> transforms = {};
		uint32_t uInstanceCount = 0;

		bool operator==(const Batch& _other) const {
			return pMeshComponent == _other.pMeshComponent &&
				pShaderComponent == _other.pShaderComponent;
		}
	};



	class DENG_API SceneRenderer {
		private:
			IRenderer* m_pRenderer = nullptr;
			IFramebuffer* m_pFramebuffer = nullptr;

			// [ [ Transform SSBO ] [ Material SSBO ] ]
			size_t m_uInstancedSSBOOffset = 0;
			size_t m_uInstancedSSBOSize = 0;

			std::array<size_t, 3> m_arrLightOffsets = { SIZE_MAX, 0, 0 };
			size_t m_uUsedLightsSize = 0;

			char* m_pIntermediateStorageBuffer = nullptr;
			size_t m_uIntermediateStorageBufferSize = 0;

			// batching/instancing
			std::vector<std::vector<size_t>> m_batchLookup;
			std::vector<Batch> m_batches;

		public:
			SceneRenderer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer);
			~SceneRenderer();

			void RenderLights(
				const std::vector<PointLightComponent>& _pointLights, 
				const std::vector<DirectionalLightComponent>& _dirLights, 
				const std::vector<SpotlightComponent>& _spotLights,
				const TRS::Vector3<float>& _vAmbient);

			template<typename T, typename U>
			void RenderMesh(
				const T& _mesh,
				U& _shader,
				const MaterialComponent& _material,
				const TransformComponent& _transform,
				Entity _idMesh = entt::null,
				Entity _idShader = entt::null)
			{
				const size_t uMeshEntity = static_cast<size_t>(_idMesh);
				const size_t uShaderEntity = static_cast<size_t>(_idShader);

				if (uMeshEntity < m_batchLookup.size() && uShaderEntity < m_batchLookup[uMeshEntity].size() &&
					m_batchLookup[uMeshEntity][uShaderEntity] != SIZE_MAX)
				{
					if constexpr (std::is_same<T, MeshComponent>::value)
						m_batches[m_batchLookup[uMeshEntity][uShaderEntity]].pMeshComponent = &_mesh;
					else m_batches[m_batchLookup[uMeshEntity][uShaderEntity]].pMeshComponent = &_mesh.Get();

					if constexpr (std::is_same<U, ShaderComponent>::value)
						m_batches[m_batchLookup[uMeshEntity][uShaderEntity]].pShaderComponent = &_shader;
					else m_batches[m_batchLookup[uMeshEntity][uShaderEntity]].pShaderComponent = &_shader.Get();

					m_batches[m_batchLookup[uMeshEntity][uShaderEntity]].materials.push_back(_material);
					m_batches[m_batchLookup[uMeshEntity][uShaderEntity]].transforms.push_back(_transform);
					m_batches[m_batchLookup[uMeshEntity][uShaderEntity]].uInstanceCount++;
				}
				else {
					m_batches.emplace_back();

					if constexpr (std::is_same<T, MeshComponent>::value)
						m_batches.back().pMeshComponent = &_mesh;
					else m_batches.back().pMeshComponent = &_mesh.Get();

					if constexpr (std::is_same<U, ShaderComponent>::value)
						m_batches.back().pShaderComponent = &_shader;
					else m_batches.back().pShaderComponent = &_shader.Get();
					
					m_batches.back().materials.push_back(_material);
					m_batches.back().transforms.push_back(_transform);
					m_batches.back().uInstanceCount++;

					if (uMeshEntity >= m_batchLookup.size())
						m_batchLookup.resize(uMeshEntity + 1);
					if (uShaderEntity >= m_batchLookup[uMeshEntity].size())
						m_batchLookup[uMeshEntity].resize(uShaderEntity + 1, SIZE_MAX);

					m_batchLookup[uMeshEntity][uShaderEntity] = m_batches.size() - 1;
				}
			}

			void SubmitBatches(const CameraComponent& _camera);
	};
}

#endif