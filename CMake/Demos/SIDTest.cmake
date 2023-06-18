# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: SIDTest.cmake - Compile time string hashing algorithm test
# author: Karl-Mihkel Ott

set(SID_TEST_TARGET SIDTest)
set(SID_TEST_SOURCES 
	Demos/SIDTest.cpp)

add_executable(${SID_TEST_TARGET} 
	${SID_TEST_SOURCES})

target_include_directories(${SID_TEST_TARGET}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include)
set_target_properties(${SID_TEST_TARGET} PROPERTIES FOLDER ${DEMO_APPS_DIR})