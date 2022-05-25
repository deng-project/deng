# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: libdas.cmake - Libdas CMake configuration file
# author: Karl-Mihkel Ott

set(LIBDAS_TARGET das)
set(LIBDAS_HEADERS
    deps/libdas/include/Algorithm.h
    deps/libdas/include/Api.h
    deps/libdas/include/AsciiLineReader.h
    deps/libdas/include/AsciiStreamReader.h
    deps/libdas/include/Base64Decoder.h
    deps/libdas/include/BufferImageTypeResolver.h
    deps/libdas/include/DasParser.h
    deps/libdas/include/DasReaderCore.h
    deps/libdas/include/DasStructures.h
    deps/libdas/include/DASTool.h
    deps/libdas/include/DasWriterCore.h
    deps/libdas/include/Debug.h
    deps/libdas/include/ErrorHandlers.h
    deps/libdas/include/GLTFCompiler.h
    deps/libdas/include/GLTFParser.h
    deps/libdas/include/GLTFStructures.h
    deps/libdas/include/Hash.h
    deps/libdas/include/HuffmanCompression.h
    deps/libdas/include/Iterators.h
    deps/libdas/include/JSONParser.h
    deps/libdas/include/LibdasAssert.h
    deps/libdas/include/Libdas.h
    deps/libdas/include/Matrix.h
    deps/libdas/include/Points.h
    deps/libdas/include/Quaternion.h
    deps/libdas/include/SIMD.h
    deps/libdas/include/stb_image.h
    deps/libdas/include/STLCompiler.h
    deps/libdas/include/STLParser.h
    deps/libdas/include/STLStructures.h
    deps/libdas/include/TextureReader.h
    deps/libdas/include/URIResolver.h
    deps/libdas/include/Vector.h
    deps/libdas/include/WavefrontObjCompiler.h
    deps/libdas/include/WavefrontObjParser.h
    deps/libdas/include/WavefrontObjStructures.h
)

set(LIBDAS_SOURCES
    deps/libdas/src/Algorithm.cpp
    deps/libdas/src/AsciiLineReader.cpp
    deps/libdas/src/AsciiStreamReader.cpp
    deps/libdas/src/Base64Decoder.cpp
    deps/libdas/src/BufferImageTypeResolver.cpp
    deps/libdas/src/DasParser.cpp
    deps/libdas/src/DasReaderCore.cpp
    deps/libdas/src/DasStructures.cpp
    deps/libdas/src/DasWriterCore.cpp
    deps/libdas/src/Debug.cpp
    deps/libdas/src/ErrorHandlers.cpp
    deps/libdas/src/GLTFCompiler.cpp
    deps/libdas/src/GLTFParser.cpp
    deps/libdas/src/Hash.cpp
    deps/libdas/src/HuffmanCompression.cpp
    deps/libdas/src/interpolation
    deps/libdas/src/JSONParser.cpp
    deps/libdas/src/STLCompiler.cpp
    deps/libdas/src/STLParser.cpp
    deps/libdas/src/STLStructures.cpp
    deps/libdas/src/TextureReader.cpp
    deps/libdas/src/URIResolver.cpp
    deps/libdas/src/WavefrontObjCompiler.cpp
    deps/libdas/src/WavefrontObjParser.cpp
    deps/libdas/src/WavefrontObjStructures.cpp
)

add_library(${LIBDAS_TARGET} STATIC 
    ${LIBDAS_HEADERS} 
    ${LIBDAS_SOURCES}
)

target_compile_definitions(${LIBDAS_TARGET}
    PRIVATE LIBDAS_EXPORT_LIBRARY
    PUBLIC LIBDAS_STATIC
)


# DASTool build config
set(DASTOOL_TARGET dastool)
set(DASTOOL_SOURCES 
    deps/libdas/include/DASTool.h
    deps/libdas/src/DASTool.cpp
)

add_executable(${DASTOOL_TARGET} ${DASTOOL_SOURCES})
add_dependencies(${DASTOOL_TARGET} ${LIBDAS_TARGET})
target_link_libraries(${DASTOOL_TARGET} PRIVATE ${LIBDAS_TARGET})

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${LIBDAS_TARGET} PRIVATE _DEBUG)
endif()

target_include_directories(${LIBDAS_TARGET} PRIVATE deps/libdas/include)
install(FILES)
