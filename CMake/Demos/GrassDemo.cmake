# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: GrassDemo.cmake - Grass rendering program
# author: Karl-Mihkel Ott

set(GRASS_DEMO_TARGET GrassDemo)
set(GRASS_DEMO_HEADERS
	Demos/GrassDemo/GrassApp.h
	Demos/GrassDemo/GrassLayer.h
	Demos/GrassDemo/GrassResourceBuilders.h
	Demos/Utils/CameraScript.h)
	
set(GRASS_DEMO_SOURCES
	Demos/GrassDemo/GrassApp.cpp
	Demos/GrassDemo/GrassLayer.cpp
	Demos/GrassDemo/GrassResourceBuilders.cpp
	Demos/Utils/CameraScript.cpp)
	
add_executable(${GRASS_DEMO_TARGET}
	${GRASS_DEMO_HEADERS}
	${GRASS_DEMO_SOURCES})
	
add_dependencies(${GRASS_DEMO_TARGET} ${DENG_MINIMAL_TARGET})
target_link_libraries(${GRASS_DEMO_TARGET} 
	PRIVATE ${DENG_MINIMAL_TARGET})
set_target_properties(${GRASS_DEMO_TARGET} PROPERTIES FOLDER ${DEMO_APPS_DIR})