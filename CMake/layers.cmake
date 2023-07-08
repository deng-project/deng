# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: layers.cmake - premade layers library configuration
# author: Karl-Mihkel Ott

set(LAYERS_TARGET deng-layers)
set(LAYERS_HEADERS
	Include/deng/Layers/CameraScript.h
	Include/deng/Layers/CubeResourceBuilders.h
	Include/deng/Layers/CubeVertices.h
	Include/deng/Layers/GrassLayer.h
	Include/deng/Layers/GrassResourceBuilders.h
	Include/deng/Layers/Layers.h
	Include/deng/Layers/LightSourceBuilders.h
	Include/deng/Layers/PBRLayer.h
	Include/deng/Layers/PBRResourceBuilders.h)
	
set(LAYERS_SOURCES
	Sources/Layers/CameraScript.cpp
	Sources/Layers/CubeResourceBuilders.cpp
	Sources/Layers/GrassLayer.cpp
	Sources/Layers/GrassResourceBuilders.cpp
	Sources/Layers/LightSourceBuilders.cpp
	Sources/Layers/PBRLayer.cpp
	Sources/Layers/PBRResourceBuilders.cpp)
	
add_library(${LAYERS_TARGET} STATIC
	${LAYERS_HEADERS}
	${LAYERS_SOURCES})
	
add_dependencies(${LAYERS_TARGET}
	${DENG_MINIMAL_TARGET})
	
target_link_libraries(${LAYERS_TARGET}
	PUBLIC ${DENG_MINIMAL_TARGET})