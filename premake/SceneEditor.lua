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

        files {
            "headers/imgui-layer/scene_editor/*",
            "src/imgui-layer/scene_editor/*"
        }

        links { "deng", "imgui" }
end


return SceneEditor
