# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: CVarTest.cmake - CVar test programm
# author: Karl-Mihkel Ott

set(CVAR_TEST_TARGET CVarTest)
	
set(CVAR_TEST_SOURCES
	Demos/CVarTest.cpp)
	
add_executable(${CVAR_TEST_TARGET}
	${CVAR_TEST_SOURCES})
	
add_dependencies(${CVAR_TEST_TARGET} ${DENG_MINIMAL_TARGET})
target_link_libraries(${CVAR_TEST_TARGET} 
	PRIVATE ${DENG_MINIMAL_TARGET})
set_target_properties(${CVAR_TEST_TARGET} PROPERTIES FOLDER ${DEMO_APPS_DIR})