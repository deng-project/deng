--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: Spirv.lua - libSPIRV dependency build configuration
--- author: Karl-Mihkel Ott

local Spirv = {}

function Spirv.build()
    project "SPIRV"
        kind "StaticLib"
        language "C++"
        cppdialect "C++17"

        includedirs { "deps/shaderc/third_party/glslang" }

        files {
            "deps/shaderc/third_party/glslang/SPIRV/GlslangToSpv.cpp",
            "deps/shaderc/third_party/glslang/SPIRV/InReadableOrder.cpp",
            "deps/shaderc/third_party/glslang/SPIRV/Logger.cpp",
            "deps/shaderc/third_party/glslang/SPIRV/SpvBuilder.cpp",
            "deps/shaderc/third_party/glslang/SPIRV/SpvPostProcess.cpp",
            "deps/shaderc/third_party/glslang/SPIRV/doc.cpp",
            "deps/shaderc/third_party/glslang/SPIRV/SpvTools.cpp",
            "deps/shaderc/third_party/glslang/SPIRV/disassemble.cpp",
            "deps/shaderc/third_party/glslang/SPIRV/CInterface/spirv_c_interface.cpp"
        }

        links { "MachineIndependent", "SPIRV-Tools-opt" }
end

return Spirv
