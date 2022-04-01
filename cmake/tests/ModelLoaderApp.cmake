# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: ModelLoaderApp.cmake - ModelLoader application build config
# author: Karl-Mihkel Ott

set(VK_MODEL_LOADER_TARGET VulkanModelLoader)
set(VK_MODEL_LOADER_SOURCES tests/VulkanModelLoader.cpp tests/ModelLoaderApp.h)

set(OGL_MODEL_LOADER_TARGET OpenGLModelLoader)
set(OGL_MODEL_LOADER_SOURCES tests/OpenGLModelLoader.cpp tests/ModelLoaderApp.h)

add_executable(${VK_MODEL_LOADER_TARGET} ${VK_MODEL_LOADER_SOURCES})
add_executable(${OGL_MODEL_LOADER_TARGET} ${OGL_MODEL_LOADER_SOURCES})

add_dependencies(${VK_MODEL_LOADER_TARGET} ${DENG_SHARED_TARGET})
add_dependencies(${OGL_MODEL_LOADER_TARGET} ${DENG_SHARED_TARGET})

target_link_libraries(${VK_MODEL_LOADER_TARGET} PRIVATE ${DENG_SHARED_TARGET})
target_link_libraries(${OGL_MODEL_LOADER_TARGET} PRIVATE ${DENG_SHARED_TARGET})

target_include_directories(${VK_MODEL_LOADER_TARGET} PRIVATE tests)
target_include_directories(${OGL_MODEL_LOADER_TARGET} PRIVATE tests)

add_custom_command(TARGET ${VK_MODEL_LOADER_TARGET} 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/Models
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_directory ${CMAKE_SOURCE_DIR}/tests/Models ${CMAKE_CURRENT_BINARY_DIR}/Models
)

# Write default loader.conf file
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/loader.conf "Models/Box.das")
