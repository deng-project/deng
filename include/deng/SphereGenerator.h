// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SphereGenerator.h - Sphere generator class header
// author: Karl-Mihkel Ott

#ifndef SPHERE_GENERATOR_H
#define SPHERE_GENERATOR_H

#ifdef SPHERE_GENERATOR_CPP
	#include <vector>
	#include <array>
	#include <string>
	#include <unordered_map>
	#include <chrono>
	#include <algorithm>
	#include <fstream>
	#include <any>
	
	#ifdef _WIN32
	#include <Windows.h>
	#endif
	
		// trs
	#include "trs/Points.h"
	#include "trs/Vector.h"	
	#include "trs/Matrix.h"
	#include "trs/Quaternion.h"
	
	// mar
	#include "mar/AsciiStreamReader.h"
	#include "mar/AsciiLineReader.h"
	
	// libdas 
	#include "das/Api.h"
	#include "das/Hash.h"
	#include "das/ErrorHandlers.h"
	#include "das/DasStructures.h"
	#include "das/TextureReader.h"
	#include "das/DasWriterCore.h"
	#include "das/DasReaderCore.h"
	#include "das/DasParser.h"
	
	#include "deng/Api.h"
	#include "deng/Version.h"
	#include "deng/ErrorDefinitions.h"
	#include "deng/ModelUniforms.h"
	#include "deng/ShaderDefinitions.h"
	#include "deng/Entity.h"
	#include "deng/ScriptableEntity.h"
	#include "deng/Renderer.h"
	
	#include "deng/VertexNormalVisualizer.h"
	#include "deng/MeshLoader.h"
	#include "deng/AnimationSampler.h"
	#include "deng/SkeletonLoader.h"
	#include "deng/NodeLoader.h"
	#include "deng/SceneLoader.h"
	#include "deng/ModelLoader.h"

#endif

namespace DENG {
	
	class DENG_API SphereGenerator {
		private:
			const float x = .525731112119133606f;
			const float z = .850650808352039932f;
			const float n = 0.f;

			std::vector<TRS::Vector3<float>> m_vertices = {
				{-x,n,z}, {x,n,z}, {-x,n,-z}, {x,n,-z},
				{n,z,x}, {n,z,-x}, {n,-z,x}, {n,-z,-x},
				{z,x,n}, {-z,x, n}, {z,-x,n}, {-z,-x, n}
			};

			std::vector<TRS::Vector3<float>> m_normals;
			
			std::vector<TRS::Point3D<uint32_t>> m_indices = {
				{ 0,4,1 }, { 0,9,4 }, { 9,5,4 }, { 4,5,8 }, { 4,8,1 },
				{ 8,10,1 }, { 8,3,10 }, { 5,3,8 }, { 5,2,3 }, { 2,7,3 },
				{ 7,10,3 }, { 7,6,10 }, { 7,11,6 }, { 11,0,6 }, { 0,1,6 },
				{ 6,1,10 }, { 9,0,11 }, { 9,11,2 }, { 9,2,5 }, { 7,2,11 }
			};

			static uint32_t m_sphere_count;
			const std::string m_sphere_name = "Generated sphere " + std::to_string(m_sphere_count);

			bool m_use_normals;

		private:
			void _SubDivide(float _radius);
			void _GenerateNormals();

		public:
			SphereGenerator(float _radius, uint32_t _subdiv, bool _use_normals = false);
			ModelLoader ToModelLoader(Entity* _parent, Renderer& _rend, uint32_t _camera_id, const std::string& _mesh_name = "");
	};
}

#endif
