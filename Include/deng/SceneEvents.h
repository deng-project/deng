// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SceneEvents.h - Scene events class header
// author: Karl-Mihkel Ott

#ifndef SCENE_EVENTS_H
#define SCENE_EVENTS_H

#include "deng/Scene.h"
#include "deng/Event.h"
#include "deng/Components.h"

namespace DENG {

	class DENG_API SceneEvent : public IEvent {
		public:
			EVENT_CLASS_TYPE_NO_ID_CHECK("SceneEvent", IEvent);
	};

	class DENG_API RegistryStateEvent : public SceneEvent {
		public:
			EVENT_CLASS_TYPE_NO_ID_CHECK("RegistryStateEvent", SceneEvent);
	};


	class DENG_API EntityEvent : public RegistryStateEvent {
		private:
			Entity m_idEntity;

		public:
			EntityEvent(Entity _idEntity) :
				m_idEntity(_idEntity) {}
			EntityEvent(const EntityEvent&) = default;

			inline Entity GetEntity() {
				return m_idEntity;
			}

			EVENT_CLASS_TYPE_NO_ID_CHECK("EntityEvent", RegistryStateEvent);
	};


	class DENG_API EntityAddedEvent : public EntityEvent {
		public:
			EntityAddedEvent(Entity _idEntity) :
				EntityEvent(_idEntity) {}
			EntityAddedEvent(const EntityAddedEvent&) = default;

			EVENT_CLASS_TYPE_NO_ID_CHECK("EntityAddedEvent", EntityEvent);
	};

	class DENG_API EntityRemoveEvent : public EntityEvent {
		public:
			EntityRemoveEvent(Entity _idEntity) :
				EntityEvent(_idEntity) {}
			EntityRemoveEvent(const EntityRemoveEvent&) = default;

			EVENT_CLASS_TYPE_NO_ID_CHECK("EntityRemoveEvent", EntityEvent);
	};

	class DENG_API ComponentEvent : public RegistryStateEvent {
		private:
			Entity m_idEntity;
			ComponentType m_eType;

		public:
			ComponentEvent(Entity _idEntity, ComponentType _eType) :
				m_idEntity(_idEntity),
				m_eType(_eType) {}
			ComponentEvent(const ComponentEvent&) = default;

			inline Entity GetEntity() {
				return m_idEntity;
			}

			inline ComponentType GetComponentType() {
				return m_eType;
			}

			EVENT_CLASS_TYPE_NO_ID_CHECK("ComponentEvent", RegistryStateEvent);
	};

	class DENG_API ComponentModifiedEvent : public ComponentEvent {
		public:
			ComponentModifiedEvent(Entity _idEntity, ComponentType _eType) :
				ComponentEvent(_idEntity, _eType) {}
			ComponentModifiedEvent(const ComponentModifiedEvent&) = default;

			EVENT_CLASS_TYPE_NO_ID_CHECK("ComponentModifiedEvent", ComponentEvent);
	};

	class DENG_API ComponentAddedEvent : public ComponentEvent {
		public:
			ComponentAddedEvent(Entity _idEntity, ComponentType _eType) :
				ComponentEvent(_idEntity, _eType) {}
			ComponentAddedEvent(const ComponentAddedEvent&) = default;

			EVENT_CLASS_TYPE_NO_ID_CHECK("ComponentAddedEvent", ComponentEvent);
	};

	class DENG_API ComponentRemoveEvent : public ComponentEvent {
		public:
			ComponentRemoveEvent(Entity _idEntity, ComponentType _eType) :
				ComponentEvent(_idEntity, _eType) {}
			ComponentRemoveEvent(const ComponentRemoveEvent&) = default;

			EVENT_CLASS_TYPE_NO_ID_CHECK("ComponentRemoveEvent", ComponentEvent);
	};

}

#endif
