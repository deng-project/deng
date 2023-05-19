# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: BulletHelloWorld.cmake - Bullet test application CMake configuration file
# author: Karl-Mihkel Ott

set(BULLET_HELLO_WORLD BulletHelloWorld)
set(BULLET_HELLO_WORLD_SRC
	tests/BulletHelloWorld/BulletHelloWorld.cpp)
	
add_executable(${BULLET_HELLO_WORLD} ${BULLET_HELLO_WORLD_SRC})

find_package(Bullet CONFIG REQUIRED)
target_link_libraries(${BULLET_HELLO_WORLD}
	PRIVATE ${BULLET_LIBRARIES})
	
set_target_properties(${BULLET_HELLO_WORLD} PROPERTIES FOLDER ${MANUAL_TEST_DIR})