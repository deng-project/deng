--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: OpenGLTriangle.lua - OpenGL triangle application build test
--- author: Karl-Mihkel Ott


project "OpenGLTriangle"
	basedir("../..")
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    includedirs { "../../include", "../../deps", "../../deps/nekowin/include/third_party" }

    files { "../../tests/OpenGLTriangle.cpp" }

    libdirs { "%{cfg.buildtarget.directory}" }
    defines { "LIBDAS_STATIC" }
	links { "deng" }
		
	filter "platforms:Win32"
        libdirs { "C:/VulkanSDK/**/Lib" }
        includedirs { "C:/VulkanSDK/**/Include" }
			
	postbuildcommands {
		"{RMDIR} %{cfg.buildtarget.directory}/OpenGLTriangleData",
		"{COPYDIR} tests/OpenGLTriangleData %{cfg.buildtarget.directory}/OpenGLTriangleData"
	}
