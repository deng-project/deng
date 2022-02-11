# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: deng.cmake - deng CMake configuration file
# author: Karl-Mihkel Ott

set(DENG_SHARED_TARGET deng-shared)
set(DENG_STATIC_TARGET deng-static)
set(DENG_HEADERS
    include/Api.h
    include/BaseTypes.h
    include/BufferAlignment.h
    include/Camera3DControl.h
    include/Camera3DEventBase.h
    include/Camera3D.h
    include/CameraMatrix.h
    include/EditorCamera.h
    include/ErrorDefinitions.h
    include/FirstPersonCamera.h
    include/HidEventManager.h
    include/ImGuiEntity.h
    include/LightManager.h
    include/LightSources.h
    include/OpenGLBufferLoader.h
    include/OpenGLRenderer.h
    include/OpenGLShaderLoader.h
    include/Renderer.h
    include/ShaderDefinitions.h
    include/UniformData.h
    include/VertexAttributes.h
    include/VulkanAssetCopy.h
    include/VulkanAssets.h
    include/VulkanBufferManager.h
    include/VulkanDescriptorPoolCreator.h
    include/VulkanDescriptorSetLayoutCreator.h
    include/VulkanDescriptorSetsCreator.h
    include/VulkanDrawCaller.h
    include/VulkanHelpers.h
    include/VulkanInstanceCreator.h
    include/VulkanPipelineCreationHelper.h
    include/VulkanPipelineCreator.h
    include/VulkanPipelineData.h
    include/VulkanQueueManager.h
    include/VulkanRenderer.h
    include/VulkanRendererInitialiser.h
    include/VulkanResourceManager.h
    include/VulkanResources.h
    include/VulkanRuntimeUpdater.h
    include/VulkanSwapchainCreator.h
    include/VulkanTextureManager.h
    include/VulkanUniformBufferAllocator.h
    include/Window.h
)

set(DENG_SOURCES
    src/OpenGLBufferLoader.cpp
    src/OpenGLShaderLoader.cpp
    src/OpenGLRenderer.cpp
    src/VulkanDescriptorPoolCreator.cpp
    src/VulkanDescriptorSetLayoutCreator.cpp
    src/VulkanDescriptorSetsCreator.cpp
    src/VulkanHelpers.cpp
    src/VulkanInstanceCreator.cpp
    src/VulkanPipelineCreator.cpp
    src/VulkanRenderer.cpp
    src/VulkanSwapchainCreator.cpp
    src/VulkanUniformBufferAllocator.cpp
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

target_include_directories(${DENG_STATIC_TARGET}
    PRIVATE include
    PRIVATE deps
    PRIVATE deps/nekowin/include/third_party
    PRIVATE deps/shaderc/libshaderc/include
)

target_link_libraries(${DENG_STATIC_TARGET}
    PUBLIC shaderc
    PUBLIC nwin
    PUBLIC das
)


# Shared library build configuration
add_library(${DENG_SHARED_TARGET} SHARED
    ${DENG_HEADERS}
    ${DENG_SOURCES}
)

target_compile_definitions(${DENG_SHARED_TARGET} PRIVATE DENG_EXPORT_LIBRARY)
target_include_directories(${DENG_SHARED_TARGET} 
    PRIVATE include
    PRIVATE deps
    PRIVATE deps/nekowin/include/third_party
    PRIVATE deps/shaderc/libshaderc/include
)

target_link_libraries(${DENG_SHARED_TARGET}
    PRIVATE shaderc
    PRIVATE nwin
    PRIVATE das
)

# Check if debug mode is used
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${DENG_SHARED_TARGET} PRIVATE _DEBUG)
    target_compile_definitions(${DENG_STATIC_TARGET} PRIVATE _DEBUG)
endif()


# Check if build dependencies should be built
if(BUILD_DEPS)
    target_link_directories(${DENG_SHARED_TARGET}
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/deps/shaderc/libshaderc
    )
    
    target_link_directories(${DENG_STATIC_TARGET}
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/deps/shaderc/libshaderc
    )
else()
    target_link_directories(${DENG_SHARED_TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    target_link_directories(${DENG_STATIC_TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endif()
