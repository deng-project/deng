--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: imgui.lua - Build configuration for ImGui static library
--- author: Karl-Mihkel Ott

local ImGui = {}

-- Create a imgui library build
function ImGui.build() 
    project "imgui"
        kind "StaticLib"
        language "C++"
        defines { "ImDrawIdx=unsigned int" }
        includedirs { "deps/imgui" }
        files {
            "deps/imgui/imgui.cpp",
            "deps/imgui/imgui_widgets.cpp",
            "deps/imgui/imgui_draw.cpp",
            "deps/imgui/imgui_tables.cpp",
            "deps/imgui/imgui.h"
        }

        filter "configurations:Debug"
            files { "deps/imgui/imgui_demo.cpp" }

        optimize "Full"
end

return ImGui
