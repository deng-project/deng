// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SphereGenerator.h - Sphere generator class implementation
// author: Karl-Mihkel Ott

#define SPHERE_GENERATOR_CPP
#include "deng/SphereGenerator.h"

namespace DENG {

	uint32_t SphereGenerator::m_sphere_count = 0;

	SphereGenerator::SphereGenerator(float _radius, uint32_t _subdiv) {
		// initial step resize the icosahedron according to the given diameter
		for (auto it = m_vertices.begin(); it != m_vertices.end(); it++) {
			it->Normalise();
			*it = *it * _radius;
		}

		// subdivide into more sphere looking mesh

		m_vertices.reserve(m_vertices.size() * (size_t) std::pow(2., (double) _subdiv));
		m_indices.reserve(m_indices.size() * (size_t) std::pow(4., (double) _subdiv));
		for (uint32_t i = 0; i < _subdiv; i++)
			_SubDivide(_radius);
	}


	void SphereGenerator::_SubDivide(float _diameter) {
		const size_t old_size = m_indices.size();
		std::unordered_map<TRS::Vector3<float>, uint32_t, Libdas::Hash<TRS::Vector3<float>>> redundancy_map;
		redundancy_map.reserve(m_vertices.size());

		for (size_t i = 0; i < old_size; i++) {
			// edges: v1 -> v2; v1 -> v3; v2 -> v3
			auto v1 = (m_vertices[m_indices[i].x] + m_vertices[m_indices[i].y]) / 2.f;
			auto v2 = (m_vertices[m_indices[i].x] + m_vertices[m_indices[i].z]) / 2.f;
			auto v3 = (m_vertices[m_indices[i].y] + m_vertices[m_indices[i].z]) / 2.f;
			
			v1.Normalise();
			v1 *= _diameter;
			v2.Normalise();
			v2 *= _diameter;
			v3.Normalise();
			v3 *= _diameter;

			// v1 already exists
			uint32_t ix = 0, iy = 0, iz = 0;
			if (redundancy_map.find(v1) != redundancy_map.end()) {
				ix = redundancy_map[v1];
			} else {
				m_vertices.push_back(v1);
				ix = static_cast<uint32_t>(m_vertices.size() - 1);
				redundancy_map[v1] = ix;
			}

			// v2 already exists
			if (redundancy_map.find(v2) != redundancy_map.end()) {
				iy = redundancy_map[v2];
			} else {
				m_vertices.push_back(v2);
				iy = static_cast<uint32_t>(m_vertices.size() - 1);
				redundancy_map[v2] = iy;
			}

			// v3 already exists
			if (redundancy_map.find(v3) != redundancy_map.end()) {
				iz = redundancy_map[v3];
			} else {
				m_vertices.push_back(v3);
				iz = static_cast<uint32_t>(m_vertices.size() - 1);
				redundancy_map[v3] = iz;
			}

			const uint32_t oy = m_indices[i].y;
			const uint32_t oz = m_indices[i].z;
			m_indices[i].y = ix;
			m_indices[i].z = iy;

			m_indices.emplace_back(iz, iy, ix);
			m_indices.emplace_back(iy, iz, oz);
			m_indices.emplace_back(ix, oy, iz);
		}
		std::cout << "Sphere generation successful" << std::endl;
	}


	ModelLoader SphereGenerator::ToModelLoader(Entity* _parent, Renderer& _rend, uint32_t _camera_id, const std::string& _file_name, const std::string& _mesh_name) {
		const std::string* p_name = nullptr;
		if (_mesh_name == "")
			p_name = &m_sphere_name;
		else p_name = &_mesh_name;

		{
			Libdas::DasWriterCore writer(_file_name);
			
			Libdas::DasProperties props;
			props.author = "DENG v" + std::to_string(DENG_VERSION_MAJOR) + '.' + std::to_string(DENG_VERSION_MINOR) + '.' + std::to_string(DENG_VERSION_REVISION);
			props.default_scene = 0;
			props.moddate = static_cast<uint64_t>(time(NULL));
			props.model = "SPHERE";
			writer.InitialiseFile(props);

			Libdas::DasBuffer buffer;
			buffer.data_len = static_cast<uint32_t>(m_vertices.size() * sizeof(TRS::Vector3<float>) + m_indices.size() * sizeof(TRS::Point3D<uint32_t>));
			buffer.data_ptrs.push_back(std::make_pair(reinterpret_cast<char*>(m_vertices.data()), m_vertices.size() * sizeof(TRS::Vector3<float>)));
			buffer.data_ptrs.push_back(std::make_pair(reinterpret_cast<char*>(m_indices.data()), m_indices.size() * sizeof(TRS::Point3D<uint32_t>)));
			buffer.type = LIBDAS_BUFFER_TYPE_VERTEX | LIBDAS_BUFFER_TYPE_INDICES;
			writer.WriteBuffer(buffer);

			Libdas::DasScene scene;
			scene.name = *p_name + " scene";
			scene.node_count = 1;
			scene.nodes = new uint32_t[1];
			*scene.nodes = 0;
			writer.WriteScene(scene);

			Libdas::DasNode node;
			node.name = *p_name + " node";
			node.children_count = 0;
			node.mesh = 0;
			writer.WriteNode(node);

			Libdas::DasMesh mesh;
			mesh.name = *p_name;
			mesh.primitive_count = 1;
			mesh.primitives = new uint32_t[1];
			*mesh.primitives = 0;
			writer.WriteMesh(mesh);

			Libdas::DasMeshPrimitive prim;
			prim.vertex_buffer_id = 0;
			prim.vertex_buffer_offset = 0;
			prim.indices_count = static_cast<uint32_t>(m_indices.size() * 3);
			prim.index_buffer_id = 0;
			prim.index_buffer_offset = static_cast<uint32_t>(m_vertices.size() * sizeof(TRS::Vector3<float>));
			writer.WriteMeshPrimitive(prim);
		}

		return ModelLoader(_parent, _file_name, _rend, _camera_id);
	}

}