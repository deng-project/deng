--- DENG: dynamic engine - powerful 3D game engine
--- licence: Apache, see LICENCE file
--- file: GLSLang.lua - GLSLang dependency build config
--- author: Karl-Mihkel Ott

local GLSLang = {}

function GLSLang.build()
    defines { "ENABLE_OPT=1", "GLSLANG_OSINCLUDE_UNIX", "ENABLE_HLSL" }

    project "HLSL"
        kind "StaticLib"
        language "C++"
        cppdialect "C++17"

        files { "deps/shaderc/third_party/glslang/hlsl/stub.cpp" }


    project "OGLCompiler"
        kind "StaticLib"
        language "C++"
        cppdialect "C++17"

        files { "deps/shaderc/third_party/glslang/OGLCompilersDLL/InitializeDll.cpp" }


    project "OSDependent"
        kind "StaticLib"
        language "C++"
        cppdialect "C++17"

        filter "platforms:Win32"
            files { "deps/shaderc/third_party/glslang/glslang/OSDependent/Windows/ossource.cpp" }

        filter "platforms:Linux"
            files { "deps/shaderc/third_party/glslang/glslang/OSDependent/Unix/ossource.cpp" }


    project "GenericCodeGen"
        kind "StaticLib"
        language "C++"
        cppdialect "C++17"

        files {
            "deps/shaderc/third_party/glslang/glslang/GenericCodeGen/CodeGen.cpp",
            "deps/shaderc/third_party/glslang/glslang/GenericCodeGen/Link.cpp"
        }


    project "MachineIndependent"
        kind "StaticLib"
        language "C++"
        cppdialect "C++17"

        includedirs { "deps/shaderc/third_party/glslang/glslang/HLSL" }

        files {
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/glslang.m4",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/glslang.y",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/glslang_tab.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/attribute.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/Constant.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/iomapper.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/InfoSink.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/Initialize.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/IntermTraverse.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/Intermediate.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/ParseContextBase.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/ParseHelper.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/PoolAlloc.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/RemoveTree.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/Scan.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/ShaderLang.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/SpirvIntrinsics.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/SymbolTable.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/Versions.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/intermOut.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/limits.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/linkValidate.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/parseConst.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/reflection.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/preprocessor/Pp.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/preprocessor/PpAtom.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/preprocessor/PpContext.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/preprocessor/PpScanner.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/preprocessor/PpTokens.cpp",
            "deps/shaderc/third_party/glslang/glslang/MachineIndependent/propagateNoContraction.cpp",

            -- HLSL source files --
            "deps/shaderc/third_party/glslang/glslang/HLSL/hlslAttributes.cpp",
            "deps/shaderc/third_party/glslang/glslang/HLSL/hlslParseHelper.cpp",
            "deps/shaderc/third_party/glslang/glslang/HLSL/hlslScanContext.cpp",
            "deps/shaderc/third_party/glslang/glslang/HLSL/hlslOpMap.cpp",
            "deps/shaderc/third_party/glslang/glslang/HLSL/hlslTokenStream.cpp",
            "deps/shaderc/third_party/glslang/glslang/HLSL/hlslGrammar.cpp",
            "deps/shaderc/third_party/glslang/glslang/HLSL/hlslParseables.cpp"
        }


        links { 
            "OGLCompiler",
            "OSDependent",
            "GenericCodeGen"
        }

    project "glslang"
        kind "StaticLib"
        language "C++"
        cppdialect "C++17"

        includedirs { "deps/shaderc/third_party/glslang" }

        files { "deps/shaderc/third_party/glslang/glslang/CInterface/glslang_c_interface.cpp" }

        links {
            "OGLCompiler",
            "OSDependent",
            "MachineIndependent"
        }

    print("Adding glslang dependency")
        
end

return GLSLang
