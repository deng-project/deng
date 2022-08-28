# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: rt.cmake - DENG runtime executable binary build configuration
# author: Karl-Mihkel Ott

set(RT_TARGET rt)
set(RT_HEADERS
	include/deng/Executables/rt/RuntimeExecutable.h)
	
set(RT_SOURCES 
	src/Executables/rt/RuntimeExecutable.cpp)

add_executable(${RT_TARGET} ${RT_HEADERS} ${RT_SOURCES})
add_dependencies(${RT_TARGET} ${DENG_COMPLETE_TARGET})
target_link_libraries(${RT_TARGET} PRIVATE ${DENG_COMPLETE_TARGET})