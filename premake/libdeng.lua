--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: libdeng.lua - libdeng build configuration file
--- author: Karl-Mihkel Ott

local libdeng = {}

-- Build libdeng library
function libdeng.build()
    project "deng"
        if os.istarget("windows") then
            kind "StaticLib"
        else
            kind "SharedLib"
        end
        language "C++"
        cppdialect "C++14"

        files {
            "headers/**/*.h",
            "src/common/*.c",
            "src/data/*.c",
            "src/deng/**.cpp",
            "src/deng/**.c",
            "src/imgui-layer/ui_manager.cpp",
            "src/math/camera_mat.cpp",
            "src/math/conversion.cpp",
            "src/math/projection_mat.cpp",
            "src/math/transformer.cpp",
            "src/utils/*.cpp"
        }

        removefiles { 
            "src/deng/opengl/test_application/test.cpp",
            "src/utils/font.cpp",
            "headers/utils/font.h"
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

        filter "configurations:Debug"
            defines { "__DEBUG" }
        filter {}
end

return libdeng
