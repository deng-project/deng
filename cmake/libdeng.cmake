# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: deng.cmake - deng CMake configuration file
# author: Karl-Mihkel Ott

set(DENG_SHARED_TARGET deng-shared)
set(DENG_STATIC_TARGET deng-static)
set(DENG_HEADERS
    include/AnimationSampler.h
    include/Api.h
    include/BaseTypes.h
    include/BufferAlignment.h
    include/Camera3D.h
    include/EditorCamera.h
    include/ErrorDefinitions.h
    include/FirstPersonCamera.h
    include/GPUMemoryManager.h
    include/ImGuiLayer.h
    include/MeshLoader.h
    include/Missing.h
    include/ModelLoader.h
    include/ModelShaderManager.h
    include/ModelShaderGenerator.h
    include/ModelUniforms.h
    include/NodeLoader.h
    include/OpenGLBufferLoader.h
    include/OpenGLRenderer.h
    include/OpenGLShaderLoader.h
    include/PointViewer.h
    include/PythonScriptExecutor.h
    include/Renderer.h
    include/SceneLoader.h
    include/ShaderDefinitions.h
    include/SkeletonDataManager.h
    include/ThirdPersonCamera.h
    include/VulkanDescriptorAllocator.h
    include/VulkanDescriptorSetLayoutCreator.h
    include/VulkanHelpers.h
    include/VulkanInstanceCreator.h
    include/VulkanPipelineCreator.h
    include/VulkanRenderer.h
    include/VulkanSwapchainCreator.h
    include/Window.h
)

set(DENG_SOURCES
    src/AnimationSampler.cpp
    src/Camera3D.cpp
    src/EditorCamera.cpp
    src/FirstPersonCamera.cpp
    src/GPUMemoryManager.cpp
    src/ImGuiLayer.cpp
    src/MeshLoader.cpp
    src/Missing.cpp
    src/ModelLoader.cpp
    src/ModelShaderManager.cpp
    src/ModelShaderGenerator.cpp
    src/NodeLoader.cpp
    src/OpenGLBufferLoader.cpp
    src/OpenGLRenderer.cpp
    src/OpenGLShaderLoader.cpp
    src/PointViewer.cpp
    src/PythonScriptExecutor.cpp
    src/Renderer.cpp
    src/SceneLoader.cpp
    src/ShaderDefinitions.cpp
    src/SkeletonDataManager.cpp
    src/ThirdPersonCamera.cpp
    src/VulkanDescriptorAllocator.cpp
    src/VulkanDescriptorSetLayoutCreator.cpp
    src/VulkanHelpers.cpp
    src/VulkanInstanceCreator.cpp
    src/VulkanPipelineCreator.cpp
    src/VulkanRenderer.cpp
    src/VulkanSwapchainCreator.cpp
    src/Window.cpp
)


# Static library configuration
add_library(${DENG_STATIC_TARGET} STATIC
    ${DENG_HEADERS}
    ${DENG_SOURCES}
)

target_compile_definitions(${DENG_STATIC_TARGET} 
    PUBLIC DENG_EXPORT_LIBRARY
    PUBLIC DENG_STATIC
)

# Find python library
find_package(Python COMPONENTS Development REQUIRED)

target_include_directories(${DENG_STATIC_TARGET}
    PUBLIC include
    PUBLIC deps
    PUBLIC ${Python_INCLUDE_DIRS}
    PUBLIC deps/${LIBLUA_DEP_DIR}/src
)

target_link_libraries(${DENG_STATIC_TARGET}
    PUBLIC ${LIBNWIN_TARGET}
    PUBLIC ${LIBDAS_TARGET}
    PUBLIC ${IMGUI_TARGET}
    PUBLIC ${LIBLUA_TARGET}
    PUBLIC ${Python_LIBRARIES}
)


# Shared library build configuration
add_library(${DENG_SHARED_TARGET} SHARED
    ${DENG_HEADERS}
    ${DENG_SOURCES}
)

target_compile_definitions(${DENG_SHARED_TARGET} PRIVATE DENG_EXPORT_LIBRARY)
target_include_directories(${DENG_SHARED_TARGET} 
    PUBLIC include
    PUBLIC deps
    PUBLIC ${Python_INCLUDE_DIRS}
    PUBLIC deps/${LIBLUA_DEP_DIR}/src
)

target_link_libraries(${DENG_SHARED_TARGET}
    PUBLIC ${LIBNWIN_TARGET}
    PUBLIC ${LIBDAS_TARGET}
    PUBLIC ${IMGUI_TARGET}
    PUBLIC ${LIBLUA_TARGET}
    PUBLIC ${Python_LIBRARIES}
)

# Link either shaderc_combined if on Linux or shaderc if building for windows platform
if(WIN32)
	target_link_libraries(${DENG_SHARED_TARGET} 
		PUBLIC shaderc)
	target_link_libraries(${DENG_STATIC_TARGET} 
		PUBLIC shaderc)
else()
	target_link_libraries(${DENG_SHARED_TARGET} 
		PUBLIC shaderc_combined)
	target_link_libraries(${DENG_STATIC_TARGET} 
		PUBLIC shaderc_combined)
endif()

# Check if debug mode is used
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${DENG_SHARED_TARGET} PRIVATE _DEBUG)
    target_compile_definitions(${DENG_STATIC_TARGET} PRIVATE _DEBUG)
endif()

# Check if VulkanSDK path was explicitly specified
if(NOT VULKAN_SDK_PATH STREQUAL "")
    message(STATUS "Using Vulkan SDK from path ${VULKAN_SDK_PATH}")
    if(UNIX AND NOT APPLE)
        target_include_directories(${DENG_SHARED_TARGET} PUBLIC ${VULKAN_SDK_PATH}/x86_64/include)
        target_include_directories(${DENG_STATIC_TARGET} PUBLIC ${VULKAN_SDK_PATH}/x86_64/include)

        target_link_directories(${DENG_SHARED_TARGET} PUBLIC ${VULKAN_SDK_PATH}/x86_64/lib)
        target_link_directories(${DENG_STATIC_TARGET} PUBLIC ${VULKAN_SDK_PATH}/x86_64/lib)
    elseif(WIN32)
		target_include_directories(${DENG_SHARED_TARGET} PUBLIC ${VULKAN_SDK_PATH}/Include)
		target_include_directories(${DENG_STATIC_TARGET} PUBLIC ${VULKAN_SDK_PATH}/Include)
		
		target_link_directories(${DENG_SHARED_TARGET} PUBLIC ${VULKAN_SDK_PATH}/Lib)
		target_link_directories(${DENG_STATIC_TARGET} PUBLIC ${VULKAN_SDK_PATH}/Lib)
	endif()
endif()


if(BUILD_DEPS)
    add_dependencies(${DENG_STATIC_TARGET}
        ${LIBDAS_TARGET}
        ${IMGUI_TARGET}
        ${LIBNWIN_TARGET}
    )
    
    add_dependencies(${DENG_SHARED_TARGET}
        ${LIBDAS_TARGET}
        ${IMGUI_TARGET}
        ${LIBNWIN_TARGET}
    )
    
    if(WIN32)
        add_dependencies(${DENG_STATIC_TARGET} shaderc)
        add_dependencies(${DENG_SHARED_TARGET} shaderc)
    endif()
endif()


add_custom_command(TARGET ${DENG_SHARED_TARGET}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/ModelShaders
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_directory ${CMAKE_SOURCE_DIR}/shaders/ModelShaders ${CMAKE_CURRENT_BINARY_DIR}/ModelShaders
)
