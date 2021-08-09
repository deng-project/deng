--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: libneko.lua - libneko build configuration file
--- author: Karl-Mihkel Ott


local libneko = {};

function libneko.build()
    project "nwin"
        kind "StaticLib"
        language "C"
        cdialect "C99"

        files {
            "modules/nekowin/include/*.h",
            "modules/nekowin/src/*.c"
        }

        filter "platforms:Win32"
            removefiles { "modules/nekowin/src/x11_window.c" }
            defines { "_CRT_SECURE_NO_WARNINGS" }
            links {
                "vulkan-1",
                "gdi32"
            }

        filter "platforms:Linux"
            removefiles { "modules/nekowin/src/win32_window.c" }
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

return libneko
