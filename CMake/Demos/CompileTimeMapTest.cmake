# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: CompileTimeMapTest.cmake - Compile time map test
# author: Karl-Mihkel Ott

set(COMPILE_TIME_MAP_TEST_TARGET CompileTimeMapTest)
set(COMPILE_TIME_MAP_TEST_SOURCES 
	Demos/CompileTimeMapTest.cpp)

add_executable(${COMPILE_TIME_MAP_TEST_TARGET} 
	${COMPILE_TIME_MAP_TEST_SOURCES})

target_include_directories(${COMPILE_TIME_MAP_TEST_TARGET}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
set_target_properties(${COMPILE_TIME_MAP_TEST_TARGET} PROPERTIES FOLDER ${DEMO_APPS_DIR})