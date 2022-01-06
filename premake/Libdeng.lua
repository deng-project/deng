--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: Libdeng.lua - Libdeng build configuration file
--- author: Karl-Mihkel Ott

local Libdeng = {}

-- Build libdeng library
function Libdeng.build()
    project "deng"
        if os.istarget("windows") then
            kind "StaticLib"
        else
            kind "SharedLib"
        end
        language "C++"
        cppdialect "C++14"
        includedirs { "./include", "./deps", "./deps/nekowin/include/third_party" }

        files {
            -- manually defining each and every single file, grim --
            "include/*.h",
            "src/*.c",
            "src/*.cpp",
            "src/imgui-layer/ui_manager.cpp"
        }

        removefiles { 
            "src/vec*.cpp",
            "src/mat*.cpp",
            "src/font.cpp",
            "include/font.h"
        }
        removefiles {  }

        -- Link all Windows specific dependencies
        filter "platforms:Win32"
            links {
                "imgui",
                "nwin",
                "vulkan-1",
                "comctl32",
				"iphlpapi"
            }

        -- Link all GNU/Linux specific dependencies
        filter "platforms:Linux"
            links {
                "imgui",
                "nwin",
                "vulkan",
                "pthread",
                "X11",
                "GL",
                "Xcursor",
                "dl"
            }

        filter {}
end

return Libdeng
