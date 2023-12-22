// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SpirvByteCodeToolset.h - ByteCodeToolset implementation class for Spirv bytecode format implementation
// author: Karl-Mihkel Ott

#include <ostream>
#include <sstream>
#include "deng/ErrorDefinitions.h"
#include "deng/Exceptions.h"
#include <spirv/unified1/spirv.hpp>
#include "deng/SpirvByteCodeToolset.h"

#define SPV_MAGIC 0x07230203
#define SPV_ENDIANNESS_CONFLICT_MAGIC 0x03022307
#define SPV_VERSION(major, minor) ((static_cast<uint32_t>(major%256) << 16) | (static_cast<uint32_t>(minor%256) << 8))
#define SPV_VERSION_MAJOR(x) ((x >> 16) % 256)
#define SPV_VERSION_MINOR(x) ((x >> 8) % 256)
#define SPV_FIRST_WORD_INDEX 5

namespace DENG
{
	SpirvByteCodeToolset::SpirvByteCodeToolset(const std::vector<uint32_t>& _vertexBytes, const std::vector<uint32_t>& _geometryBytes, const std::vector<uint32_t>& _fragmentBytes) :
		IByteCodeToolset(_vertexBytes, _geometryBytes, _fragmentBytes)
	{
		// check vertex shader inputs
		bool _bEndiannessMatch = _VerifyHeader(_vertexBytes);

		for (size_t i = SPV_FIRST_WORD_INDEX; i < _vertexBytes.size(); i++)
		{
			// TODO: perform input location extraction
		}
	}

	bool SpirvByteCodeToolset::_VerifyHeader(const std::vector<uint32_t>& _bytes)
	{
		uint32_t uMagic = FixWordEndianness(_bytes[0]);

		// check if the magic number is correct
		bool bConflictingEndianness = uMagic == SPV_ENDIANNESS_CONFLICT_MAGIC;

		if (uMagic != SPV_MAGIC && uMagic != SPV_ENDIANNESS_CONFLICT_MAGIC)
		{
			throw ShaderException("Invalid SPIR-V bytecode - wrong magic number");
		}

		uint32_t uVersion = FixWordEndianness(_bytes[1]);

		if (uVersion > SPV_VERSION(1, 3))
		{
			std::stringstream ss;
			ss << "Unsupported SPIR-V version " << SPV_VERSION_MAJOR(uVersion) << '.' << SPV_VERSION_MINOR(uVersion);
		}
		LOG("SPIR-V version: " << SPV_VERSION_MAJOR(uVersion) << '.' << SPV_VERSION_MINOR(uVersion));

#ifdef _DEBUG
		uint32_t uGenerator = FixWordEndianness(_bytes[2]);
		LOG("SPIR-V generator: " << uGenerator);
		uint32_t uBound = FixWordEndianness(_bytes[3]);
		LOG("SPIR-V bound: 0 < id < " << uBound);
#endif
	}
}