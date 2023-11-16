// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: Strings.h - string manipulation utility methods
// author: Karl-Mihkel Ott

#pragma once

#include "deng/Exceptions.h"
#include <array>
#include <string>
#include <bitset>

namespace DENG {

	template <typename ...Args>
	std::string FormatString(const char* _szFormat, Args... args) {
		int iSize = std::snprintf(nullptr, 0, _szFormat, args...) + 1;
		if (iSize <= 0) { throw STDException("snprintf(): Error during formating"); }
		size_t uSize = static_cast<size_t>(iSize);
		std::string buf;
		buf.resize(uSize, 0);
		std::snprintf(buf.data(), uSize, _szFormat, args...);
		buf.pop_back();
		return buf;
	}

	template <size_t N>
	std::string BitsetToHexString(const std::bitset<N>& _bitset) {
		std::string sBuf;
		sBuf.reserve(N / 4);

		const std::array<char, 16> cLookup{
			'0', '1', '2', '3',
			'4', '5', '6', '7',
			'8', '9', 'a', 'b',
			'c', 'd', 'e', 'f'
		};

		for (size_t i = 0; i < _bitset.size(); i += 4) {
			uint64_t uVal = 0;
			for (size_t j = 0; j < 4; j++)
				if (_bitset[i+j])
					uVal |= (static_cast<size_t>(1) << j);
			sBuf += cLookup[uVal % 16];
		}

		return sBuf;
	}
}