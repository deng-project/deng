// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: CubeGenerator.h - Cube generator class headers
// author: Karl-Mihkel Ott

#ifndef CUBE_GENERATOR_H
#define CUBE_GENERATOR_H

#ifdef CUBE_GENERATOR_CPP
	#include <vector>
	#include <array>
	#include <string>
	#include <unordered_map>
	#include <chrono>
	#include <fstream>
	#include <any>

#ifdef _WIN32
	#include <Windows.h>
#endif

	#include "trs/Points.h"
	#include "trs/Vector.h"	
	#include "trs/Matrix.h"
	#include "trs/Quaternion.h"

	// mar
	#include "mar/AsciiStreamReader.h"
	#include "mar/AsciiLineReader.h"

	// libdas 
	#include "das/Api.h"
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

	#include "deng/MeshLoader.h"
	#include "deng/AnimationSampler.h"
	#include "deng/SkeletonLoader.h"
	#include "deng/NodeLoader.h"
	#include "deng/SceneLoader.h"
	#include "deng/ModelLoader.h"
#endif

namespace DENG {

	class DENG_API CubeGenerator {
		private:
			std::array<TRS::Point3D<float>, 36> m_vertices;
			std::array<TRS::Point3D<float>, 36> m_normals;

			bool m_normal;
			
			static uint32_t m_cube_count;
			const std::string m_cube_name = "Generated cube " + std::to_string(m_cube_count);

		public:
			CubeGenerator(TRS::Point3D<float> _min, TRS::Point3D<float> _max, bool _normals = false);
			ModelLoader ToModelLoader(Entity *_parent, Renderer &_rend, uint32_t _camera_id, const std::string &_file_name, const std::string &_mesh_name = "");
	};
}

#endif