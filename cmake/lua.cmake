# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: liblua.cmake - Lua interpreter library build configuration
# author: Karl-Mihkel Ott

set(LIBLUA_DEP_DIR lua-5.4.4)
set(LIBLUA_TARGET lua)
set(LIBLUA_SOURCES 
    deps/${LIBLUA_DEP_DIR}/src/lapi.c 
    deps/${LIBLUA_DEP_DIR}/src/lcode.c
    deps/${LIBLUA_DEP_DIR}/src/lctype.c
    deps/${LIBLUA_DEP_DIR}/src/ldebug.c
    deps/${LIBLUA_DEP_DIR}/src/ldo.c
    deps/${LIBLUA_DEP_DIR}/src/ldump.c
    deps/${LIBLUA_DEP_DIR}/src/lfunc.c
    deps/${LIBLUA_DEP_DIR}/src/lgc.c
    deps/${LIBLUA_DEP_DIR}/src/llex.c
    deps/${LIBLUA_DEP_DIR}/src/lmem.c
    deps/${LIBLUA_DEP_DIR}/src/lobject.c
    deps/${LIBLUA_DEP_DIR}/src/lopcodes.c
    deps/${LIBLUA_DEP_DIR}/src/lparser.c
    deps/${LIBLUA_DEP_DIR}/src/lstate.c
    deps/${LIBLUA_DEP_DIR}/src/lstring.c
    deps/${LIBLUA_DEP_DIR}/src/ltable.c
    deps/${LIBLUA_DEP_DIR}/src/ltm.c
    deps/${LIBLUA_DEP_DIR}/src/lundump.c
    deps/${LIBLUA_DEP_DIR}/src/lvm.c
    deps/${LIBLUA_DEP_DIR}/src/lzio.c
    deps/${LIBLUA_DEP_DIR}/src/lauxlib.c
    deps/${LIBLUA_DEP_DIR}/src/lbaselib.c
    deps/${LIBLUA_DEP_DIR}/src/lcorolib.c
    deps/${LIBLUA_DEP_DIR}/src/ldblib.c
    deps/${LIBLUA_DEP_DIR}/src/liolib.c
    deps/${LIBLUA_DEP_DIR}/src/lmathlib.c
    deps/${LIBLUA_DEP_DIR}/src/loadlib.c
    deps/${LIBLUA_DEP_DIR}/src/loslib.c
    deps/${LIBLUA_DEP_DIR}/src/lstrlib.c
    deps/${LIBLUA_DEP_DIR}/src/ltablib.c
    deps/${LIBLUA_DEP_DIR}/src/lutf8lib.c
    deps/${LIBLUA_DEP_DIR}/src/linit.c
)

set(LIBLUA_HEADERS
    deps/${LIBLUA_DEP_DIR}/src/lapi.h
    deps/${LIBLUA_DEP_DIR}/src/lauxlib.h
    deps/${LIBLUA_DEP_DIR}/src/lcode.h
    deps/${LIBLUA_DEP_DIR}/src/lctype.h
    deps/${LIBLUA_DEP_DIR}/src/ldebug.h
    deps/${LIBLUA_DEP_DIR}/src/ldo.h
    deps/${LIBLUA_DEP_DIR}/src/lfunc.h
    deps/${LIBLUA_DEP_DIR}/src/lgc.h
    deps/${LIBLUA_DEP_DIR}/src/ljumptab.h
    deps/${LIBLUA_DEP_DIR}/src/llex.h
    deps/${LIBLUA_DEP_DIR}/src/llimits.h
    deps/${LIBLUA_DEP_DIR}/src/lmem.h
    deps/${LIBLUA_DEP_DIR}/src/lobject.h
    deps/${LIBLUA_DEP_DIR}/src/lopcodes.h
    deps/${LIBLUA_DEP_DIR}/src/lopnames.h
    deps/${LIBLUA_DEP_DIR}/src/lparser.h
    deps/${LIBLUA_DEP_DIR}/src/lprefix.h
    deps/${LIBLUA_DEP_DIR}/src/lstate.h
    deps/${LIBLUA_DEP_DIR}/src/lstring.h
    deps/${LIBLUA_DEP_DIR}/src/ltable.h
    deps/${LIBLUA_DEP_DIR}/src/ltm.h
    deps/${LIBLUA_DEP_DIR}/src/luaconf.h
    deps/${LIBLUA_DEP_DIR}/src/lua.h
    deps/${LIBLUA_DEP_DIR}/src/lualib.h
    deps/${LIBLUA_DEP_DIR}/src/lundump.h
    deps/${LIBLUA_DEP_DIR}/src/lvm.h
    deps/${LIBLUA_DEP_DIR}/src/lzio.h
)

add_library(${LIBLUA_TARGET} STATIC
    ${LIBLUA_SOURCES}
    ${LIBLUA_HEADERS}
)


target_compile_definitions(${LIBLUA_TARGET} PUBLIC LUA_COMPAT_5_3)

# Add windows specific compile definitions
if(WIN32)
    target_compile_definitions(${LIBLUA_TARGET} PUBLIC LUA_USE_WINDOWS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(${LIBLUA_TARGET} PUBLIC LUA_USE_LINUX)
endif()
