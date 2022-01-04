--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: SceneEditor.lua - SceneEditor application build configuration
--- author: Karl-Mihkel Ott

local SceneEditor = {}


function SceneEditor.build()
    project "dengsc"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++14"
        includedirs{ "./include", "./deps" }

        files {
            "headers/*",
            "src/sc_ed.cpp",
            "src/sc_ed_entity_panel.cpp",
            "src/sc_ed_ui.cpp",
        }

        links { "deng", "imgui" }
end


return SceneEditor
