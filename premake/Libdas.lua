--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: Libdas.lua - Libdas library build configuration
--- author: Karl-Mihkel Ott

local Libdas = {}

function Libdas.build()
    project "das"
        kind "StaticLib"
        language "C++"
        cppdialect "C++17"

        includedirs { "deps/libdas/include" }

        files {
            "deps/libdas/src/*.cpp"
        }

        -- Most of these files are for legacy references --
        removefiles {
            "deps/libdas/src/DASTool.cpp",
            "deps/libdas/src/GLTFCompiler.cpp"
            -- This list is incomplete, you can help by expanding it :)))
        }

        filter "action:vs2019"
            defines { "LIBDAS_EXPORT_LIBRARY" }
end

return Libdas
