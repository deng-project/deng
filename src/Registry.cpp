#define REGISTRY_CPP
#include "deng/Registry.h"


namespace DENG {

	Registry* Registry::m_instance = nullptr;


	Registry* Registry::GetInstance() {
		if (!m_instance)
			m_instance = new Registry;
		return m_instance;
	}


	void Registry::DeleteInstance() {
		delete m_instance;
	}


	void Registry::AttachEntities() {
		for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
			switch ((*it)->GetType()) {
				case ENTITY_TYPE_EDITOR_CAMERA:
					((EditorCamera*)(*it))->_Attach();
					break;

				case ENTITY_TYPE_FIRST_PERSON_CAMERA:
					((FirstPersonCamera*)(*it))->_Attach();
					break;

				case ENTITY_TYPE_GRID_MESH:
					((GridGenerator*)(*it))->_Attach();
					break;

				default:
					break;
			}
		}
	}


	void Registry::Update() {
		for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
			switch ((*it)->GetType()) {
				case ENTITY_TYPE_EDITOR_CAMERA:
					((EditorCamera*)(*it))->_Update();
					break;

				case ENTITY_TYPE_FIRST_PERSON_CAMERA:
					((FirstPersonCamera*)(*it))->_Update();
					break;

				case ENTITY_TYPE_GRID_MESH:
					((GridGenerator*)(*it))->_Update();
					break;
			}
		}
	}


	uint32_t Registry::FindIdByName(const std::string &_name) {
		for (uint32_t i = 0; i < static_cast<uint32_t>(m_entities.size()); i++) {
			if (m_entities[i]->GetName() == _name)
				return i;
		}

		return UINT32_MAX;
	}
}