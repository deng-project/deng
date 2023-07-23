// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SID.cpp - Runtime string hash algorithm
// author: Karl-Mihkel Ott

#include "deng/SID.h"

namespace DENG {

#if defined(ENV32)
	uint32_t RuntimeCrc32(const std::string& _str) {
		uint32_t uHash = 0;
		for (size_t i = 0; i < _str.size(); i++) {
			uHash = (uHash >> 8) ^ crc32_table[(uHash ^ _str[i]) & 0xff];
		}

		return uHash ^ 0xffffffff;
	}
#elif defined (ENV64)
	uint64_t RuntimeCrc64(const std::string& _str) {
		uint64_t uHash = 0;
		for (size_t i = 0; i < _str.size(); i++) {
			uHash = (uHash >> 8) ^ crc64_table[(uHash ^ _str[i]) & 0xff];
		}

		return uHash ^ 0xffffffffffffffff;
	}
#endif
}