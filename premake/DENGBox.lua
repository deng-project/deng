--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: DENGBox.lua - DENG vulkan test application build configuration
--- author: Karl-Mihkel Ott

local DENGBox = {}


function DENGBox.build()
    project "DENGBox"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++14"
        includedirs{ "./include", "./deps" }
        files {
            "src/sandbox/dengbox/dengbox.h",
            "src/sandbox/dengbox/dengbox.cpp"
        }

		libdirs { "build" }
        links { "deng" }

        filter "configurations:Debug"
            symbols "On"
        filter "configurations:Release"
            symbols "Off"
end

return DENGBox
