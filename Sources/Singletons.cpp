#include "deng/RenderResources.h"
#include "deng/Event.h"
#include "deng/AssetManager.h"

namespace DENG {
	EventManager EventManager::m_sEventManager = EventManager();
	SharedAssetManager SharedAssetManager::m_sharedAssetManager = SharedAssetManager();
}