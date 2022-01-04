--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: premake5.lua - main premake5 configuration file
--- author: Karl-Mihkel Ott

workspace "deng"
    cleancommands { "make clean %{cfg.buildcfg}" }
    configurations { "Debug", "Release" }
    platforms { "Win32", "Linux" }
    includedirs { 
        "./include",
        "./deps/imgui",
        "./deps/nekowin/include"
    }

	architecture "x86_64"
    defines { "__DENG_LIB_EXPORT" }
	pic "On"

	-- Ignore safety warnings that MSVC gives
	filter "platforms:Win32"
		defines { "_CRT_SECURE_NO_WARNINGS" }

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
-- Create an option to build DENG sandboxapp using specific sandbox configuration
newoption {
    trigger = "sandbox-mode",
    value = "MODE",
    description = "Select sandbox application mode",
    allowed = {
        { "deng", "Build an application for testing the renderer by loading assets" },
        { "imgui", "Build an application for testing ImGui with DENG renderer" },
        { "opengl", "Build a test application for testing OpenGL functionality" },
        { "all", "Build all sandbox applications" },
        { "none", "Do not build any sandbox applications" }
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
    local imgui = require("premake/ImGui")
    imgui.build()

    local libneko = require("premake/Libneko");
    libneko.build()
end


-- Setup build destinations
function buildcfg()
    if _OPTIONS["sandbox-mode"] then

        -- Sandbox application build configuration
        if _OPTIONS["sandbox-mode"] == "deng" or _OPTIONS["sandbox-mode"] == "all" then
            local DENGBox = require("premake/DENGBox")
            DENGBox.build()
        end

        if _OPTIONS["sandbox-mode"] == "imgui" or _OPTIONS["sandbox-mode"] == "all" then
            local imgui_sandbox = require("premake/ImGuiSandbox")
            imgui_sandbox.build()
        end

        if _OPTIONS["sandbox-mode"] == "opengl" or _OPTIONS["sandbox-mode"] == "all" then
            local opengl_sandbox = require("premake/OpenGLSandbox")
            opengl_sandbox.build()
        end
    end

    -- Build DENG runtime library
    local libdeng = require("premake/Libdeng")
    libdeng.build()

    --- Filters for libdeng build regarding sandbox application builds
    filter "options:sandbox-mode=all or options:sandbox-mode=imgui or options:sandbox-mode=opengl or options:sandbox-mode=deng"
        postbuildcommands {
            "{RMDIR} %{cfg.targetdir}/assets",
            "{COPYDIR} assets %{cfg.targetdir}/assets",
            "{RMDIR} %{cfg.targetdir}/shaders",
            "{COPYDIR} shaders %{cfg.targetdir}/shaders",
            "{RMDIR} %{cfg.targetdir}/textures",
            "{COPYDIR} textures %{cfg.targetdir}/textures"
        }

    filter "options:sandbox-mode=none"
        postbuildcommands {
            "{RMDIR} %{cfg.targetdir}/shaders",
            "{COPYDIR} shaders %{cfg.targetdir}/shaders"
        }

    -- Build DENG asset manager application
    local dam = require("premake/dam")
    dam.build()

    -- Build DENG scene editor
    local scene_editor = require("premake/SceneEditor")
    scene_editor.build()
end


-- Script entry point, check if not help
if not _OPTIONS["help"] then
    oscheck()
    optcheck()
    modcheck()
    buildcfg()
end
