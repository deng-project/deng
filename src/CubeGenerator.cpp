// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: CubeGenerator.h - Cube generator class headers
// author: Karl-Mihkel Ott

#define CUBE_GENERATOR_CPP
#include "deng/CubeGenerator.h"


namespace DENG {

	uint32_t CubeGenerator::m_cube_count = 0;
	
	CubeGenerator::CubeGenerator(TRS::Point3D<float> _min, TRS::Point3D<float> _max, bool _normals) :
		m_normal(_normals) 
	{
		m_cube_count++;
		// quad 1
		std::array<TRS::Point3D<float>, 8> base;
		base[0] = TRS::Point3D<float>(_min.x, _min.y, _min.z);
		base[1] = TRS::Point3D<float>(_max.x, _min.y, _min.z);
		base[2] = TRS::Point3D<float>(_min.x, _min.y, _max.z);
		base[3] = TRS::Point3D<float>(_max.x, _min.y, _max.z);

		// quad 2
		base[4] = TRS::Point3D<float>(_min.x, _max.y, _min.z);
		base[5] = TRS::Point3D<float>(_max.x, _max.y, _min.z);
		base[6] = TRS::Point3D<float>(_min.x, _max.y, _max.z);
		base[7] = TRS::Point3D<float>(_max.x, _max.y, _max.z);

		// might be replaced with a convex hull algorithm later
		// bottom
		m_vertices[0] = base[2];
		m_vertices[1] = base[0];
		m_vertices[2] = base[1];
		
		m_vertices[3] = base[1];
		m_vertices[4] = base[3];
		m_vertices[5] = base[2];
		
		// back
		m_vertices[6] = base[2];
		m_vertices[7] = base[6];
		m_vertices[8] = base[7];
		
		m_vertices[9] = base[7];
		m_vertices[10] = base[3];
		m_vertices[11] = base[2];
		
		// left
		m_vertices[12] = base[0];
		m_vertices[13] = base[4];
		m_vertices[14] = base[6];
		
		m_vertices[15] = base[6];
		m_vertices[16] = base[2];
		m_vertices[17] = base[0];

		// front
		m_vertices[18] = base[1];
		m_vertices[19] = base[5];
		m_vertices[20] = base[4];

		m_vertices[21] = base[4];
		m_vertices[22] = base[0];
		m_vertices[23] = base[1];

		// right
		m_vertices[24] = base[1];
		m_vertices[25] = base[5];
		m_vertices[26] = base[7];
		
		m_vertices[27] = base[7];
		m_vertices[28] = base[3];
		m_vertices[29] = base[1];

		// top
		m_vertices[30] = base[6];
		m_vertices[31] = base[4];
		m_vertices[32] = base[5];
		
		m_vertices[33] = base[5];
		m_vertices[34] = base[7];
		m_vertices[35] = base[6];

		if (_normals) {
			for (int i = 0; i < 6; i++) {
				m_normals[i] = TRS::Point3D(0.f, -1.f, 0.f);
				m_normals[i + 6] = TRS::Point3D(0.f, 0.f, 1.f);
				m_normals[i + 6 * 2] = TRS::Point3D(-1.f, 0.f, 0.f);
				m_normals[i + 6 * 3] = TRS::Point3D(0.f, 0.f, -1.f);
				m_normals[i + 6 * 4] = TRS::Point3D(1.f, 0.f, 0.f);
				m_normals[i + 6 * 5] = TRS::Point3D(0.f, 1.f, 0.f);
			}
		}
	}

	ModelLoader CubeGenerator::ToModelLoader(Entity *_parent, Renderer &_rend, uint32_t _camera_id, const std::string &_mesh_name) {
		// NOTE: There is a bug regarding the buffer deletion after the cube is successfully loaded from cached file
		// See DasParser.cpp:195
		const std::string* p_name = nullptr;
		if (_mesh_name == "")
			p_name = &m_cube_name;
		else p_name = &_mesh_name;

		Libdas::DasModel model;
		model.props.author = "DENG v" + std::to_string(DENG_VERSION_MAJOR) + '.' + std::to_string(DENG_VERSION_MINOR) + '.' + std::to_string(DENG_VERSION_REVISION);
		model.props.moddate = static_cast<uint64_t>(time(nullptr));
		model.props.model = *p_name;
		model.props.default_scene = 0;

		// buffer
		model.buffers.emplace_back();
		model.buffers.back().data_len = static_cast<uint32_t>(m_vertices.size() * sizeof(TRS::Point3D<float>));
		model.buffers.back().data_ptrs.push_back(std::make_pair(reinterpret_cast<char*>(m_vertices.data()), m_vertices.size() * sizeof(TRS::Point3D<float>)));
		model.buffers.back().type = LIBDAS_BUFFER_TYPE_VERTEX;
		model.buffers.back()._free_bit = false;

		if (m_normal) {
			model.buffers.back().data_len += static_cast<uint32_t>(m_normals.size() * sizeof(TRS::Point3D<float>));
			model.buffers.back().data_ptrs.push_back(std::make_pair(reinterpret_cast<char*>(m_normals.data()), m_normals.size() * sizeof(TRS::Point3D<float>)));
			model.buffers.back().type |= LIBDAS_BUFFER_TYPE_VERTEX_NORMAL;
		}

		// scene
		model.scenes.emplace_back();
		model.scenes.back().name = (*p_name) + " scene";
		model.scenes.back().node_count = 1;
		model.scenes.back().nodes = new uint32_t[1];
		*model.scenes.back().nodes = 0;

		// node
		model.nodes.emplace_back();
		model.nodes.back().name = (*p_name) + " node";
		model.nodes.back().children_count = 0;
		model.nodes.back().mesh = 0;

		// mesh
		model.meshes.emplace_back();
		model.meshes.back().name = (*p_name);
		model.meshes.back().primitive_count = 1;
		model.meshes.back().primitives = new uint32_t[1];
		*model.meshes.back().primitives = 0;

		// mesh primitive
		model.mesh_primitives.emplace_back();
		model.mesh_primitives.back().vertex_buffer_id = 0;
		model.mesh_primitives.back().vertex_buffer_offset = 0;

		if (m_normal) {
			model.mesh_primitives.back().vertex_normal_buffer_id = 0;
			model.mesh_primitives.back().vertex_normal_buffer_offset = static_cast<uint32_t>(sizeof(TRS::Point3D<float>) * m_vertices.size());
		}
		model.mesh_primitives.back().draw_count = 36;

		return ModelLoader(_parent, model, _rend, _camera_id);
	}
}