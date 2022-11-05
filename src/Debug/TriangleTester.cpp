// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: TriangleTester.h - Basic visual triange generation class header
// author: Karl-Mihkel Ott

#define TRIANGLE_TESTER_CPP
#include "deng/Debug/TriangleTester.h"

namespace DENG {
	
	TriangleTester::TriangleTester(
		Entity* _parent, 
		Renderer& _rend, 
		std::array<TRS::Point2D<float>, 3>& _vertices, 
		const std::string& _texture,
		const std::string &_framebuffer
	) :
		Entity(_parent, TEST_TRIANGLE_NAME, ENTITY_TYPE_TRIANGLE_TESTER),
		m_renderer(_rend),
		m_vertices(_vertices),
		m_texture_name(_texture),
		m_framebuffer(_framebuffer) {}


	void TriangleTester::_InitTexturedShaders(ShaderModule& _module) {
		_module.vertex_shader_src =
			"#version 450\n"\
			"#extension GL_ARB_separate_shader_objects : enable\n"\
			"layout(location = 0) in vec2 i_pos;\n"\
			"layout(location = 0) out vec2 o_uv;\n"\
			"void main() {\n"\
			"	gl_Position = vec4(i_pos, 0.0f, 1.0f);\n"\
			"	o_uv = (i_pos + 1.0f) / 2.f;\n"\
			"}";

		_module.fragment_shader_src =
			"#version 450\n"\
			"#extension GL_ARB_separate_shader_objects : enable\n"\
			"layout(location = 0) in vec2 i_uv;\n"\
			"layout(binding = 0) uniform sampler2D ts;\n"\
			"layout(location = 0) out vec4 color;\n"\
			"void main() {\n"\
			"	color = texture(ts, i_uv);\n"\
			"}";

		_module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC2_FLOAT);
		_module.attribute_strides.push_back(sizeof(float) * 2);
		_module.enable_scissor = false;
		_module.enable_depth_testing = false;
		_module.enable_2d_textures = true;
		_module.load_shaders_from_file = false;

		_module.ubo_data_layouts.emplace_back();
		_module.ubo_data_layouts.back().block.binding = 0;
		_module.ubo_data_layouts.back().stage = SHADER_STAGE_FRAGMENT;
		_module.ubo_data_layouts.back().type = DENG::UNIFORM_DATA_TYPE_2D_IMAGE_SAMPLER;
		_module.ubo_data_layouts.back().usage = DENG::UNIFORM_USAGE_PER_SHADER;

	}


	void TriangleTester::_InitColoredShaders(ShaderModule& _module) {
		_module.vertex_shader_src =
			"#version 450\n"\
			"#extension GL_ARB_separate_shader_objects : enable\n"\
			"layout(location = 0) in vec2 i_pos;\n"\
			"void main() {\n"\
			"	gl_Position = vec4(i_pos, 0.1f, 1.f);\n"\
			"}";

		_module.fragment_shader_src =
			"#version 450\n"\
			"#extension GL_ARB_separate_shader_objects : enable\n"\
			"layout(location = 0) out vec4 o_color;\n"\
			"layout(binding = 0) uniform ColorData { vec4 color; } cl_data;\n"\
			"void main() {\n"\
			"	o_color = cl_data.color;\n"\
			"}";

		_module.attributes.push_back(DENG::ATTRIBUTE_TYPE_VEC2_FLOAT);
		_module.attribute_strides.push_back(sizeof(float) * 2);
		_module.enable_scissor = false;
		_module.enable_depth_testing = false;
		_module.enable_2d_textures = false;
		_module.load_shaders_from_file = false;
		_module.enable_indexing = false;

		// NOTE per mesh ubo data concept is broken in vulkan implementation
		_module.ubo_data_layouts.emplace_back();
		_module.ubo_data_layouts.back().block.binding = 0;
		_module.ubo_data_layouts.back().block.size = sizeof(TRS::Point4D<float>);
		_module.ubo_data_layouts.back().stage = SHADER_STAGE_FRAGMENT;
		_module.ubo_data_layouts.back().type = DENG::UNIFORM_DATA_TYPE_BUFFER;
		_module.ubo_data_layouts.back().usage = DENG::UNIFORM_USAGE_PER_SHADER;
	}


	void TriangleTester::Attach() {
		ShaderModule module;
		if (m_texture_name == "")
			_InitColoredShaders(module);
		else _InitTexturedShaders(module);

		const uint32_t shader_id = m_renderer.PushShader(module);
		const uint32_t mesh_id = m_renderer.NewMeshReference(m_framebuffer);
	
		GPUMemoryManager* mem_man = GPUMemoryManager::GetInstance();
		uint32_t offset = mem_man->RequestMainMemoryLocationF((uint32_t)sizeof(float), 3u * (uint32_t)sizeof(TRS::Point2D<float>));
		m_renderer.UpdateVertexDataBuffer(std::make_pair(reinterpret_cast<const char*>(m_vertices.data()), (uint32_t)(3u * sizeof(TRS::Point2D<float>))), offset);

		auto& mesh = m_renderer.GetMesh(mesh_id, m_framebuffer);
		mesh.name = TEST_TRIANGLE_NAME;
		mesh.shader_module_id = shader_id;
		mesh.commands.emplace_back();
		mesh.commands.back().draw_count = 3;
		mesh.commands.back().attribute_offsets.push_back(offset);
		
		if (m_texture_name.empty()) {
			m_ubo_offset = mem_man->RequestUniformMemoryLocationF(m_renderer, sizeof(TRS::Point4D<float>));
			m_renderer.UpdateUniform(reinterpret_cast<const char*>(&m_color), sizeof(TRS::Point4D<float>), m_ubo_offset);
		} else {
			mesh.commands.back().texture_names.push_back(m_texture_name);
		}

		SetAttachedBit(true);
	}


	void TriangleTester::Update() {
		if (m_texture_name.empty()) {
			m_renderer.UpdateUniform(reinterpret_cast<const char*>(&m_color), sizeof(TRS::Point4D<float>), m_ubo_offset);
		}
	}
}