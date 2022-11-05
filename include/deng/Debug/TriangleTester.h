// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: TriangleTester.h - Basic visual triange generation class header
// author: Karl-Mihkel Ott

#ifndef TRIANGLE_TESTER_H
#define TRIANGLE_TESTER_H

#ifdef TRIANGLE_TESTER_CPP
	#include <array>
	#include <vector>
	#include <string>
	#include <unordered_map>
	
	#include <cstring>
	#include <cmath>

#ifdef _WIN32
	#include <Windows.h>
#endif

	#include <trs/Points.h>
	#include <trs/Vector.h>
	#include <trs/Matrix.h>
	#include <trs/Quaternion.h>

	#include "das/Api.h"
	#include "das/DasStructures.h"
	#include "das/TextureReader.h"

	#include "deng/Api.h"
	#include "deng/ErrorDefinitions.h"
	#include "deng/Entity.h"
	#include "deng/ShaderDefinitions.h"
	#include "deng/Renderer.h"
	#include "deng/GPUMemoryManager.h"

#define TEST_TRIANGLE_NAME	"__triangle_test__"
#endif

namespace DENG {

	class DENG_API TriangleTester : public Entity {
		private:
			Renderer& m_renderer;
			std::array<TRS::Point2D<float>, 3> m_vertices;
			const std::string m_texture_name;
			const std::string m_framebuffer;
			TRS::Point4D<float> m_color = { 1.0f, 0.f, 0.f, 1.f };
			uint32_t m_ubo_offset = 0;

		private:
			void _InitTexturedShaders(ShaderModule &_module);
			void _InitColoredShaders(ShaderModule& _module);

		public:
			TriangleTester(
				Entity* _parent, 
				Renderer &_rend, 
				std::array<TRS::Point2D<float>, 3> &_vertices, 
				const std::string &_texture = "",
				const std::string &_framebuffer = MAIN_FRAMEBUFFER_NAME);
			
			void Attach();
			void Update();
			
			inline void ChangeColor(TRS::Point4D<float> _color) {
				m_color = _color;
			}
	};
}

#endif