# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: ODETest.cmake - Small test application for Open Dynamics Engine
# author: Karl-Mihkel Ott

set(ODE_TEST_TARGET odetest)
set(ODE_TEST_SOURCES tests/ODETest.cpp)

add_executable(${ODE_TEST_TARGET}
	${ODE_TEST_SOURCES})

target_link_directories(${ODE_TEST_TARGET}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/ode-0.16.2/lib)
	
target_link_libraries(${ODE_TEST_TARGET}
	PRIVATE drawstuff
	PRIVATE ode_doubles
	PRIVATE opengl32
	PRIVATE glu32
	PRIVATE winmm)
	
target_include_directories(${ODE_TEST_TARGET}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/ode-0.16.2/include)
set_target_properties(${ODE_TEST_TARGET} PROPERTIES FOLDER ${MANUAL_TEST_DIR})