#include "deng/RenderResources.h"
#include "deng/Event.h"
#include "deng/AssetManager.h"

namespace DENG {
	ResourceManager ResourceManager::m_sResourceManager = ResourceManager();
	EventManager EventManager::m_sEventManager = EventManager();
	SharedAssetManager SharedAssetManager::m_sharedAssetManager = SharedAssetManager();
}