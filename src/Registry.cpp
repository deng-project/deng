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


	void Registry::Replace(uint32_t _id, Entity* _new) {
		DENG_ASSERT(_id < static_cast<uint32_t>(m_entities.size()));

		for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
			if ((*it)) continue;

			if ((*it)->GetParent() == m_entities[_id])
				(*it)->SetParent(_new);
		}

		m_entities[_id] = _new;
	}


	uint32_t Registry::AddEntity(Entity *_ent) {
		// check if there are any entity elements with nullptr value
		size_t i = 0;
		for (; i < m_entities.size(); i++) {
			if (m_entities[i] == nullptr)
				break;
		}

		if (i < m_entities.size()) {
			m_entities[i] = _ent;
			return static_cast<uint32_t>(i);
		} else {
			m_entities.push_back(_ent);
			return static_cast<uint32_t>(m_entities.size() - 1);
		}
	}


	void Registry::AttachEntities() {
		for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
			if (!*it) continue;

			switch ((*it)->GetType()) {
				case ENTITY_TYPE_EDITOR_CAMERA:
				{
					EditorCamera* cam = (EditorCamera*)*it;
					if (!cam->GetAttachedBit())
						cam->Attach();
					break;
				}

				case ENTITY_TYPE_FIRST_PERSON_CAMERA:
				{
					FirstPersonCamera* cam = (FirstPersonCamera*)*it;
					if (!cam->GetAttachedBit())
						cam->Attach();
					break;
				}

				case ENTITY_TYPE_GRID_MESH: 
				{
					GridGenerator* grid = (GridGenerator*)*it;
					if (!grid->GetAttachedBit())
						grid->Attach();
					break;
				}

				case ENTITY_TYPE_MODEL: 
				{
					ModelLoader* model = (ModelLoader*)*it;
					if (!model->GetAttachedBit())
						model->Attach();
				}

				default:
					break;
			}
		}
	}


	void Registry::Update() {
		for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
			if (!*it) continue;

			switch ((*it)->GetType()) {
				case ENTITY_TYPE_EDITOR_CAMERA:
				{
					EditorCamera* cam = (EditorCamera*)*it;
					if (cam->GetAttachedBit())
						cam->Update();
					break;
				}

				case ENTITY_TYPE_FIRST_PERSON_CAMERA:
				{
					FirstPersonCamera* cam = (FirstPersonCamera*)*it;
					if (cam->GetAttachedBit())
						cam->Update();
					break;
				}

				case ENTITY_TYPE_GRID_MESH:
				{
					GridGenerator* grid = (GridGenerator*)*it;
					if (grid->GetAttachedBit())
						grid->Update();
					break;
				}

				case ENTITY_TYPE_MODEL:
				{
					ModelLoader* model = (ModelLoader*)*it;
					if (model->GetAttachedBit())
						model->Update();
					break;
				}
			}
		}
	}


	uint32_t Registry::FindIdByName(const std::string &_name) {
		for (uint32_t i = 0; i < static_cast<uint32_t>(m_entities.size()); i++) {
			if (!m_entities[i]) continue;

			if (m_entities[i]->GetName() == _name)
				return i;
		}

		return UINT32_MAX;
	}
}