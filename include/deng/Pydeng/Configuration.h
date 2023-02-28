// Pydeng: DENG Python scripting API 
// licence: Apache, see LICENCE file
// file: Configuration.h - Configuration class header for pydeng
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
			inline void SetGraphicsBackend(DXML::GraphicsBackend _backend) {
				DENG::Registry* reg = DENG::Registry::GetInstance();
				reg->SetGraphicsBackend(_backend);
			}

			inline void SetMSAA(uint32_t _msaa) {
				DENG::Registry* reg = DENG::Registry::GetInstance();
				reg->SetMSAA(_msaa);
			}

			inline void SetCustomData(pybind11::dict& _dict) {
				//DENG::Registry* reg = DENG::Registry::GetInstance();
				//reg->SetCustomData(_dict);
			}

			inline void Save() {
				std::cout << "[Saving]" << std::endl;
			}
	};
}

#endif