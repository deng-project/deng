// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: RuntimeExecutable.cpp - Main runtime binary implementation file
// author: Karl-Mihkel Ott

#define RUNTIME_EXECUTABLE_CPP
#include "deng/Executables/rt/RuntimeExecutable.h"


int main(void) {
	py::scoped_interpreter guard{};
	
	try {
		py::module_ calc = py::module_::import("Scripts.Calculator");
		calc.attr("Random")();
	} catch(py::error_already_set& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}