// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: CubeGenerator.h - Editor cube generator header
// author: Karl-Mihkel Ott

#ifndef CUBE_GENERATOR_H
#define CUBE_GENERATOR_H

#ifdef CUBE_GENERATOR_CPP
	#include <wx/wx.h>

	#include "deng/deng.h"
#endif

namespace DENG {
	namespace Editor {

		class CubeGenerator : public wxDialog {
			private:
				TRS::Vector3<float> max;
				TRS::Vector3<float> min;

			public:
				CubeGenerator(wxWindow* _parent);
		};
	}
}

#endif