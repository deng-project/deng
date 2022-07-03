# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: CubicsInterp.cmake - Cubic spline test config
# author: Karl-Mihkel Ott

set(CUBICS_INTERP_TARGET "CubicsInterp")
set(CUBIBS_INTERP_SOURCES "tests/CubicsInterp.cpp")

add_executable(${CUBICS_INTERP_TARGET} ${CUBIBS_INTERP_SOURCES})
target_link_libraries(${CUBICS_INTERP_TARGET} ${DENG_TARGET})
