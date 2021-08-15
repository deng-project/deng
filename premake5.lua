--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: premake5.lua - main premake5 configuration file
--- author: Karl-Mihkel Ott

workspace "deng"
    cleancommands { "make clean %{cfg.buildcfg}" }
    configurations { "Debug", "Release" }
    platforms { "Win32", "Linux" }
    includedirs { 
        "./headers",
        "./modules/imgui",
        "./modules/glad/include",
        "./modules/nekowin/include"
    }

	architecture "x86_64"
    defines { "ImDrawIdx=unsigned int" }
	pic "On"

	-- Ignore safety warnings that MSVC gives
	--filter "platforms:Win32"
		--defines { "_CRT_SECURE_NO_WARNINGS" }

    -- Enable debug symbols if specified
    filter "configurations:Debug"
        symbols "On"
        optimize "Debug"
        targetdir "build/debug"
        defines { "__DEBUG" }

    filter "configurations:Release"
        symbols "Off"
        optimize "Speed"
        targetdir "build/release"
    filter {}

--!!! Add new options to use !!!--
-- Create an option to specify vulkan sdk library location (Windows only)
newoption {
	trigger = "vk-sdk-path",
	description = "Specify Vulkan SDK path for Windows builds (Windows only)"
}


-- Create an option to build DENG sandboxapp using specific sandbox configuration
newoption {
    trigger = "sandbox-mode",
    value = "MODE",
    description = "Select sandbox application mode",
    allowed = {
        { "deng", "Build an application for testing the renderer by loading assets" },
        { "imgui", "Build an application for testing ImGui with DENG renderer" },
        { "opengl", "Build a test application for testing OpenGL functionality" },
        { "none", "Do not build any sandbox applications (default)" },
        { "all", "Build all sandbox applications" }
    }
}


-- Check if given operating system is even supported
function oscheck() 
    if(not os.istarget("linux") and not os.istarget("windows")) then
        local host = os.target()
        host = host:gsub("%a", string.upper, 1)
        print(host .. " is not supported by DENG! :(")
        os.exit();
    end
end


-- Define the clean action 
function cleanoptcheck()
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


-- Check for the SDK path to be used in correct situation
function vksdkoptcheck()
    -- Check if Vulkan SDK path should be and if it is specified
    if os.istarget("windows") and _OPTIONS["vk-sdk-path"] then
        libdirs { _OPTIONS["vk-sdk-path"] .. "\\Lib" }
        includedirs{ _OPTIONS["vk-sdk-path"] .. "\\Include" }
    elseif os.istarget("windows") then
        libdirs { "C:/VulkanSDK/*/Lib" }
        includedirs { "C:/VulkanSDK/*/Include" }
    end
end


-- Check if given options are valid
function optcheck()
    cleanoptcheck()
    vksdkoptcheck()
end


-- Check which modules to build
function modcheck()
    -- These modules are going to be required so they are built
    local imgui = require("premake/imgui")
    imgui.build()

    local libneko = require("premake/libneko");
    libneko.build()
    if _OPTIONS["sandbox-mode"] then
        postbuildcommands {
            "{RMDIR} %{cfg.targetdir}/assets",
            "{COPYDIR} assets %{cfg.targetdir}/assets",
            "{RMDIR} %{cfg.targetdir}/shaders",
            "{COPYDIR} shaders %{cfg.targetdir}/shaders",
            "{RMDIR} %{cfg.targetdir}/textures",
            "{COPYDIR} textures %{cfg.targetdir}/textures"
        }
    end
end


-- Setup build destinations
function buildcfg()
    if _OPTIONS["sandbox-mode"] then

        -- Sandbox application build configuration
        if _OPTIONS["sandbox-mode"] == "deng" or _OPTIONS["sandbox-mode"] == "all" then
            local dengbox = require("premake/dengbox")
            dengbox.build()
        end

        if _OPTIONS["sandbox-mode"] == "imgui" or _OPTIONS["sandbox-mode"] == "all" then
            local imgui_sandbox = require("premake/imgui_sandbox")
            imgui_sandbox.build()
        end

        if _OPTIONS["sandbox-mode"] == "opengl" or _OPTIONS["sandbox-mode"] == "all" then
            local opengl_sandbox = require("premake/opengl_sandbox")
            opengl_sandbox.build()
        end
    end

    -- Build DENG runtime library
    local libdeng = require("premake/libdeng")
    libdeng.build()

    -- Build DENG asset manager application
    local dam = require("premake/dam")
    dam.build()

    -- Build DENG scene editor
    local scene_ed = require("premake/scene_ed")
    scene_ed.build()
end


-- Script entry point, check if not help
if not _OPTIONS["help"] then
    oscheck()
    optcheck()
    modcheck()
    buildcfg()
end
