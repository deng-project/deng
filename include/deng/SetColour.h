// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SetColour.h - cross-platform color changer struct header
// author: Karl-Mihkel Ott

#ifndef SET_COLOUR_H
#define SET_COLOUR_H

namespace DENG {

	enum Colour {
		DARKBLUE = 1,
		DARKGREEN,
		DARKTEAL,
		DARKRED,
		DARKPINK,
		DARKYELLOW,
		GRAY,
		DARKGRAY,
		BLUE,
		GREEN,
		TEAL,
		RED,
		PINK,
		YELLOW,
		WHITE
	};

#ifdef _WIN32
#include <iostream>
#include <windows.h>
#include <iomanip>

	struct SetColour {
		Colour c;

		SetColour(Colour _c) :
			c(_c) {}
	};
}

namespace std {
	basic_ostream<char>& operator<<(basic_ostream<char>& s, const DENG::SetColour& ref) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ref.c);
		return s;
	}
}

#endif

#endif