// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VertexNormalVisualizer.h - Vertex normal visualizer class header
// author: Karl-Mihkel Ott

#ifndef VERTEX_NORMAL_VISUALIZER_H
#define VERTEX_NORMAL_VISUALIZER_H

#ifdef VERTEX_NORMAL_VISUALIZER_CPP
	#include <cstdint>
	#include <vector>
	#include <string>
	#include <array>
	#include <unordered_map>
	#include <queue>

#ifdef _WIN32
	#include <Windows.h>
#endif

	#include <python3.10/pyconfig.h>
	#include <pybind11/embed.h>

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
	#include "deng/ErrorDefinitions.h"
	#include "deng/Entity.h"
	#include "deng/ScriptableEntity.h"
	#include "deng/Registry.h"
	#include "deng/RenderState.h"
	#include "deng/ModelUniforms.h"
	#include "deng/ShaderDefinitions.h"
	#include "deng/Renderer.h"
	#include "deng/Camera3D.h"
	#include "deng/EditorCamera.h"
	#include "deng/GPUMemoryManager.h"
#endif

namespace DENG {

	struct NormalVisualizerUniform {
		TRS::Vector4<float> color = { 1.f, 0.f, 0.f, 1.f };
		TRS::Vector4<float> length = { 0.1f, 0.1f, 0.1f, 1.f };
	};

	class VertexNormalVisualizer : public Entity {
		private:
			static uint32_t m_indexed_shader_id;
			static uint32_t m_unindexed_shader_id;
			static uint32_t m_visualizer_ubo_offset;
			static NormalVisualizerUniform m_ubo;

			Renderer& m_renderer;
			uint32_t m_mesh_id = 0;
			std::vector<uint32_t> m_bound_framebuffers;

		private:
			uint32_t _CheckAndCreateShaderModule(bool _index);

		public:
			VertexNormalVisualizer(
				Entity* _parent,
				const std::string& _name,
				Renderer& _rend,
				uint32_t _vertex_offset,
				uint32_t _normal_offset,
				uint32_t _index_offset,
				uint32_t _draw_count,
				const std::vector<uint32_t>& _framebuffers = { 0 }
			);

			inline void Toggle(bool _enable) {
				m_renderer.GetMesh(m_mesh_id).enable = _enable;
			}

			static void SetColor(TRS::Vector4<float> _color);
			static void SetLength(float _len);
			static void Update(Renderer& _rend);
	};

}

#endif
