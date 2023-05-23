# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: SDLTriangleApp.cmake - Configuration for simple triangle app using VulkanRenderer and SDLWindowContext 
# author: Karl-Mihkel Ott

set(SDL_TRIANGLE_APP_TARGET SDLTriangleApp)
set(SDL_TRIANGLE_APP_SOURCES
	tests/SDLTriangleApp.cpp)
	
add_executable(${SDL_TRIANGLE_APP_TARGET} ${SDL_TRIANGLE_APP_SOURCES})
add_dependencies(${SDL_TRIANGLE_APP_TARGET} ${DENG_MINIMAL_TARGET})
target_link_libraries(${SDL_TRIANGLE_APP_TARGET} 
	PRIVATE ${DENG_MINIMAL_TARGET})
set_target_properties(${SDL_TRIANGLE_APP_TARGET} PROPERTIES FOLDER ${MANUAL_TEST_DIR})