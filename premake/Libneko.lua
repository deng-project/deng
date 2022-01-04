--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: Libneko.lua - Libneko build configuration file
--- author: Karl-Mihkel Ott


local Libneko = {};

function Libneko.build()
    project "nwin"
        kind "StaticLib"
        language "C"
        cdialect "C99"

        files {
            "deps/nekowin/include/*.h",
            "deps/nekowin/src/*.c"
        }

        filter "platforms:Win32"
            removefiles { "deps/nekowin/src/x11_window.c" }
            defines { "_CRT_SECURE_NO_WARNINGS" }
            links {
                "vulkan-1",
                "gdi32"
            }

        filter "platforms:Linux"
            removefiles { "deps/nekowin/src/win32_window.c" }
            links {
                "X11",
                "Xcursor",
                "Xrandr",
                "GL",
                "dl",
                "vulkan"
            }

        filter {}
end

return Libneko
