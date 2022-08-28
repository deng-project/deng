// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: deng.h - metaheader containing most deng includes
// author: Karl-Mihkel Ott

#ifndef DENG_H
#define DENG_H

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
#include "das/Algorithm.h"

// basics
#include "deng/Api.h"
#include "deng/Version.h"
#include "deng/Window.h"
#include "deng/BaseTypes.h"
#include "deng/ErrorDefinitions.h"
#include "deng/ShaderDefinitions.h"
#include "deng/ProjectDataManager.h"

// renderers
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

// python scripting
#include "deng/PythonScriptExecutor.h"

// registries
#include "deng/InputRegistry.h"
#include "deng/Registry.h"

#endif