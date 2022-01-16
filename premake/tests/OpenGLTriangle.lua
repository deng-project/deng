--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: OpenGLTriangle.lua - OpenGL triangle application build test
--- author: Karl-Mihkel Ott

local OpenGLTriangle = {}

function OpenGLTriangle.build()
    project "OpenGLTriangle"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"

        includedirs { "include", "deps", "./deps/nekowin/include/third_party" }

        files { "tests/OpenGLTriangle.cpp" }

        links { "deng", "shaderc_shared", "nwin" }

        filter "platforms:Linux"
            links { "GL", "GLX", "X11", "Xcursor", "dl", "vulkan" }
        filter "platforms:Win32"
            libdirs { "C:/VulkanSDK/**/Lib" }
            includedirs { "C:/VulkanSDK/**/Include" }
end

return OpenGLTriangle
