// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: BulletPhysicsDemo.h - header of layer to demonstrate the power of bullet3 engine :)
// author: Karl-Mihkel Ott

#pragma once

#include <chrono>
#include "deng/ILayer.h"
#include "deng/WindowEvents.h"
#include "deng/Scene.h"

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

namespace DENG {

	class BulletFallingCubeLayer : public ILayer {
		private:
			Scene m_scene;

			Entity m_entCamera = entt::null;
			Entity m_entCube = entt::null;
			Entity m_entPlane = entt::null;
			Entity m_entDirectionalLight = entt::null;

			std::chrono::time_point<std::chrono::high_resolution_clock> m_beginTimePoint =
				std::chrono::high_resolution_clock::now();
			std::chrono::time_point<std::chrono::high_resolution_clock> m_endTimePoint =
				std::chrono::high_resolution_clock::now();

			btDefaultCollisionConfiguration* m_pCollisionConfiguration = nullptr;
			btCollisionDispatcher* m_pDispatcher = nullptr;
			btBroadphaseInterface* m_pOverlappingPairCache = nullptr;
			btSequentialImpulseConstraintSolver* m_pSolver = nullptr;
			btDiscreteDynamicsWorld* m_pDynamicsWorld = nullptr;
			btAlignedObjectArray<btCollisionShape*> m_collisionShapes;

		private:
			void _CreateRigidBodies();

		public:
			BulletFallingCubeLayer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer);
			~BulletFallingCubeLayer();
			virtual void Attach(IRenderer* _pRenderer, IWindowContext* _pWindowContext) override;
			virtual void Update(IFramebuffer* _pFramebuffer) override;

			bool OnWindowResizedEvent(WindowResizedEvent& _event);
	};
}