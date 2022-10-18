// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Skybox.h - Skybox generation class header
// author: Karl-Mihkel Ott

#ifndef SKYBOX_H
#define SKYBOX_H

#ifdef SKYBOX_CPP
	#include <string>
	#include <vector>
	#include <unordered_map>

#ifdef _WIN32
	#include <Windows.h>
#endif

	#include "trs/Points.h"
	#include "trs/Vector.h"
	#include "trs/Matrix.h"

	#include "deng/Api.h"
	#include "deng/Entity.h"
	#include <deng/ErrorDefinitions.h>
	#include "deng/ModelUniforms.h"
	#include "deng/ShaderDefinitions.h"
	#include "deng/Renderer.h"
	#include "deng/GPUMemoryManager.h"
#endif

namespace DENG {

	class DENG_API Skybox : public Entity {
		private:
            static const float m_skybox_vertices[];
			static const std::string m_name_prefix;
			static uint32_t m_skybox_counter;

			Renderer& m_renderer;
			uint32_t m_camera_offset = 0;
			const std::string m_framebuffer;

		public:
			Skybox(Entity *_parent, 
				   Renderer &_rend,
				   uint32_t _camera_ubo,
				   const std::string &_right,
				   const std::string &_left, 
				   const std::string &_top,
				   const std::string &_botton,
				   const std::string &_back,
				   const std::string &_front,
				   const std::string& _framebuffer = MAIN_FRAMEBUFFER_NAME);
			Skybox(Skybox&& _sb) = default;
			Skybox(const Skybox& _sb) = delete;
			void Attach();
	};

}

#endif