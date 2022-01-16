--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: Libdeng.lua - DENG dynamic library build config
--- author: Karl-Mihkel Ott

local Libdeng = {}

function Libdeng.build()
	project "deng"
		kind "SharedLib"
		language "C++"
		cppdialect "C++17"
		
		files {
			"src/OpenGLBufferLoader.cpp",
            "src/OpenGLShaderLoader.cpp",
            "src/OpenGLRenderer.cpp",
            "src/VulkanDescriptorPoolCreator.cpp",
            "src/VulkanDescriptorSetLayoutCreator.cpp",
            "src/VulkanDescriptorSetsCreator.cpp",
            "src/VulkanHelpers.cpp",
            "src/VulkanInstanceCreator.cpp",
            "src/VulkanPipelineCreator.cpp",
            "src/VulkanRenderer.cpp",
            "src/VulkanSwapchainCreator.cpp",
            "src/VulkanUniformBufferAllocator.cpp",
			"src/Window.cpp",
		}
		
		defines { "DENG_EXPORT_LIBRARY" }
		includedirs { "include", "deps", "./deps/nekowin/include/third_party" }
		
		links { "shaderc_shared", "nwin" }
		
		filter "platforms:Linux"
            links { "GL", "GLX", "X11", "Xcursor", "dl", "vulkan" }
        filter "platforms:Win32"
            libdirs { "C:/VulkanSDK/**/Lib" }
            includedirs { "C:/VulkanSDK/**/Include" }

end

return Libdeng