// Pydeng: DENG Python scripting API 
// licence: Apache, see LICENCE file
// file: EmbedModules.h - Declare all Pydeng embedded modules
// author: Karl-Mihkel Ott

#ifndef PYDENG_CONFIGURATION_H
#define PYDENG_CONFIGURATION_H

#ifdef PYDENG_CONFIGURATION_CPP
	#include "deng/deng.h"
	
	#include <python3.10/pyconfig.h>
	#include <pybind11/pybind11.h>
#endif

namespace Pydeng {
	class Configuration {
		public:
			Configuration() = default;
			void SetGraphicsBackend(DXML::GraphicsBackend);
			void SetMSAA(uint32_t _msaa);
			void SetCustomData(pybind11::dict &_dict);
			void Save();
	};
}

#endif