# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: BasicLightingTest.cmake - Basic lighting test program configuration
# author: Karl-Mihkel Ott

set(BASIC_LIGHTING_TEST_TARGET BasicLightingTest)
set(BASIC_LIGHTING_TEST_SOURCES Tests/BasicLightingTest.cpp)

add_executable(${BASIC_LIGHTING_TEST_TARGET} ${BASIC_LIGHTING_TEST_SOURCES})
add_dependencies(${BASIC_LIGHTING_TEST_TARGET} ${DENG_MINIMAL_TARGET})
target_link_libraries(${BASIC_LIGHTING_TEST_TARGET} 
	PRIVATE ${DENG_MINIMAL_TARGET})
set_target_properties(${BASIC_LIGHTING_TEST_TARGET} PROPERTIES FOLDER ${MANUAL_TEST_DIR})