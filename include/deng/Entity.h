// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Entity.h - Generic entity structure definition header
// author: Karl-Mihkel Ott

#ifndef ENTITY_H
#define ENTITY_H

#ifdef ENTITY_CPP
	#include <python3.10/pyconfig.h>
	#include <pybind11/embed.h>

	#include "trs/Vector.h"
	#include "trs/Matrix.h"
	#include "trs/Quaternion.h"
	
	#include "dxml/GameStructs.h"

	#include "deng/Registry.h"
#endif

namespace DENG {

	class DENG_API Entity {
		private:
			std::string m_name;

		public:
			Entity();
			Entity(const Entity& _ent) = delete;
			Entity(Entity&& _ent) noexcept;
			~Entity();

			inline void SetName(const std::string& _name) {
				m_name = _name;
			}

			inline const std::string& GetName() {
				return m_name;
			}
	};
}

#endif