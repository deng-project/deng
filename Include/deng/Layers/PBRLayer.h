// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: PBRLayer.h - PBRLayer class header
// author: Karl-Mihkel Ott

#ifndef PBR_LAYER_H
#define PBR_LAYER_H

#include <array>
#include "deng/ILayer.h"
#include "deng/Scene.h"
#include "deng/WindowEvents.h"

#ifdef PBR_LAYER_CPP
#include "deng/ImGuiLayer.h"
#include "deng/SID.h"
#include "deng/ResourceIdTable.h"
#include "deng/Layers/PBRTable.h"
#include "deng/InitializerListBuilder.h"
#include "deng/FileTextureBuilder.h"
#include "deng/Layers/CameraScript.h"
#include "deng/Layers/PBRResourceBuilders.h"
#endif

namespace DENG {

	class PBRLayer : public ILayer {
		private:
			Entity m_idCamera = entt::null;
			Entity m_idDirLight = entt::null;
			Entity m_sphere;

			IRenderer* m_pRenderer = nullptr;
			Scene m_scene;

		public:
			PBRLayer(IRenderer* _pRenderer, IFramebuffer* _pFramebuffer);
			virtual void Attach(IRenderer* _pRenderer, IWindowContext* _pWindowContext) override;
			virtual void Update(IFramebuffer* _pFramebuffer) override;
	
			void OnImGuiDraw();
			bool OnWindowResizeEvent(WindowResizedEvent& _event);
	};
}

#endif
