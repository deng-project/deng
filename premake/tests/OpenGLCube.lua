--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: OpenGLCube.lua - OpenGL cube test application build config
--- author: Karl-Mihkel Ott

project "OpenGLCube"
	basedir("../..")
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
		
	includedirs { "../../include", "../../deps", "../../deps/nekowin/include/third_party" }
		
	files { "../../tests/OpenGLCube.cpp" }
	libdirs { "%{cfg.buildtarget.directory}" }
	defines { "LIBDAS_STATIC" }
	
	links { "deng" }
		
	filter "platforms:Win32"
		libdirs { "C:/VulkanSDK/**/Lib" }
		includedirs { "C:/VulkanSDK/**/Include" }
			
	postbuildcommands {
		"{RMDIR} %{cfg.buildtarget.directory}/OpenGLCubeData",
		"{COPYDIR} tests/OpenGLCube %{cfg.buildtarget.directory}"
	}