// Pydeng: Python scripting API for DENG
// licence: Apache, see LICENCE file
// file: ComponentFinder.h - 
// author: Karl-Mihkel Ott

#ifndef PYDENG_COMPONENT_FINDER_H
#define PYDENG_COMPONENT_FINDER_H

#ifdef PYDENG_COMPONENT_FINDER_CPP
	#include <string>
	#include <deng/deng.h>
#endif

namespace Pydeng {
	class ComponentFinder {
		private:
			DENG::Registry* m_reg;
		
		public:
			ComponentFinder() : m_reg(DENG::Registry::GetInstance()) {}
			
			uint32_t FindIdByName(const std::string& _name) {
				return m_reg->FindIdByName(_name);
			}

			DENG::Entity* GetFirstEntityByType(DENG::EntityType _type) {
				return m_reg->GetFirstEntityByType(_type);
			}

			DENG::Entity* GetLastEntityByType(DENG::EntityType _type) {
				return m_reg->GetLastEntityByType(_type);
			}

			DENG::Entity* GetEntityById(uint32_t _id) {
				return m_reg->GetEntityById(_id);
			}
	};
}

#endif