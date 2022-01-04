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

        filter {}

        postbuildcommands {
            "{RMDIR} %{cfg.targetdir}/include",
            "{COPY} headers %{cfg.targetdir}/include",
            "{COPY} src/math/vec2.cpp %{cfg.targetdir}/include/math/vec2.cpp",
            "{COPY} src/math/vec3.cpp %{cfg.targetdir}/include/math/vec3.cpp",
            "{COPY} src/math/vec4.cpp %{cfg.targetdir}/include/math/vec4.cpp",
            "{COPY} src/math/mat2.cpp %{cfg.targetdir}/include/math/mat2.cpp",
            "{COPY} src/math/mat3.cpp %{cfg.targetdir}/include/math/mat3.cpp",
            "{COPY} src/math/mat4.cpp %{cfg.targetdir}/include/math/mat4.cpp",
            "{COPY} deps/imgui/*.h %{cfg.targetdir}/include",
            "{COPY} deps/nekowin/include/*.h %{cfg.targetdir}/include",
            "{COPY} deps/nekowin/include/glad %{cfg.targetdir}/include"
        }
end

return libdeng
