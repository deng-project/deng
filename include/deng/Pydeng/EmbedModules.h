// Pydeng: DENG Python scripting API 
// licence: Apache, see LICENCE file
// file: EmbedModules.h - Declare all Pydeng embedded modules
// author: Karl-Mihkel Ott

#ifndef PYDENG_EMBED_MODULES_H
#define PYDENG_EMBED_MODULES_H

#include <python3.10/pyconfig.h>
#include <pybind11/embed.h>

#include "deng/deng.h"

namespace py = pybind11;

#include "deng/Pydeng/Collections.h"
#include "deng/Pydeng/DataQuery.h"
#include "deng/Pydeng/Configuration.h"
#include "deng/Pydeng/DynamicAssetLoader.h"
#include "deng/Pydeng/EventManager.h"
#include "deng/Pydeng/Framebuffer.h"
#include "deng/Pydeng/Networking.h"
#include "deng/Pydeng/SceneLoader.h"
#include "deng/Pydeng/Scriptable.h"
#include "deng/Pydeng/SoundPlayer.h"
#include "deng/Pydeng/VideoPlayer.h"


PYBIND11_EMBEDDED_MODULE(Pydeng, m) {
	m.doc() = "DENG Python api";

	// Pydeng.ComponentFinder
	py::class_<Pydeng::ComponentFinder>(m, "ComponentFinder")
		.def(py::init<>())
		.def("GetEntityById", &Pydeng::ComponentFinder::GetEntityById)
		.def("FindIdByName", &Pydeng::ComponentFinder::FindIdByName)
		.def("GetFirstEntityByType", &Pydeng::ComponentFinder::GetFirstEntityByType)
		.def("GetLastEntityByName", &Pydeng::ComponentFinder::GetLastEntityByType);

	// Pydeng.Configuration
	py::enum_<DXML::GraphicsBackend>(m, "GraphicsBackend")
		.value("GRAPHICS_BACKEND_VULKAN", DXML::GRAPHICS_BACKEND_VULKAN)
		.value("GRAPHICS_BACKEND_OPENGL", DXML::GRAPHICS_BACKEND_OPENGL);

	py::class_<Pydeng::Configuration>(m, "Configuration")
		.def(py::init<>())
		.def("SetGraphicsBackend", &Pydeng::Configuration::SetGraphicsBackend)
		.def("SetMSAA", &Pydeng::Configuration::SetMSAA)
		.def("SetCustomData", &Pydeng::Configuration::SetCustomData)
		.def("Save", &Pydeng::Configuration::Save);

	// Pydeng.Scriptable
	py::enum_<DENG::EntityType>(m, "EntityType")
		.value("ENTITY_TYPE_LEVEL", DENG::ENTITY_TYPE_LEVEL)
		.value("ENTITY_TYPE_EDITOR_CAMERA", DENG::ENTITY_TYPE_EDITOR_CAMERA)
		.value("ENTITY_TYPE_THIRD_PERSON_CAMERA", DENG::ENTITY_TYPE_THIRD_PERSON_CAMERA)
		.value("ENTITY_TYPE_FIRST_PERSON_CAMERA", DENG::ENTITY_TYPE_FIRST_PERSON_CAMERA)
		.value("ENTITY_TYPE_LIGHT_SOURCE_POINT", DENG::ENTITY_TYPE_LIGHT_SOURCE_POINT)
		.value("ENTITY_TYPE_LIGHT_SOURCE_DIRECTIONAL", DENG::ENTITY_TYPE_LIGHT_SOURCE_DIRECTIONAL)
		.value("ENTITY_TYPE_LIGHT_SOURCE_AMBIENT", DENG::ENTITY_TYPE_LIGHT_SOURCE_AMBIENT)
		.value("ENTITY_TYPE_MODEL", DENG::ENTITY_TYPE_MODEL)
		.value("ENTITY_TYPE_SCENE", DENG::ENTITY_TYPE_SCENE)
		.value("ENTITY_TYPE_NODE", DENG::ENTITY_TYPE_NODE)
		.value("ENTITY_TYPE_SKELETON", DENG::ENTITY_TYPE_SKELETON)
		.value("ENTITY_TYPE_SKELETON_JOINT", DENG::ENTITY_TYPE_SKELETON_JOINT)
		.value("ENTITY_TYPE_ANIMATION", DENG::ENTITY_TYPE_ANIMATION)
		.value("ENTITY_TYPE_ANIMATION_SAMPLER", DENG::ENTITY_TYPE_ANIMATION_SAMPLER)
		.value("ENTITY_TYPE_SCRIPT", DENG::ENTITY_TYPE_SCRIPT)
		.value("ENTITY_TYPE_MESH", DENG::ENTITY_TYPE_MESH)
		.value("ENTITY_TYPE_GRID_MESH", DENG::ENTITY_TYPE_GRID_MESH)
		.value("ENTITY_TYPE_SKYBOX", DENG::ENTITY_TYPE_SKYBOX)
		.value("ENTITY_TYPE_VERTEX_NORMAL_VISUALIZER", DENG::ENTITY_TYPE_VERTEX_NORMAL_VISUALIZER)
		.value("ENTITY_TYPE_UNKNOWN", DENG::ENTITY_TYPE_UNKNOWN);

	py::class_<Pydeng::Scriptable>(m, "Scriptable")
		.def(py::init<DENG::Entity*>())
		.def("GetName", &Pydeng::Scriptable::GetName)
		.def("GetId", &Pydeng::Scriptable::GetId)
		.def("GetAttachedBit", &Pydeng::Scriptable::GetAttachedBit)
		.def("GetType", &Pydeng::Scriptable::GetType);

	py::class_<Pydeng::CameraScriptable>(m, "CameraScriptable")
		.def(py::init<DENG::Entity*>())
		.def("EnableCamera", &Pydeng::CameraScriptable::EnableCamera)
		.def("DisableCamera", &Pydeng::CameraScriptable::DisableCamera)
		.def("ChangeFov", &Pydeng::CameraScriptable::ChangeFov);

	py::class_<Pydeng::EditorCameraScriptable>(m, "EditorCameraScriptable")
		.def(py::init<DENG::Entity*>())
		.def("ZoomIn", &Pydeng::EditorCameraScriptable::ZoomIn)
		.def("ZoomOut", &Pydeng::EditorCameraScriptable::ZoomOut)
		.def("SetZoomStep", &Pydeng::EditorCameraScriptable::SetZoomStep)
		.def("SetActionDelay", &Pydeng::EditorCameraScriptable::SetActionDelay)
		.def("SetRotationStep", &Pydeng::EditorCameraScriptable::SetRotationStep)
		.def("SetMouseRotationStep", &Pydeng::EditorCameraScriptable::SetMouseRotationStep)
		.def("SetOrigin", &Pydeng::EditorCameraScriptable::SetOrigin);

	py::class_<Pydeng::FirstPersonCameraScriptable>(m, "FirstPersonCameraScriptable")
		.def(py::init<DENG::Entity*>())
		.def("Move", &Pydeng::FirstPersonCameraScriptable::Move)
		.def("Rotate", &Pydeng::FirstPersonCameraScriptable::Rotate);
}

#endif