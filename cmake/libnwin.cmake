# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: libnwin.cmake - libnwin static library build configuration
# author: Karl-Mihkel Ott

set(LIBNWIN_TARGET nwin)
set(LIBNWIN_HEADERS
    deps/nekowin/include/key_ev.h
    deps/nekowin/include/key_translation.h
    deps/nekowin/include/napi.h
    deps/nekowin/include/nekodll.h
    deps/nekowin/include/nwin.h
    deps/nekowin/include/win32_window.h
    deps/nekowin/include/window.h
    deps/nekowin/include/x11_window.h
)

set(LIBNWIN_SOURCES
    deps/nekowin/src/glad.c
    deps/nekowin/src/key_ev.c
    deps/nekowin/src/key_translation.c
    deps/nekowin/src/napi.c
)

# Add platform specific source files
if(WIN32)
    list(APPEND LIBNWIN_SOURCES deps/nekowin/src/win32_window.c)
elseif(UNIX)
    list(APPEND LIBNWIN_SOURCES deps/nekowin/src/x11_window.c)
endif()


add_library(${LIBNWIN_TARGET} STATIC
    ${LIBNWIN_HEADERS}
    ${LIBNWIN_SOURCES}
)

target_include_directories(${LIBNWIN_TARGET}
    PRIVATE deps/nekowin/include
    PUBLIC deps/nekowin/include/third_party
)

target_compile_definitions(${LIBNWIN_TARGET}
    PRIVATE LIBNWIN_EXPORT_LIBRARY
    PRIVATE LIBNWIN_STATIC
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${LIBNWIN_TARGET} PRIVATE _DEBUG)
endif()

# Link platform specific libraries
if(UNIX)
    target_link_libraries(${LIBNWIN_TARGET}
        PUBLIC X11
        PUBLIC Xcursor
        PUBLIC GL
        PUBLIC dl
        PUBLIC vulkan
    )
elseif(WIN32)
    target_link_libraries(${LIBNWIN_TARGET}
        PUBLIC vulkan-1
        PUBLIC gdi32
        PUBLIC kernel32
    )
endif()

