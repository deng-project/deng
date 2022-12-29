// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VertexNormalVisualizer.cpp - Vertex normal visualizer class implementation
// author: Karl-Mihkel Ott

#define VERTEX_NORMAL_VISUALIZER_CPP
#include "deng/VertexNormalVisualizer.h"

namespace DENG {

	uint32_t VertexNormalVisualizer::m_indexed_shader_id = UINT32_MAX;
	uint32_t VertexNormalVisualizer::m_unindexed_shader_id = UINT32_MAX;
	uint32_t VertexNormalVisualizer::m_visualizer_ubo_offset = UINT32_MAX;
	NormalVisualizerUniform VertexNormalVisualizer::m_ubo = NormalVisualizerUniform();

	VertexNormalVisualizer::VertexNormalVisualizer(
		Entity* _parent, 
		const std::string& _name, 
		Renderer& _rend,
		uint32_t _vertex_offset, 
		uint32_t _normal_offset,
		uint32_t _index_offset,
		uint32_t _draw_count,
		const std::vector<uint32_t>& _framebuffers
	) : Entity(_parent, _name, ENTITY_TYPE_VERTEX_NORMAL_VISUALIZER),
		m_renderer(_rend),
		m_bound_framebuffers(_framebuffers)
	{
		uint32_t id = _CheckAndCreateShaderModule(_index_offset != UINT32_MAX);
		MeshReference mesh;
		mesh.commands.emplace_back();
		mesh.commands.back().attribute_offsets.push_back(_vertex_offset);
		mesh.commands.back().attribute_offsets.push_back(_normal_offset);
		mesh.commands.back().draw_count = _draw_count;
		mesh.commands.back().indices_offset = _index_offset;
		mesh.enable = false;
		mesh.shader_module_id = id;
		mesh.framebuffer_ids = m_bound_framebuffers;

		m_mesh_id = m_renderer.PushMeshReference(mesh);
	}


	uint32_t VertexNormalVisualizer::_CheckAndCreateShaderModule(bool _index) {
		if ((_index && m_indexed_shader_id == UINT32_MAX) || (!_index && m_unindexed_shader_id == UINT32_MAX)) {
			ShaderModule module;
			module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT); // position vector
			module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT); // vertex normal vector
			module.attribute_strides.push_back(sizeof(TRS::Vector3<float>));
			module.attribute_strides.push_back(sizeof(TRS::Vector3<float>));
			

			// retrieve camera offset, camera instance type needs to be refactored
			Registry* reg = Registry::GetInstance();
			Camera3D* cam = (Camera3D*)reg->GetFirstEntityByType(ENTITY_TYPE_EDITOR_CAMERA);
			DENG_ASSERT(cam);

			module.enable_indexing = _index;
			// binding 0 (camera)
			module.ubo_data_layouts.push_back({
				{ 0, static_cast<uint32_t>(sizeof(ModelCameraUbo)), cam->GetUboOffset() },
				UNIFORM_DATA_TYPE_BUFFER,
				SHADER_STAGE_GEOMETRY,
				UNIFORM_USAGE_PER_SHADER
			});

			if (m_visualizer_ubo_offset == UINT32_MAX) {
				GPUMemoryManager* mem_man = GPUMemoryManager::GetInstance();
				m_visualizer_ubo_offset = mem_man->RequestUniformMemoryLocationF(m_renderer, static_cast<uint32_t>(sizeof(NormalVisualizerUniform)));
			}

			// binding 1 (color)
			module.ubo_data_layouts.push_back({
				{ 1, static_cast<uint32_t>(sizeof(NormalVisualizerUniform)), m_visualizer_ubo_offset },
				UNIFORM_DATA_TYPE_BUFFER,
				SHADER_STAGE_GEOMETRY,
				UNIFORM_USAGE_PER_SHADER
			});

			module.vertex_shader_src =
				"#version 450\n"\
				"#extension GL_ARB_separate_shader_objects : enable\n"\
				"layout(location = 0) in vec3 in_pos;\n"\
				"layout(location = 1) in vec3 in_norm;\n"\
				"layout(location = 0) out Vertex {\n"\
				"\tvec4 normal;\n"\
				"\tvec4 pos;\n"\
				"} vertex;\n"\
				"void main() {\n"\
				"\tvertex.normal = vec4(in_norm, 1.f);\n"\
				"\tvertex.pos = vec4(in_pos, 1.f);\n"\
				"}\n";

			
			RenderState* rs = RenderState::GetInstance();
			switch (rs->GetPrimary()) {
				case RENDERER_TYPE_VULKAN:
					module.geometry_shader_src =
						"#version 450\n"\
						"#extension GL_ARB_separate_shader_objects : enable\n"\
						"layout(triangles) in;\n"\
						"layout(line_strip, max_vertices=6) out;\n"\
						"\n"\
						"layout(location = 0) in Vertex {\n"\
						"\tvec4 normal;\n"\
						"\tvec4 pos;\n"\
						"} vertex[];\n"\
						"\n"\
						"layout(std140, set = 0, binding = 0) uniform CameraUbo {\n"\
						"\tmat4 projection;\n"\
						"\tmat4 view;\n"\
						"\tvec4 pos;\n"\
						"} camera;\n"\
						"\n"\
						"layout(std140, set = 0, binding = 1) uniform NormalUbo {\n"\
						"\tvec4 color;\n"
						"\tvec4 len;\n"\
						"} normal;\n"\
						"\n"\
						"const float normal_length = 0.1f;\n"\
						"layout(location = 0) out vec4 out_color;\n"\
						"void main() {\n"\
						"\tfor(int i = 0; i < vertex.length(); i++) {\n"\
						"\t\tvec3 p = vertex[i].pos.xyz;\n"\
						"\t\tvec3 n = normalize(vertex[i].normal.xyz);\n"\
						"\t\tgl_Position = camera.projection * camera.view * vec4(p, 1.f);\n"\
						"\t\tout_color = normal.color;\n"\
						"\t\tEmitVertex();\n"\
						"\n"\
						"\t\tgl_Position = camera.projection * camera.view * vec4(p + n * normal.len.x, 1.f);\n"\
						"\t\tout_color = normal.color;\n"\
						"\t\tEmitVertex();\n"\
						"\t\tEndPrimitive();\n"\
						"\t}\n"\
						"}\n";
					break;

					case RENDERER_TYPE_OPENGL:
						module.geometry_shader_src =
							"#version 450\n"\
							"#extension GL_ARB_separate_shader_objects : enable\n"\
							"layout(triangles) in;\n"\
							"layout(line_strip, max_vertices=6) out;\n"\
							"\n"\
							"layout(location = 0) in Vertex {\n"\
							"\tvec4 normal;\n"\
							"\tvec4 pos;\n"\
							"} vertex[];\n"\
							"\n"\
							"layout(std140, binding = 0) uniform CameraUbo {\n"\
							"\tmat4 projection;\n"\
							"\tmat4 view;\n"\
							"\tvec4 pos;\n"\
							"} camera;\n"\
							"\n"\
							"layout(std140, binding = 1) uniform NormalUbo {\n"\
							"\tvec4 color;\n"
							"\tvec4 len;\n"\
							"} normal;\n"\
							"\n"\
							"const float normal_length = 0.1f;\n"\
							"layout(location = 0) out vec4 out_color;\n"\
							"void main() {\n"\
							"\tfor(int i = 0; i < vertex.length(); i++) {\n"\
							"\t\tvec3 p = vertex[i].pos.xyz;\n"\
							"\t\tvec3 n = normalize(vertex[i].normal.xyz);\n"\
							"\t\tgl_Position = camera.projection * camera.view * vec4(p, 1.f);\n"\
							"\t\tout_color = normal.color;\n"\
							"\t\tEmitVertex();\n"\
							"\n"\
							"\t\tgl_Position = camera.projection * camera.view * vec4(p + n * normal.len.x, 1.f);\n"\
							"\t\tout_color = normal.color;\n"\
							"\t\tEmitVertex();\n"\
							"\t\tEndPrimitive();\n"\
							"\t}"\
							"}\n";
						break;

					default:
						break;
			}

			module.fragment_shader_src =
				"#version 450\n"\
				"#extension GL_ARB_separate_shader_objects : enable\n"\
				"layout(location = 0) in vec4 in_color;\n"\
				"layout(location = 0) out vec4 out_color;\n"\
				"void main() {\n"\
				"\tout_color = in_color;\n"\
				"}\n";

			module.load_shaders_from_file = false;
			module.enable_depth_testing = true;
			module.enable_indexing = _index;

			const uint32_t id = m_renderer.PushShaderModule(module);
			if (_index) m_indexed_shader_id = id;
			else m_unindexed_shader_id = id;

			return id;
		} else if (_index) {
			return m_indexed_shader_id;
		} else {
			return m_unindexed_shader_id;
		}
	}


	void VertexNormalVisualizer::SetColor(TRS::Vector4<float> _color) {
		m_ubo.color = _color;
	}

	void VertexNormalVisualizer::SetLength(float _len) {
		m_ubo.length = TRS::Vector4<float>(_len, _len, _len, 1.f);
	}

	void VertexNormalVisualizer::Update(Renderer& _rend) {
		DENG_ASSERT(m_visualizer_ubo_offset != UINT32_MAX);
		_rend.UpdateUniform(reinterpret_cast<const char*>(&m_ubo), static_cast<uint32_t>(sizeof(NormalVisualizerUniform)), m_visualizer_ubo_offset);
	}
}