# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: LuaTest.cmake - LuaTest application build config
# author: Karl-Mihkel Ott

set(LUATEST_TARGET LuaTest)
set(LUATEST_SOURCES tests/LuaTest.cpp)
set(LUATEST_HEADERS "")

add_executable(${LUATEST_TARGET} ${LUATEST_SOURCES} ${LUATEST_HEADERS})
add_dependencies(${LUATEST_TARGET} ${DENG_SHARED_TARGET})

target_link_libraries(${LUATEST_TARGET} PRIVATE ${DENG_SHARED_TARGET})
