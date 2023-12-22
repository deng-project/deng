// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SpirvByteCodeToolset.h - ByteCodeToolset implementation class for Spirv bytecode format header
// author: Karl-Mihkel Ott

#pragma once

#include "deng/IByteCodeToolset.h"

namespace DENG
{
	class DENG_API SpirvByteCodeToolset : public IByteCodeToolset
	{
		private:
			inline uint32_t FixWordEndianness(const uint32_t _uWord)
			{
				if (_IsLittleEndian())
				{
					return (_uWord & 0x000000ff) << 24 | (_uWord & 0x0000ff00) << 8 |
						(_uWord & 0x00ff0000) >> 8 | (_uWord & 0xff000000) >> 24;
				}

				return _uWord;
			}

			// returns true if current system endianness matches with bytecode endianness, false otherwise
			bool _VerifyHeader(const std::vector<uint32_t>& _bytes);

		public:
			SpirvByteCodeToolset(const std::vector<uint32_t>& _vertexBytes, const std::vector<uint32_t>& _geometryBytes, const std::vector<uint32_t>& _fragmentBytes);
	};
}