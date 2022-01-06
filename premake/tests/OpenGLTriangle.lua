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

        includedirs { "include", "deps" }

        files {
            "src/OpenGLBufferLoader.cpp",
            "src/OpenGLShaderLoader.cpp",
            "src/OpenGLRenderer.cpp",
            "tests/OpenGLTriangle.cpp",
            "src/Window.cpp",
        }

        links { "nwin", "das" }

        filter "platforms:Linux"
            links { "GL", "GLX", "X11", "Xcursor", "dl", "vulkan" }
end

return OpenGLTriangle
