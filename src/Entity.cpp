#define ENTITY_CPP
#include "deng/Entity.h"

namespace DENG {
	
	Entity::Entity(Entity* _parent, const std::string& _name, const EntityType _type) :
		m_parent(_parent),
		m_name(_name),
		m_type(_type)
	{
		Registry* reg = Registry::GetInstance();
		m_registry_id = reg->AddEntity(this);
	}

	Entity::Entity(Entity&& _ent) noexcept :
		m_parent(_ent.m_parent),
		m_name(std::move(_ent.m_name)),
		m_type(_ent.m_type) 
	{
		Registry* reg = Registry::GetInstance();
		reg->Replace(m_registry_id, this);
	}

	Entity::~Entity() {
		Registry* reg = Registry::GetInstance();
		reg->RemoveEntity(m_registry_id);
	}
}