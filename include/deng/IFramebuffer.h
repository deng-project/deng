// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IFramebuffer.h - header for generic framebuffer interface
// author: Karl-Mihkel Ott

#ifndef IFRAMEBUFFER_H
#define IFRAMEBUFFER_H

#include <cstdint>
#include "trs/Vector.h"
#include "deng/Api.h"

namespace DENG {

	class DENG_API IFramebuffer {
		protected:
			uint32_t m_uWidth = 0;
			uint32_t m_uHeight = 0;

		public:
			IFramebuffer() = default;
			IFramebuffer(const IFramebuffer&) = default;
			IFramebuffer(uint32_t _uWidth, uint32_t _uHeight) :
				m_uWidth(_uWidth),
				m_uHeight(_uHeight) {}

			virtual ~IFramebuffer() {};

			virtual void BeginCommandBufferRecording(TRS::Vector4<float> _vClearColor) = 0;
			virtual void EndCommandBufferRecording() = 0;
			virtual void RenderToFramebuffer() = 0;

			inline void SetWidth(uint32_t _uWidth) {
				m_uWidth = _uWidth;
			}

			inline void SetHeight(uint32_t _uHeight) {
				m_uHeight = _uHeight;
			}

			inline uint32_t GetWidth() {
				return m_uWidth;
			}

			inline uint32_t GetHeight() {
				return m_uHeight;
			}
	};
}

#endif
