# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: ViewModels.cmake - ModelLoader application build config
# author: Karl-Mihkel Ott

set(VIEW_MODELS_TARGET ViewModels)
set(VIEW_MODELS_HEADERS include/Executables/ViewModelsApp.h)
set(VIEW_MODELS_SOURCES src/Executables/ViewModelsApp.cpp)

add_executable(${VIEW_MODELS_TARGET} ${VIEW_MODELS_HEADERS} ${VIEW_MODELS_SOURCES})
add_dependencies(${VIEW_MODELS_TARGET} ${DENG_SHARED_TARGET})
target_link_libraries(${VIEW_MODELS_TARGET} PRIVATE ${DENG_SHARED_TARGET})
