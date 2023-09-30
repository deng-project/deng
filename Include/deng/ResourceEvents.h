// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ResourceEvents.h - Resource events class header
// author: Karl-Mihkel Ott

#ifndef RESOURCE_EVENTS_H
#define RESOURCE_EVENTS_H

#include "deng/Event.h"

namespace DENG {

	enum class ResourceType {
		None,
		Mesh,
		Shader,
		Material_PBR,
		Material_Phong,
		Texture
	};

	class DENG_API ResourceEvent : public IEvent {
		private:
			cvar::hash_t m_hshResource;
			ResourceType m_eType;

		public:
			ResourceEvent(cvar::hash_t _hshResource, ResourceType _eType) :
				m_hshResource(_hshResource),
				m_eType(_eType) {}
			ResourceEvent(const ResourceEvent&) = default;

			inline cvar::hash_t GetResourceHash() {
				return m_hshResource;
			}

			inline ResourceType GetType() {
				return m_eType;
			}

			EVENT_CLASS_TYPE_NO_ID_CHECK("ResourceEvent", IEvent);
	};

	class DENG_API ResourceModifiedEvent : public ResourceEvent {
		public:
			ResourceModifiedEvent(cvar::hash_t _hshResource, ResourceType _eType) :
				ResourceEvent(_hshResource, _eType) {}
			ResourceModifiedEvent(const ResourceModifiedEvent&) = default;

			EVENT_CLASS_TYPE_NO_ID_CHECK("ResourceModifiedEvent", ResourceEvent);
	};

	class DENG_API ResourceAddedEvent : public ResourceEvent {
		public:
			ResourceAddedEvent(cvar::hash_t _hshResource, ResourceType _eType) :
				ResourceEvent(_hshResource, _eType) {}
			ResourceAddedEvent(const ResourceAddedEvent&) = default;

			EVENT_CLASS_TYPE_NO_ID_CHECK("ResourceAddedEvent", ResourceEvent);
	};

	class DENG_API ResourceRemoveEvent : public ResourceEvent {
		public:
			ResourceRemoveEvent(cvar::hash_t _hshResource, ResourceType _eType) :
				ResourceEvent(_hshResource, _eType) {}
			ResourceRemoveEvent(const ResourceRemoveEvent&) = default;
	
			EVENT_CLASS_TYPE_NO_ID_CHECK("ResourceRemoveEvent", ResourceEvent);
	};
}

#endif
