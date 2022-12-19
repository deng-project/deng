# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: pydeng.cmake - pydeng 
# author: Karl-Mihkel Ott

# Pydeng API has following modules:
# Pydeng.Animations
# Pydeng.Collections
# Pydeng.ComponentFinder
# Pydeng.Configuration
# Pydeng.DynamicAssetLoader
# Pydeng.EventManager
# Pydeng.Framebuffer
# Pydeng.Networking
# Pydeng.SceneLoader
# Pydeng.SoundPlayer
# Pydeng.VideoPlayer

set(PYDENG_TARGET pydeng)
set(PYDENG_HEADERS
	include/deng/Pydeng/Animations.h
	include/deng/Pydeng/Collections.h
	include/deng/Pydeng/DataQuery.h
	include/deng/Pydeng/Configuration.h
	include/deng/Pydeng/DynamicAssetLoader.h
	include/deng/Pydeng/EmbedModules.h
	include/deng/Pydeng/EventManager.h
	include/deng/Pydeng/Framebuffer.h
	include/deng/Pydeng/Networking.h
	include/deng/Pydeng/SceneLoader.h
	include/deng/Pydeng/Scriptable.h
	include/deng/Pydeng/SoundPlayer.h
	include/deng/Pydeng/VideoPlayer.h)
	
set(PYDENG_SRC
	src/Pydeng/Animations.cpp
	src/Pydeng/Collections.cpp
	src/Pydeng/DataQuery.cpp
	src/Pydeng/Configuration.cpp
	src/Pydeng/DynamicAssetLoader.cpp
	src/Pydeng/EventManager.cpp
	src/Pydeng/Framebuffer.cpp
	src/Pydeng/Networking.cpp
	src/Pydeng/SceneLoader.cpp
	src/Pydeng/SoundPlayer.cpp
	src/Pydeng/VideoPlayer.cpp)
	
add_library(${PYDENG_TARGET} STATIC
	${PYDENG_HEADERS}
	${PYDENG_SRC})

add_dependencies(${PYDENG_TARGET} ${DENG_COMPLETE_TARGET})
target_link_libraries(${PYDENG_TARGET}
	PUBLIC ${DENG_COMPLETE_TARGET}
	PUBLIC pybind11::embed)
	
if(WIN32)
	# For debug build
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/deps/vcpkg/installed/x64-windows/tools/python3/DLLs
		DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Python
	)
	
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/deps/vcpkg/installed/x64-windows/tools/python3/Lib
		DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Python
	)
	
	# For release build
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/deps/vcpkg/installed/x64-windows/tools/python3/DLLs
		DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Python
	)
	
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/deps/vcpkg/installed/x64-windows/tools/python3/Lib
		DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Python
	)
endif()