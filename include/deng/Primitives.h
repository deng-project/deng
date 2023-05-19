// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: Primitives.h - header basic with primitive classes
// author: Karl-Mihkel Ott

#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#ifdef PRIMITIVES_CPP
	#include <array>
	#include <string>

	#include "deng/Api.h"
	#include "deng/Entity.h"
	#include "deng/ScriptableEntity.h"
	#include "deng/GameObject.h"
	#include "deng/RenderableMesh.h"
#endif

namespace DENG {

	class DENG_API CubeComponent : public RenderableMeshComponent {
		private:
			std::array<TRS::Vector3<float>, 8> m_vertices;
			std::array<uint32_t, 36> m_indices;

		public:
			Cube(TRS::Point3D<float> _min, TRS::Point3D<float> _max);
	};
}

#endif