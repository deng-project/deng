// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: LevelLoader.h - game.xml level loader class header
// author: Karl-Mihkel Ott

#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#ifdef LEVEL_LOADER_CPP
	#include <string>
	#include <vector>
	#include <queue>
	#include <unordered_map>
	#include <chrono>
	#include <array>

#ifdef _WIN32
	#include <wtypes.h>
#endif
	
	#include "trs/Points.h"
	#include "trs/Vector.h"
	#include "trs/Matrix.h"
	#include "trs/Quaternion.h"

	#include "das/Api.h"
	#include "das/DasStructures.h"
	#include "das/TextureReader.h"

	#include "dxml/GameStructs.h"

	#include "deng/Api.h"
	#include "deng/BaseTypes.h"
	#include "deng/ErrorDefinitions.h"
	#include "deng/ShaderDefinitions.h"
	#include "deng/ModelUniforms.h"
	#include "deng/Renderer.h"
	#include "deng/Entity.h"
	#include "deng/ScriptableEntity.h"
	#include "deng/Camera3D.h"
	#include "deng/EditorCamera.h"
	#include "deng/FirstPersonCamera.h"
#endif

namespace DENG {

	
	class DENG_API LevelLoader {
		private:
			Renderer& m_renderer;
			std::vector<Entity*> m_loaded_entities;

		public:
			//LevelLoader(DXML::Level& _level, Renderer &_rend);
			//void Update(DXML::Level& _level);
	};
}

#endif