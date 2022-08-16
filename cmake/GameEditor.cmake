# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: ViewModels.cmake - ModelLoader application build config
# author: Karl-Mihkel Ott

set(GAME_EDITOR_TARGET deng-game-editor)
set(GAME_EDITOR_HEADERS include/deng/Executables/GameEditor.h)
set(GAME_EDITOR_SOURCES src/Executables/GameEditor.cpp)

add_executable(${GAME_EDITOR_TARGET} ${GAME_EDITOR_HEADERS} ${GAME_EDITOR_SOURCES})
add_dependencies(${GAME_EDITOR_TARGET} ${DENG_COMPLETE_TARGET})
target_link_libraries(${GAME_EDITOR_TARGET} PRIVATE ${DENG_COMPLETE_TARGET})

# Copy BackendChooser.py to build directory
add_custom_command(TARGET ${GAME_EDITOR_TARGET}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy ${CMAKE_SOURCE_DIR}/scripts/BackendChooser.py ${CMAKE_CURRENT_BINARY_DIR}
)
