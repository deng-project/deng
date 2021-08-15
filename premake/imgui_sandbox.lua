--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: imgui_sandbox.lua - imgui_sandbox application build configuration
--- author: Karl-Mihkel Ott


local imgui_sandbox = {}


-- Build ImGui sandbox application
function imgui_sandbox.build()
    project "imgui_sandbox"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++14"

        files { 
            "src/sandbox/imgui_sandbox/imgui_sandbox.cpp",
            "src/sandbox/imgui_sandbox/imgui_sandbox.h"
        }
        links { "deng", "imgui" }
end


return imgui_sandbox
