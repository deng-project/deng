--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: ShaderC.lua - Shaderc dependency build configuration
--- author: Karl-Mihkel Ott

local ShaderC = {}

function ShaderC.build()
    -- Sync all shaderc dependencies
    os.execute("python deps/shaderc/utils/git-sync-deps")

    -- Require shaderc dependencies to be built --
    local glslang = require("premake/GLSLang")
    glslang.build()

    -- Require spirv-tools dependencies to be built --
    local spirv_tools = require("premake/SpirvTools")
    spirv_tools.build()

    -- Require SPRIV library to be built --
    local spirv = require("premake/Spirv")
    spirv.build()

    -- Build libshaderc_util library --
    project "shaderc_util"
        kind "StaticLib"
        language "C++"
        cppdialect "C++17"

        includedirs {
            "deps/shaderc/libshaderc_util/include",
            "deps/shaderc/libshaderc_util/include",
            "deps/shaderc/third_party/glslang"
        }

        defines { "ENABLE_HLSL" }

        files {
            "deps/shaderc/libshaderc_util/src/args.cc",
            "deps/shaderc/libshaderc_util/src/compiler.cc",
            "deps/shaderc/libshaderc_util/src/file_finder.cc",
            "deps/shaderc/libshaderc_util/src/io_shaderc.cc",
            "deps/shaderc/libshaderc_util/src/message.cc",
            "deps/shaderc/libshaderc_util/src/resources.cc",
            "deps/shaderc/libshaderc_util/src/shader_stage.cc",
            "deps/shaderc/libshaderc_util/src/spirv_tools_wrapper.cc",
            "deps/shaderc/libshaderc_util/src/version_profile.cc"
        }

        links {
            "glslang",
            "OSDependent",
            "OGLCompiler",
            "HLSL",
            "glslang",
            "SPIRV",
            "SPIRV-Tools-opt"
        }


    -- Static shaderc library build --
    project "shaderc_shared"
        kind "SharedLib"
        language "C++"
        cppdialect "C++17"

        defines { 
            "SHADERC_IMPLEMENTATION",
            "SHADERC_SHAREDLIB"
        }

        files {
            "deps/shaderc/libshaderc/include/shaderc/shaderc.h",
            "deps/shaderc/libshaderc/include/shaderc/shaderc.hpp",
            "deps/shaderc/libshaderc/src/shaderc.cc",
            "deps/shaderc/libshaderc/src/shaderc_private.h",
        }

        includedirs {
            "deps/shaderc/libshaderc/include",
            "deps/shaderc/libshaderc_util/include",
            "deps/shaderc/third_party/glslang",
            "deps/shaderc/third_party/spirv-headers/include"
        }

        links {
            "glslang",
            "OSDependent",
            "OGLCompiler",
            "glslang",
            "pthread",
            "shaderc_util",
            "SPIRV",
            "SPIRV-Tools"
        }
end

return ShaderC
