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
#include "deng/Pydeng/SoundPlayer.h"
#include "deng/Pydeng/VideoPlayer.h"

PYBIND11_EMBEDDED_MODULE(Pydeng_DataQuery, m) {
	m.doc() = "Data query module";

	py::class_<Pydeng::ComponentFinder>(m, "ComponentFinder")
		.def(py::init<>())
		.def("GetEntityById", &Pydeng::ComponentFinder::GetEntityById)
		.def("FindIdByName", &Pydeng::ComponentFinder::FindIdByName)
		.def("GetFirstEntityByType", &Pydeng::ComponentFinder::GetFirstEntityByType)
		.def("GetLastEntityByName", &Pydeng::ComponentFinder::GetLastEntityByType);
}

PYBIND11_EMBEDDED_MODULE(Pydeng_Configuration, m) {
	m.doc() = "Renderer configuration setup module";
}

#endif