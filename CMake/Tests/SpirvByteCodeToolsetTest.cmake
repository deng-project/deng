# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: SpirvByteCodeToolsetTest.cmake - SPIR-V bytecode toolset test application
# author: Karl-Mihkel Ott

set(SPIRV_BYTE_CODE_TOOLSET_TEST_TARGET SpirvByteCodeToolsetTest)
set(SPIRV_BYTE_CODE_TOOLSET_TEST_HEADERS
    Include/deng/ErrorDefinitions.h
    Include/deng/Exceptions.h
    Include/deng/SpirvByteCodeToolset.h
    Include/deng/spirv_reflect.h)

set(SPIRV_BYTE_CODE_TOOLSET_TEST_SOURCES
    Sources/spirv_reflect.c
    Sources/ErrorDefinitions.cpp
    Sources/SpirvByteCodeToolset.cpp
    Tests/Sources/SpirvByteCodeToolsetTest.cpp)

enable_testing()
find_package(GTest CONFIG REQUIRED)

add_executable(${SPIRV_BYTE_CODE_TOOLSET_TEST_TARGET}
    ${SPIRV_BYTE_CODE_TOOLSET_TEST_HEADERS}
    ${SPIRV_BYTE_CODE_TOOLSET_TEST_SOURCES})
    
target_link_libraries(${SPIRV_BYTE_CODE_TOOLSET_TEST_TARGET} 
    PRIVATE GTest::gtest 
    PRIVATE GTest::gmock)

target_compile_definitions(${SPIRV_BYTE_CODE_TOOLSET_TEST_TARGET}
    PRIVATE DENG_STATIC)
target_include_directories(${SPIRV_BYTE_CODE_TOOLSET_TEST_TARGET}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include)

add_test(${SPIRV_BYTE_CODE_TOOLSET_TEST_TARGET} ${SPIRV_BYTE_CODE_TOOLSET_TEST_TARGET})

set_target_properties(${SPIRV_BYTE_CODE_TOOLSET_TEST_TARGET} PROPERTIES FOLDER "UnitTests")