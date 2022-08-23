// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLLoader.h - Universal OpenGL loader class interface
// author: Karl-Mihkel Ott

#ifndef OPENGL_LOADER_H
#define OPENGL_LOADER_H

namespace DENG {
	namespace Editor {

		class OpenGLLoader {
			public:
				virtual void CreateContext(std::any _hwnd) = 0;
				virtual void MakeCurrent() = 0;
				virtual void DeleteContext() = 0;
		};
	}
}

#endif