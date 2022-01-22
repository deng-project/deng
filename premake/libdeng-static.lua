--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: libdeng-static.lua - DENG static library build config
--- author: Karl-Mihkel Ott

project "deng"
	basedir("..")
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	
	files {
		"../include/*.h",
		"../src/OpenGLBufferLoader.cpp",
        "../src/OpenGLShaderLoader.cpp",
		"../src/OpenGLRenderer.cpp",
        "../src/VulkanDescriptorPoolCreator.cpp",
        "../src/VulkanDescriptorSetLayoutCreator.cpp",
        "../src/VulkanDescriptorSetsCreator.cpp",
        "../src/VulkanHelpers.cpp",
        "../src/VulkanInstanceCreator.cpp",
        "../src/VulkanPipelineCreator.cpp",
        "../src/VulkanRenderer.cpp",
        "../src/VulkanSwapchainCreator.cpp",
        "../src/VulkanUniformBufferAllocator.cpp",
		"../src/Window.cpp",
	}
	
	defines { "DENG_EXPORT_LIBRARY", "LIBDAS_STATIC", "LIBNWIN_STATIC", "DENG_STATIC" }
	includedirs { "../include", "../deps", "../deps/nekowin/include/third_party" }
	
    libdirs { "%{cfg.buildtarget.directory}" }
	links { "shaderc_shared", "nwin", "das" }
	
	filter "platforms:Win32"
		libdirs { "C:/VulkanSDK/**/Lib" }
        includedirs { "C:/VulkanSDK/**/Include" }