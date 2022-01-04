--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: OpenGLSandbox.lua - OpenGL sandbox application premake5 configuration file
--- author: Karl-Mihkel Ott

local OpenGLSandbox = {}

function OpenGLSandbox.build() 
    project "OpenGLSandbox"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++14"
        files {
            "src/sandbox/opengl/opengl_sandbox.h",
            "src/sandbox/opengl/opengl_sandbox.cpp",
        }

        libdirs { "build" }
        links { "deng" }

        filter "configurations:Debug"
            symbols "On"
        filter "configurations:Release"
            symbols "Off"
end

return OpenGLSandbox
