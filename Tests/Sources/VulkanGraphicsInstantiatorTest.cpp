// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanGraphicsInstantiatorTest.cpp - Smoke test for testing Vulkan graphics instantiator implementation
// author: Karl-Mihkel Ott

#include <iostream>
#include <gtest/gtest.h>

#include "deng/ErrorDefinitions.h"
#include "deng/Exceptions.h"
#include "deng/SDLWindowContext.h"
#include "deng/VulkanGraphicsInstantiator.h"

using namespace DENG;

TEST(VulkanGraphicsInstantiatorClass, DevicePickSmokeTest)
{
	Handle<IWindowContext> hWindow = {};
	
	try
	{
		hWindow = new SDLWindowContext();
		hWindow->SetHints(WindowHint_Hidden | WindowHint_Vulkan);
		hWindow->Create("Demo", 10, 10);
	}
	catch (const WindowContextException& e)
	{
		DISPATCH_ERROR_MESSAGE("WindowContextException", e.what(), ErrorSeverity::CRITICAL_NON_FATAL);
		EXPECT_FALSE(true);
	}


	Handle<IGraphicsInstantiator> hGraphicsInstantiator; 
	try
	{
		hGraphicsInstantiator = new Vulkan::GraphicsInstantiator(hWindow, GPUPickStrategy::BestScoring);
		LOG("Picked graphics card: " << hGraphicsInstantiator->GetGraphicsCardName());
	}
	catch (const WindowContextException& e)
	{
		DISPATCH_ERROR_MESSAGE("WindowContextException", e.what(), ErrorSeverity::CRITICAL_NON_FATAL);
		EXPECT_FALSE(true);
	}
	catch (const RendererException& e)
	{
		DISPATCH_ERROR_MESSAGE("RendererException", e.what(), ErrorSeverity::CRITICAL_NON_FATAL);
		EXPECT_FALSE(true);
	}
	catch (const HardwareException& e)
	{
		DISPATCH_ERROR_MESSAGE("HardwareException", e.what(), ErrorSeverity::CRITICAL_NON_FATAL);
		EXPECT_FALSE(true);
	}
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}