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
	#include <functional>
	#include <array>
	#include <queue>
	#include <variant>
	#include <stdexcept>

#ifdef _WIN32
	#include <Windows.h>
#endif
	
	#include <vulkan/vulkan.h>
	#include <python3.10/pyconfig.h>
	#include <pybind11/embed.h>

	#include "nwin/glad/glad.h"

	#include "trs/Points.h"
	#include "trs/Vector.h"
	#include "trs/Matrix.h"
	#include "trs/Quaternion.h"

	#include "das/Api.h"
	#include "das/DasStructures.h"
	#include "das/TextureReader.h"

	#include "dxml/GameStructs.h"

	#include "deng/Api.h"
	#include "deng/BaseTypes.h"
	#include "deng/Entity.h"
	#include "deng/ScriptableEntity.h"
	#include <deng/ErrorDefinitions.h>
	#include "deng/ModelUniforms.h"
	#include "deng/ShaderDefinitions.h"
	#include "deng/Renderer.h"
	#include "deng/GPUMemoryManager.h"
	#include "deng/Registry.h"
	#include "deng/Camera3D.h"
	#include "deng/TextureDatabase.h"
#endif

namespace DENG {

	class DENG_API Skybox : public Entity {
		private:
            static const float m_skybox_vertices[];
			static const std::string m_name_prefix;
			static uint32_t m_skybox_counter;

			Renderer& m_renderer;
			std::vector<uint32_t> m_framebuffer_ids;
			uint32_t m_camera_offset = 0;
			uint32_t m_texture_id;
			TRS::Vector4<float> m_ubo = { 1.f, 1.f, 1.f, 1.f };
			uint32_t m_ubo_offset = 0;

		public:
			Skybox(Entity *_parent, 
				   Renderer &_rend,
				   const std::string &_right,
				   const std::string &_left, 
				   const std::string &_top,
				   const std::string &_botton,
				   const std::string &_back,
				   const std::string &_front,
				   const std::vector<uint32_t>& _framebuffers = { 0 });
			Skybox(Skybox&& _sb) = default;
			Skybox(const Skybox& _sb) = delete;
			void Attach();
			void Update();
			void BindCamera(uint32_t _id);

			inline void SetScale(float _scale) {
				m_ubo = { _scale, _scale, _scale, 1.0f };
			}

			inline uint32_t GetTextureId() {
				return m_texture_id;
			}
	};

}

#endif