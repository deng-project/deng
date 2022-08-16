# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: ShaderGenerator.cmake - ModelShaderGenerator class unit test
# author: Karl-Mihkel Ott

set(SHADER_GENERATOR_TARGET "ShaderGenerator")
set(SHADER_GENERATOR_SOURCES "tests/ShaderGenerator.cpp")

set(SHADER_GENERATOR_TEST_MAKER_TARGET "ShaderGeneratorTestMaker")
set(SHADER_GENERATOR_TEST_MAKER_SOURCES "tests/ShaderGeneratorTestMaker.cpp")

add_executable(${SHADER_GENERATOR_TARGET} ${SHADER_GENERATOR_SOURCES})
add_dependencies(${SHADER_GENERATOR_TARGET} ${DENG_COMPLETE_TARGET})
target_link_libraries(${SHADER_GENERATOR_TARGET} PRIVATE ${DENG_COMPLETE_TARGET})

add_executable(${SHADER_GENERATOR_TEST_MAKER_TARGET} ${SHADER_GENERATOR_TEST_MAKER_SOURCES})
