# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: BasicLightingTest.cmake - Basic lighting test program configuration
# author: Karl-Mihkel Ott

set(BASIC_LIGHTING_DEMO_TARGET BasicLightingDemo)
set(BASIC_LIGHTING_DEMO_HEADERS 
	Demos/BasicLightingDemo/BasicLightingDemo.h
	Demos/BasicLightingDemo/BasicLightingLayer.h
	Demos/Utils/CameraScript.h
	Demos/Utils/CubeVertices.h)
set(BASIC_LIGHTING_DEMO_SOURCES 
	Demos/BasicLightingDemo/BasicLightingDemo.cpp
	Demos/BasicLightingDemo/BasicLightingLayer.cpp
	Demos/Utils/CameraScript.cpp)

add_executable(${BASIC_LIGHTING_DEMO_TARGET} 
	${BASIC_LIGHTING_DEMO_HEADERS}
	${BASIC_LIGHTING_DEMO_SOURCES})

add_dependencies(${BASIC_LIGHTING_DEMO_TARGET} ${DENG_MINIMAL_TARGET})
target_link_libraries(${BASIC_LIGHTING_DEMO_TARGET} 
	PRIVATE ${DENG_MINIMAL_TARGET})
set_target_properties(${BASIC_LIGHTING_DEMO_TARGET} PROPERTIES FOLDER ${DEMO_APPS_DIR})