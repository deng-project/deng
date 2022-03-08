# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: imgui.cmake - ImGui cmake configuration file
# author: Karl-Mihkel Ott

set(IMGUI_TARGET imgui)
set(IMGUI_HEADERS
    deps/imgui/imgui.h
    deps/imgui/imconfig.h
    deps/imgui/imgui_internal.h
    deps/imgui/imstb_rectpack.h
    deps/imgui/imstb_textedit.h
    deps/imgui/imstb_textedit.h
    deps/imgui/imstb_truetype.h
)

set(IMGUI_SOURCES
    deps/imgui/imgui.cpp
    deps/imgui/imgui_demo.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp
)

add_library(${IMGUI_TARGET} STATIC
    ${IMGUI_HEADERS}
    ${IMGUI_SOURCES}
)

# Make sure that imgui index type corresponds to uint32_t
target_compile_definitions(${IMGUI_TARGET}
    PUBLIC ImDrawIdx=unsigned\ int)

target_include_directories(${IMGUI_TARGET} 
    PUBLIC deps/imgui)
