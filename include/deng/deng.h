// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: deng.h - metaheader containing most deng includes
// author: Karl-Mihkel Ott

#ifndef DENG_H
#define DENG_H

// std includes
#include <any>
#include <string>
#include <unordered_map>
#include <chrono>
#include <variant>
#include <fstream>
#include <stack>
#include <array>

// platform specific includes
#ifdef _WIN32
	#include <wtypes.h>
#endif
#include <vulkan/vulkan.h>

// third party (TRS)
#include "trs/Iterators.h"
#include "trs/Points.h"
#include "trs/Vector.h"
#include "trs/Quaternion.h"
#include "trs/Matrix.h"

// third party (mar)
#include "mar/AsciiStreamReader.h"
#include "mar/AsciiLineReader.h"

// third party (DXML)
#include "dxml/GameStructs.h"
#include "dxml/XMLTag.h"
#include "dxml/XMLWriter.h"
#include "dxml/XMLParser.h"
#include "dxml/GameConfigurationReader.h"
#include "dxml/GameConfigurationWriter.h"

// third party (libdas)
#include "das/Api.h"
#include "das/Hash.h"
#include "das/ErrorHandlers.h"
#include "das/Algorithm.h"
#include "das/DasStructures.h"
#include "das/DasReaderCore.h"
#include "das/DasParser.h"
#include "das/TextureReader.h"

// basics
#include "deng/Api.h"
#include "deng/Version.h"
#include "deng/Window.h"
#include "deng/BaseTypes.h"
#include "deng/ErrorDefinitions.h"
#include "deng/ShaderDefinitions.h"
#include "deng/ProjectDataManager.h"
#include "deng/Missing.h"

// renderers
#include "deng/RenderState.h"
#include "deng/Renderer.h"
#include "deng/OpenGLRenderer.h"
#include "deng/VulkanRenderer.h"

// entities
#include "deng/ModelUniforms.h"
#include "deng/Entity.h"
#include "deng/ScriptableEntity.h"
#include "deng/GridGenerator.h"
#include "deng/Camera3D.h"
#include "deng/FirstPersonCamera.h"
#include "deng/EditorCamera.h"
#include "deng/VertexNormalVisualizer.h"
#include "deng/AnimationSampler.h"
#include "deng/MeshLoader.h"
#include "deng/SkeletonLoader.h"
#include "deng/NodeLoader.h"
#include "deng/SceneLoader.h"
#include "deng/ModelLoader.h"
#include "deng/Skybox.h"

// debug entities
#ifdef __DEBUG
	#include "deng/Debug/TriangleTester.h"
#endif

// python scripting
#include "deng/PythonScriptExecutor.h"

// generators
#include "deng/CubeGenerator.h"
#include "deng/SphereGenerator.h"

// registries
#include "deng/InputRegistry.h"
#include "deng/Registry.h"

#endif