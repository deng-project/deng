# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: ViewModels.cmake - ModelLoader application build config
# author: Karl-Mihkel Ott

set(GAME_EDITOR_TARGET deng-game-editor)
set(GAME_EDITOR_HEADERS include/Executables/GameEditor.h)
set(GAME_EDITOR_SOURCES src/Executables/GameEditor.cpp)

add_executable(${GAME_EDITOR_TARGET} ${GAME_EDITOR_HEADERS} ${GAME_EDITOR_SOURCES})
add_dependencies(${GAME_EDITOR_TARGET} ${DENG_SHARED_TARGET})
target_link_libraries(${GAME_EDITOR_TARGET} PRIVATE ${DENG_SHARED_TARGET})

if(UNIX AND NOT MACOS)
    target_link_libraries(${GAME_EDITOR_TARGET} PRIVATE pthread)
endif()
