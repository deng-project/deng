// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: RuntimeExecutable.cpp - Main runtime binary implementation file
// author: Karl-Mihkel Ott

#define RUNTIME_EXECUTABLE_CPP
#include "deng/Executables/rt/RuntimeExecutable.h"

namespace DENG {
	namespace Runtime {

		GameRuntime::GameRuntime() : 
			m_conf_reader(Libdas::Algorithm::GetProgramPath() + "/game.xml") 
		{
			m_conf_reader.Parse();
			if (m_conf_reader.GetErrorBit()) {
				throw std::runtime_error(m_conf_reader.GetErrorMsg());
			}
			DXML::XMLGame &game = m_conf_reader.GetXMLGame();

			switch (game.cfg.default_backend) {
				case DXML::Configuration::VULKAN:
					std::cout << "Using vulkan runtime" << std::endl;
					break;

				case DXML::Configuration::OPENGL:
					std::cout << "Using opengl runtime" << std::endl;
					break;
			}
		}


		GameRuntime::~GameRuntime() {
			delete m_rend;
			delete m_window;
		}


		void GameRuntime::Run() {
			std::cout << "GameRuntime::Run() is not implemented :(" << std::endl;
		}
 	}
}


int main(void) {
	DENG::Runtime::GameRuntime rt;
	rt.Run();
}