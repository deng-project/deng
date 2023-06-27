# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: layers.cmake - premade layers library configuration
# author: Karl-Mihkel Ott

set(LAYERS_TARGET layers)
set(LAYERS_HEADERS
	Include/deng/Layers/BasicLightingLayer.h
	Include/deng/Layers/CameraScript.h
	Include/deng/Layers/CubeResourceBuilders.h
	Include/deng/Layers/CubeVertices.h
	Include/deng/Layers/GrassLayer.h
	Include/deng/Layers/GrassResourceBuilders.h
	Include/deng/Layers/InstancedCubeLayer.h
	Include/deng/Layers/LightSourceBuilders.h)
	
set(LAYERS_SOURCES
	Sources/Layers/BasicLightingLayer.cpp
	Sources/Layers/CameraScript.cpp
	Sources/Layers/CubeResourceBuilders.cpp
	Sources/Layers/GrassLayer.cpp
	Sources/Layers/GrassResourceBuilders.cpp
	Sources/Layers/InstancedCubeLayer.cpp
	Sources/Layers/LightSourceBuilders.cpp)
	
add_library(${LAYERS_TARGET} STATIC
	${LAYERS_HEADERS}
	${LAYERS_SOURCES})
	
add_dependencies(${LAYERS_TARGET}
	${DENG_MINIMAL_TARGET})
	
target_link_libraries(${LAYERS_TARGET}
	PUBLIC ${DENG_MINIMAL_TARGET})