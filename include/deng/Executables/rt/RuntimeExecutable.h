// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: RuntimeExecutable.h - Main runtime binary header file
// author: Karl-Mihkel Ott

#ifndef RUNTIME_EXECUTABLE_H
#define RUNTIME_EXECUTABLE_H

#ifdef RUNTIME_EXECUTABLE_CPP
	#include <algorithm>
	#include <iostream>
	#include <filesystem>
	#include "deng/Pydeng/EmbedModules.h"
	#include "deng/deng.h"
	#include <pybind11/stl_bind.h>


	PYBIND11_MAKE_OPAQUE(std::vector<std::string>);

#ifdef _DEBUG
	#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
#else
	#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

#endif

std::string GetProgramPath(bool _no_filename = false);
void SetupSysPath();
int DisplayErrorBox(const std::string &_msg);


struct LevelState {
	DENG::ModelLoader* player = nullptr;
	DENG::Skybox *skybox = nullptr;
	std::list<DENG::Camera3D> cameras;
	std::list<DENG::ModelLoader> models;
	uint32_t id = 0;
};


class PythonScript {
	private:
		py::object m_class_obj;

	public:
		PythonScript(const py::object &_obj) : m_class_obj(_obj) {}

		void OnAttach() {
			m_class_obj.attr("OnAttach")();
		}

		void OnUpdate() {
			m_class_obj.attr("OnUpdate")();
		}

		void OnDestroy() {
			m_class_obj.attr("OnDestroy")();
		}
};


class Runtime {
	private:
		LevelState m_level;
		DENG::ProjectDataManager* m_data = nullptr;
		DENG::Window *m_window = nullptr;
		DENG::Renderer *m_renderer = nullptr;
		
	private:
		void _LoadLevel(uint32_t _id);

	public:
		Runtime();
		~Runtime();

		void Run();

};

#endif