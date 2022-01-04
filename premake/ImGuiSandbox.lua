--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: ImGuiSandbox.lua - ImGui sandbox application build configuration
--- author: Karl-Mihkel Ott


local ImGuiSandbox = {}


-- Build ImGui sandbox application
function ImGuiSandbox.build()
    project "ImGuiSandbox"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++14"
        includedirs{ "./include", "./deps" }

        files { 
            "src/sandbox/imgui_sandbox/imgui_sandbox.cpp",
            "src/sandbox/imgui_sandbox/imgui_sandbox.h"
        }
        links { "deng", "imgui" }
end


return ImGuiSandbox
