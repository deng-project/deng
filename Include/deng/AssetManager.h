#pragma once

#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <cvar/SID.h>

#include <deng/Api.h>
#include <deng/IGraphicsShader.h>
#include <deng/Materials.h>

namespace DENG {

	struct RelativeMemoryOffset {
		uint32_t uVertexOffset = 0;
		uint32_t uIndexOffset = 0;
	};

	// represents batch groups that would get drawn as part of MultiDrawIndirect pipeline
	struct Batch {
		IGraphicsShader* pBatchGraphicsShader = nullptr;
		std::vector<RelativeMemoryOffset> relativeLodOffsets;
		uint32_t uFirst = 0;
		uint32_t uCount = 0;
		uint32_t uInstances = 0;
	};

	enum class AssetPool : uint8_t {
		AssetPool_Local,
		AssetPool_Global
	};

	struct AssetCollection {
		// batches before explicit DBSCAN batching
		std::vector<Batch> indirectBatches;
		// batches after explicit DBSCAN batching
		std::vector<Batch> multiDrawBatches;
		
		// misc assets such as materials and textures
		std::unordered_map<cvar::hash_t, Material<MaterialPBR>, cvar::NoHash> pbrMaterials;
		std::unordered_map<cvar::hash_t, Material<MaterialPhong>, cvar::NoHash> phongMaterials;
		std::vector<cvar::hash_t> samplers;
	};


	class DENG_API SharedAssetManager : public AssetCollection {
		private:
			static SharedAssetManager m_sharedAssetManager;

		private:
			SharedAssetManager() = default;

		public:
			static AssetCollection& GetInstance() {
				return static_cast<AssetCollection&>(m_sharedAssetManager);
			}
	};
}