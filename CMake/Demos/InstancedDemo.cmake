# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: InstancedDemo.cmake - Instanced rendering demo
# author: Karl-Mihkel Ott

set(INSTANCED_DEMO_TARGET InstancedDemo)
set(INSTANCED_DEMO_HEADERS 
	Demos/InstancedDemo/CubeResourceBuilders.h
	Demos/InstancedDemo/InstancedCubeLayer.h
	Demos/InstancedDemo/InstancedDemoApp.h
	Demos/Utils/CameraScript.h
	Demos/Utils/CubeVertices.h)
set(INSTANCED_DEMO_SOURCES
	Demos/InstancedDemo/CubeResourceBuilders.cpp
	Demos/InstancedDemo/InstancedCubeLayer.cpp
	Demos/InstancedDemo/InstancedDemoApp.cpp
	Demos/Utils/CameraScript.cpp)
	
add_executable(${INSTANCED_DEMO_TARGET}
	${INSTANCED_DEMO_HEADERS}
	${INSTANCED_DEMO_SOURCES})
add_dependencies(${INSTANCED_DEMO_TARGET} ${DENG_MINIMAL_TARGET})
target_link_libraries(${INSTANCED_DEMO_TARGET} 
	PRIVATE ${DENG_MINIMAL_TARGET})
set_target_properties(${INSTANCED_DEMO_TARGET} PROPERTIES FOLDER ${DEMO_APPS_DIR})