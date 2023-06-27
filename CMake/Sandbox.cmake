# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: layers.cmake - premade layers library configuration
# author: Karl-Mihkel Ott

set(SANDBOX_TARGET Sandbox)
set(SANDBOX_HEADERS
	Include/deng/Sandbox/Sandbox.h)
set(SANDBOX_SOURCES
	Sources/Sandbox/Sandbox.cpp)
	
add_executable(${SANDBOX_TARGET}
	${SANDBOX_HEADERS}
	${SANDBOX_SOURCES})
	
add_dependencies(${SANDBOX_TARGET}
	${LAYERS_TARGET})
	
target_link_libraries(${SANDBOX_TARGET}
	PRIVATE ${LAYERS_TARGET})