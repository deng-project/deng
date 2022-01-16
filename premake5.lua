--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: premake5.lua - main premake5 configuration file
--- author: Karl-Mihkel Ott

workspace "deng"
    cleancommands { "make clean %{cfg.buildcfg}" }
    configurations { "Debug", "Release" }
    platforms { "Win32", "Linux" }

	architecture "x86_64"
	pic "On"

	-- Ignore safety warnings that MSVC gives
	filter "platforms:Win32"
		defines { "_CRT_SECURE_NO_WARNINGS" }

    -- Enable debug symbols if specified
    filter "configurations:Debug"
        symbols "On"
        optimize "Off"
        targetdir "build/debug"
        defines { "_DEBUG" }

    filter "configurations:Release"
        symbols "Off"
        optimize "Speed"
        targetdir "build/release"

    filter {}

--!!! Add new options to use !!!--
newoption {
    trigger = "tests",
    value = "<TEST-NAME>",
    description = "Choose tests to build",
    default = "none",
    allowed = {
        { "OpenGLTriangle", "Build OpenGL triangle test application" },
        { "all", "Build all available tests" },
        { "none", "Do not build any test application (use only with legacy build)" }
    }
}


newoption {
    trigger = "cleanbuild",
    description = "Remove all statically built dependencies after building"
}


newoption {
    trigger = "no-deps",
    description = "Do not build dependencies from git submodules (do not use it on Windows)"
}


-- Check if given operating system is even supported
function OsCheck() 
    if(not os.istarget("linux") and not os.istarget("windows")) then
        local host = os.target()
        host = host:gsub("%a", string.upper, 1)
        print(host .. " is not supported by DENG! :(")
        os.exit();
    end
end


-- Define the clean action 
function CheckForCleaning()
    if _ACTION == "clean" then
        print("Cleaning project files")
        if(os.host() == "linux") then
            os.execute("rm -rf *.make obj Makefile")
            os.exit()
        elseif(os.host() == "windows") then
            os.execute("del /s /q deng.sln obj *.vcxproj*")
            os.exit()
        end
    end
end


-- Check which modules to build
function LoadModuleConfs()
    local libneko = require("premake/Libneko");
    libneko.build()

    local libdas = require("premake/Libdas")
    libdas.build()
	
	local libdeng = require("premake/Libdeng")
	libdeng.build()

    if not _OPTIONS["no-deps"] then
        local shaderc = require("premake/ShaderC")
        shaderc.build()
    end
end


-- Setup build destinations
function LoadTestConfs()
    if _OPTIONS["tests"] == "all" or _OPTIONS["tests"] == "OpenGLTriangle" then
        local triangle = require("premake/tests/OpenGLTriangle")
        triangle.build()
    end

    if _OPTIONS["tests"] then
        postbuildcommands {
            "{RMDIR} %{cfg.targetdir}/shaders",
            "{COPYDIR} shaders %{cfg.targetdir}/shaders",
        }
    end
end


-- Script entry point, check if not help
if not _OPTIONS["help"] and not _OPTIONS["shaderc-only"] then
    OsCheck()
    CheckForCleaning()
    LoadModuleConfs()
    LoadTestConfs()
end
