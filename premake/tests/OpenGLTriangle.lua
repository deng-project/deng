--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: OpenGLTriangle.lua - OpenGL triangle application build test
--- author: Karl-Mihkel Ott

local OpenGLTriangle = {}

function OpenGLTriangle.build()
    project "OpenGLTriangle"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"

        includedirs { "include", "deps", "./deps/nekowin/include/third_party" }

        files {
            "include/*.h",
            "src/OpenGLBufferLoader.cpp",
            "src/OpenGLShaderLoader.cpp",
            "src/OpenGLRenderer.cpp",
            "src/VulkanDescriptorPoolCreator.cpp",
            "src/VulkanDescriptorSetLayoutCreator.cpp",
            "src/VulkanDescriptorSetsCreator.cpp",
            "src/VulkanHelpers.cpp",
            "src/VulkanInstanceCreator.cpp",
            "src/VulkanPipelineCreator.cpp",
            "src/VulkanRenderer.cpp",
            "src/VulkanSwapchainCreator.cpp",
            "src/VulkanUniformBufferAllocator.cpp",
            "tests/OpenGLTriangle.cpp",
            "src/Window.cpp",
        }

        links { "shaderc_combined", "nwin", "pthread" }

        filter "platforms:Linux"
            links { "GL", "GLX", "X11", "Xcursor", "dl", "vulkan" }
        filter "platforms:Win32"
            libdirs { "C:/VulkanSDK/**/Lib" }
            includedirs { "C:/VulkanSDK/**/Include" }
end

return OpenGLTriangle
