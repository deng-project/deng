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
		public:
			SpirvByteCodeToolset(const std::vector<uint32_t>& _vertexBytes, const std::vector<uint32_t>& _geometryBytes, const std::vector<uint32_t>& _fragmentBytes);
	};
}