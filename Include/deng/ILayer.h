// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: ILayer.h - layer interface header
// author: Karl-Mihkel Ott

#ifndef ILAYER_H
#define ILAYER_H

#include "deng/Api.h"
#include "deng/IFramebuffer.h"
#include "deng/IRenderer.h"
#include "deng/IWindowContext.h"
#include "deng/Event.h"

namespace DENG {

	class DENG_API ILayer {
		protected:
			IRenderer* m_pRenderer = nullptr;
			IWindowContext* m_pWindowContext = nullptr;

		public:
			virtual ~ILayer() {}
			virtual void Attach(IRenderer* _pRenderer, IWindowContext* _pWindowContext) = 0;
			virtual void Update(IFramebuffer* _pFramebuffer) = 0;
	};
}

#endif
