// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: IShader.h - shader interface class implementation
// author: Karl-Mihkel Ott

#include <array>
#include <deng/IShader.h>

namespace DENG {

	const char* IShader::_Props2HexString() const {
		std::size_t uBufCounter = 0;
		static char s_buffer[N_BITS / 4 + 1]{};
		
		const std::array<char, 16> cLookup{
			'0', '1', '2', '3',
			'4', '5', '6', '7',
			'8', '9', 'a', 'b',
			'c', 'd', 'e', 'f'
		};

		for (size_t i = 0; i < m_bProperties.size();) {
			uint64_t uVal = 0;
			for (size_t j = 0; j < 4; ++j) {
				if (m_bProperties[i + j])
					uVal |= (1 << j);
			}
			i += 4;

			s_buffer[uBufCounter++] = cLookup[uVal];
		}

		return s_buffer;
	}
}