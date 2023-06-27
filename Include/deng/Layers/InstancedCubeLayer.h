// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: InstancedCubeLayer.h - Instance cube layer class header
// author: Karl-Mihkel Ott

#ifndef INSTANCED_CUBE_LAYER_H
#define INSTANCED_CUBE_LAYER_H

#include <array>
#include "deng/ILayer.h"
#include "deng/IRenderer.h"
#include "deng/IWindowContext.h"
#include "deng/WindowEvents.h"
#include "deng/Scene.h"

#ifdef INSTANCED_CUBE_LAYER_CPP
#include "deng/ImGuiLayer.h"
#include "deng/ResourceIdTable.h"
#include "deng/BuiltInTables.h"
#include "deng/Exceptions.h"
#include "deng/Layers/CameraScript.h"
#include "deng/Layers/CubeVertices.h"
#include "CubeResourceBuilders.h"
#endif

#define SQ(x) (x*x)
#define ROW_LEN	316

namespace DENG {

	class InstancedCubeLayer : public ILayer {
		private:
			Scene m_scene;
			IRenderer* m_pRenderer = nullptr;
			Entity m_prefabEntity = entt::null;

			std::array<Entity, SQ(ROW_LEN)> m_entities;

		private:
			void _ApplyTransforms();

		public:
			InstancedCubeLayer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer);
			virtual void Attach(IRenderer* _pRenderer, IWindowContext* _pWindowContext) override;
			virtual void Update(IFramebuffer* _pFramebuffer) override;

			bool OnWindowResizedEvent(WindowResizedEvent& _event);
			void OnImGuiDraw();
	};
}

#endif