// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SpirvByteCodeToolsetTest.cpp - Unit tests for testing SpirvByteCodeToolset class
// author: Karl-Mihkel Ott

#include <iostream>
#include <gtest/gtest.h>
#define DENG_API
#include "deng/SpirvByteCodeToolset.h"

using namespace DENG;

static const std::vector<uint32_t> g_vertexSPV = {
	0x07230203, 0x00010000, 0x000e0000, 0x0000002e,
	0x00000000, 0x00020011, 0x00000001, 0x0003000e,
	0x00000000, 0x00000001, 0x000b000f, 0x00000000,
	0x00000001, 0x6e69616d, 0x00000000, 0x00000002,
	0x00000003, 0x00000004, 0x00000005, 0x00000006,
	0x00000007, 0x00030003, 0x00000005, 0x00000258,
	0x00050005, 0x00000008, 0x65707974, 0x6f62752e,
	0x00000000, 0x00040006, 0x00000008, 0x00000000,
	0x006f6275, 0x00030005, 0x00000009, 0x004f4255,
	0x00050006, 0x00000009, 0x00000000, 0x657a6953,
	0x00000000, 0x00030005, 0x0000000a, 0x006f6275,
	0x00070005, 0x00000002, 0x762e6e69, 0x502e7261,
	0x5449534f, 0x304e4f49, 0x00000000, 0x00070005,
	0x00000003, 0x762e6e69, 0x542e7261, 0x4f435845,
	0x3044524f, 0x00000000, 0x00060005, 0x00000004,
	0x762e6e69, 0x432e7261, 0x524f4c4f, 0x00000030,
	0x00070005, 0x00000006, 0x2e74756f, 0x2e726176,
	0x43584554, 0x44524f4f, 0x00000030, 0x00060005,
	0x00000007, 0x2e74756f, 0x2e726176, 0x4f4c4f43,
	0x00003052, 0x00040005, 0x00000001, 0x6e69616d,
	0x00000000, 0x00040047, 0x00000005, 0x0000000b,
	0x00000000, 0x00040047, 0x00000002, 0x0000001e,
	0x00000000, 0x00040047, 0x00000003, 0x0000001e,
	0x00000001, 0x00040047, 0x00000004, 0x0000001e,
	0x00000002, 0x00040047, 0x00000006, 0x0000001e,
	0x00000000, 0x00040047, 0x00000007, 0x0000001e,
	0x00000001, 0x00040047, 0x0000000a, 0x00000022,
	0x00000000, 0x00040047, 0x0000000a, 0x00000021,
	0x00000000, 0x00050048, 0x00000009, 0x00000000,
	0x00000023, 0x00000000, 0x00050048, 0x00000008,
	0x00000000, 0x00000023, 0x00000000, 0x00030047,
	0x00000008, 0x00000002, 0x00040015, 0x0000000b,
	0x00000020, 0x00000001, 0x0004002b, 0x0000000b,
	0x0000000c, 0x00000000, 0x00030016, 0x0000000d,
	0x00000020, 0x0004002b, 0x0000000d, 0x0000000e,
	0x40000000, 0x0004002b, 0x0000000d, 0x0000000f,
	0x3f800000, 0x0004002b, 0x0000000b, 0x00000010,
	0x00000001, 0x0004002b, 0x0000000d, 0x00000011,
	0x00000000, 0x00040017, 0x00000012, 0x0000000d,
	0x00000004, 0x00040017, 0x00000013, 0x0000000d,
	0x00000002, 0x0003001e, 0x00000009, 0x00000013,
	0x0003001e, 0x00000008, 0x00000009, 0x00040020,
	0x00000014, 0x00000002, 0x00000008, 0x00040020,
	0x00000015, 0x00000001, 0x00000013, 0x00040020,
	0x00000016, 0x00000001, 0x00000012, 0x00040020,
	0x00000017, 0x00000003, 0x00000012, 0x00040020,
	0x00000018, 0x00000003, 0x00000013, 0x00020013,
	0x00000019, 0x00030021, 0x0000001a, 0x00000019,
	0x00040020, 0x0000001b, 0x00000002, 0x0000000d,
	0x0004003b, 0x00000014, 0x0000000a, 0x00000002,
	0x0004003b, 0x00000015, 0x00000002, 0x00000001,
	0x0004003b, 0x00000015, 0x00000003, 0x00000001,
	0x0004003b, 0x00000016, 0x00000004, 0x00000001,
	0x0004003b, 0x00000017, 0x00000005, 0x00000003,
	0x0004003b, 0x00000018, 0x00000006, 0x00000003,
	0x0004003b, 0x00000017, 0x00000007, 0x00000003,
	0x0004002b, 0x0000000d, 0x0000001c, 0xc0000000,
	0x00050036, 0x00000019, 0x00000001, 0x00000000,
	0x0000001a, 0x000200f8, 0x0000001d, 0x0004003d,
	0x00000013, 0x0000001e, 0x00000002, 0x0004003d,
	0x00000013, 0x0000001f, 0x00000003, 0x0004003d,
	0x00000012, 0x00000020, 0x00000004, 0x00050051,
	0x0000000d, 0x00000021, 0x0000001e, 0x00000000,
	0x00050085, 0x0000000d, 0x00000022, 0x00000021,
	0x0000000e, 0x00070041, 0x0000001b, 0x00000023,
	0x0000000a, 0x0000000c, 0x0000000c, 0x0000000c,
	0x0004003d, 0x0000000d, 0x00000024, 0x00000023,
	0x00050088, 0x0000000d, 0x00000025, 0x00000022,
	0x00000024, 0x00050083, 0x0000000d, 0x00000026,
	0x00000025, 0x0000000f, 0x00050051, 0x0000000d,
	0x00000027, 0x0000001e, 0x00000001, 0x00050085,
	0x0000000d, 0x00000028, 0x00000027, 0x0000001c,
	0x00070041, 0x0000001b, 0x00000029, 0x0000000a,
	0x0000000c, 0x0000000c, 0x00000010, 0x0004003d,
	0x0000000d, 0x0000002a, 0x00000029, 0x00050088,
	0x0000000d, 0x0000002b, 0x00000028, 0x0000002a,
	0x00050081, 0x0000000d, 0x0000002c, 0x0000002b,
	0x0000000f, 0x00070050, 0x00000012, 0x0000002d,
	0x00000026, 0x0000002c, 0x00000011, 0x0000000f,
	0x0003003e, 0x00000005, 0x0000002d, 0x0003003e,
	0x00000006, 0x0000001f, 0x0003003e, 0x00000007,
	0x00000020, 0x000100fd, 0x00010038,
};

static const std::vector<uint32_t> g_fragmentSPV = {
	0x07230203, 0x00010000, 0x000e0000, 0x00000017,
	0x00000000, 0x00020011, 0x00000001, 0x0003000e,
	0x00000000, 0x00000001, 0x0008000f, 0x00000004,
	0x00000001, 0x6e69616d, 0x00000000, 0x00000002,
	0x00000003, 0x00000004, 0x00030010, 0x00000001,
	0x00000007, 0x00030003, 0x00000005, 0x00000258,
	0x00060005, 0x00000005, 0x65707974, 0x2e64322e,
	0x67616d69, 0x00000065, 0x00050005, 0x00000006,
	0x54706d73, 0x75747865, 0x00006572, 0x00070005,
	0x00000002, 0x762e6e69, 0x542e7261, 0x4f435845,
	0x3044524f, 0x00000000, 0x00060005, 0x00000003,
	0x762e6e69, 0x432e7261, 0x524f4c4f, 0x00000030,
	0x00070005, 0x00000004, 0x2e74756f, 0x2e726176,
	0x545f5653, 0x65677261, 0x00000074, 0x00040005,
	0x00000001, 0x6e69616d, 0x00000000, 0x00070005,
	0x00000007, 0x65707974, 0x6d61732e, 0x64656c70,
	0x616d692e, 0x00006567, 0x00040047, 0x00000002,
	0x0000001e, 0x00000000, 0x00040047, 0x00000003,
	0x0000001e, 0x00000001, 0x00040047, 0x00000004,
	0x0000001e, 0x00000000, 0x00040047, 0x00000006,
	0x00000022, 0x00000000, 0x00040047, 0x00000006,
	0x00000021, 0x00000001, 0x00030016, 0x00000008,
	0x00000020, 0x00040017, 0x00000009, 0x00000008,
	0x00000004, 0x00090019, 0x00000005, 0x00000008,
	0x00000001, 0x00000002, 0x00000000, 0x00000000,
	0x00000001, 0x00000000, 0x00040017, 0x0000000a,
	0x00000008, 0x00000002, 0x00040020, 0x0000000b,
	0x00000001, 0x0000000a, 0x00040020, 0x0000000c,
	0x00000001, 0x00000009, 0x00040020, 0x0000000d,
	0x00000003, 0x00000009, 0x00020013, 0x0000000e,
	0x00030021, 0x0000000f, 0x0000000e, 0x0003001b,
	0x00000007, 0x00000005, 0x0004003b, 0x0000000b,
	0x00000002, 0x00000001, 0x0004003b, 0x0000000c,
	0x00000003, 0x00000001, 0x0004003b, 0x0000000d,
	0x00000004, 0x00000003, 0x00040020, 0x00000010,
	0x00000000, 0x00000007, 0x0004003b, 0x00000010,
	0x00000006, 0x00000000, 0x00050036, 0x0000000e,
	0x00000001, 0x00000000, 0x0000000f, 0x000200f8,
	0x00000011, 0x0004003d, 0x0000000a, 0x00000012,
	0x00000002, 0x0004003d, 0x00000009, 0x00000013,
	0x00000003, 0x0004003d, 0x00000007, 0x00000014,
	0x00000006, 0x00060057, 0x00000009, 0x00000015,
	0x00000014, 0x00000012, 0x00000000, 0x00050085,
	0x00000009, 0x00000016, 0x00000013, 0x00000015,
	0x0003003e, 0x00000004, 0x00000016, 0x000100fd,
	0x00010038,
};


TEST(SpirvByteCodeToolsetClass, ImGuiShaderInputQueryTest)
{
	try
	{
		SpirvByteCodeToolset toolset(g_vertexSPV, {}, g_fragmentSPV);
		auto& inputFormats = toolset.GetShaderInputFormats();

		const std::vector<ShaderInputFormat> expectedFormats = {
			ShaderInputFormat::R32G32_SFLOAT,
			ShaderInputFormat::R32G32_SFLOAT,
			ShaderInputFormat::R32G32B32A32_SFLOAT
		};

		EXPECT_EQ(inputFormats, expectedFormats);
	}
	catch (const ShaderException& _e)
	{
		DISPATCH_ERROR_MESSAGE("ShaderException", _e.what(), ErrorSeverity::WARNING);
		EXPECT_TRUE(false);
	}
}

TEST(SpirvByteCodeToolsetClass, ImGuiShaderBindingQueryTest)
{
	try
	{
		SpirvByteCodeToolset toolset(g_vertexSPV, {}, g_fragmentSPV);
		auto& boundResources = toolset.GetBoundResources();

		const std::vector<BoundResource> expectedResources = {
			{ BoundResourceType::Buffer, ShaderStageBit_Vertex, 16 },
			{ BoundResourceType::ImageSampler, ShaderStageBit_Fragment, 0 }
		};
		
		EXPECT_EQ(boundResources, expectedResources);
	}
	catch (const ShaderException& _e)
	{
		DISPATCH_ERROR_MESSAGE("ShaderException", _e.what(), ErrorSeverity::WARNING);
		EXPECT_TRUE(false);
	}
}


int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}