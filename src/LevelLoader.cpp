// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: LevelLoader.cpp - game.xml level loader class implementation
// author: Karl-Mihkel Ott

#define LEVEL_LOADER_CPP
#include "deng/LevelLoader.h"

namespace DENG {

	LevelLoader::LevelLoader(DXML::Level& _lvl, Renderer &_rend) : 
		m_renderer(_rend) 
	{
	}

	void LevelLoader::Update(DXML::Level& _lvl) {

	}
}