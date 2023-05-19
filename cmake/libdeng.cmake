# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: libdeng.cmake - libdeng CMake configuration file
# author: Karl-Mihkel Ott

set(DENG_COMPLETE_TARGET deng-complete)
set(DENG_IMGUI_HEADERS 
	deps/imgui/imgui.h
	deps/imgui/imconfig.h
	deps/imgui/imgui_internal.h
	deps/imgui/imstb_rectpack.h
	deps/imgui/imstb_textedit.h
	deps/imgui/imstb_truetype.h)
	
set(DENG_IMGUI_SRC
	deps/imgui/imgui.cpp
    deps/imgui/imgui_demo.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp)

set(DENG_COMPLETE_HEADERS
	include/deng/AnimationSampler.h
    include/deng/Api.h
    include/deng/BaseTypes.h
    include/deng/BufferAlignment.h
    include/deng/Camera3D.h
    include/deng/Components.h
    include/deng/CubeGenerator.h
    include/deng/deng.h
    include/deng/EditorCamera.h
    include/deng/Entity.h
    include/deng/ErrorDefinitions.h
    include/deng/Exceptions.h
    include/deng/FirstPersonCamera.h
    include/deng/GameObject.h
    include/deng/GPUMemoryManager.h
    include/deng/GridGenerator.h
    include/deng/HardwareInfo.h
	include/deng/IFramebuffer.h
    include/deng/ImGuiLayer.h
    include/deng/InputRegistry.h
    include/deng/IRenderer.h
    include/deng/IWindowContext.h
    include/deng/LevelContext.h
    include/deng/LevelLoader.h
    include/deng/Missing.h
    include/deng/ModelUniforms.h
    include/deng/OpenGLBufferLoader.h
    include/deng/OpenGLFramebuffer.h
    include/deng/OpenGLRenderer.h
    include/deng/OpenGLShaderLoader.h
    include/deng/OpenGLTextureData.h
    include/deng/ProgramFilesManager.h
    include/deng/ProjectDataManager.h
    include/deng/PythonScriptExecutor.h
    include/deng/Registry.h
    include/deng/RenderState.h
    include/deng/Scene.h
    include/deng/ScriptableEntity.h
    include/deng/Shader.h
    include/deng/ShaderDefinitions.h
    include/deng/Skybox.h
    include/deng/SphereGenerator.h
    include/deng/TextureDatabase.h
    include/deng/ThirdPersonCamera.h
    include/deng/Version.h
    include/deng/VertexNormalVisualizer.h
    include/deng/VulkanDescriptorAllocator.h
    include/deng/VulkanFramebuffer.h
    include/deng/VulkanHelpers.h
    include/deng/VulkanInstanceCreator.h
    include/deng/VulkanPipelineCreator.h
    include/deng/VulkanRenderer.h
    include/deng/VulkanSwapchainCreator.h)
    

set(DENG_COMPLETE_SOURCES
	src/AnimationSampler.cpp
	src/Camera3D.cpp
	src/Components.cpp
	src/CubeGenerator.cpp
	src/EditorCamera.cpp
	src/Entity.cpp
	src/ErrorDefinitions.cpp
	src/FirstPersonCamera.cpp
	src/GameObject.cpp
	src/GPUMemoryManager.cpp
	src/GridGenerator.cpp
	src/ImGuiLayer.cpp
	src/LevelContext.cpp
	src/LevelLoader.cpp
	src/Missing.cpp
	src/OpenGLBufferLoader.cpp
	src/OpenGLFramebuffer.cpp
	src/OpenGLRenderer.cpp
	src/OpenGLShaderLoader.cpp
	src/ProgramFilesManager.cpp
	src/ProjectDataManager.cpp
	src/Registry.cpp
	src/RenderState.cpp
	src/Scene.cpp
	src/Shader.cpp
	src/ShaderDefinitions.cpp
	src/Skybox.cpp
	src/SphereGenerator.cpp
	src/TextureDatabase.cpp
	src/ThirdPersonCamera.cpp
	src/VertexNormalVisualizer.cpp
	src/VulkanDescriptorAllocator.cpp
	src/VulkanFramebuffer.cpp
	src/VulkanHelpers.cpp
	src/VulkanInstanceCreator.cpp
	src/VulkanPipelineCreator.cpp
	src/VulkanRenderer.cpp
	src/VulkanSwapchainCreator.cpp
	src/Window.cpp)

# Library configurations
add_library(${DENG_COMPLETE_TARGET} SHARED
    ${DENG_COMPLETE_HEADERS}
	${DENG_IMGUI_HEADERS}
    ${DENG_COMPLETE_SOURCES}
	${DENG_IMGUI_SRC})

# Source groups
source_group("ImGui" FILES ${DENG_IMGUI_HEADERS} ${DENG_IMGUI_SRC})

# Compile definitions
target_compile_definitions(${DENG_COMPLETE_TARGET} PRIVATE DENG_COMPLETE_EXPORT_LIBRARY)

# Include directories
target_include_directories(${DENG_COMPLETE_TARGET} 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/nekowin/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/libdas/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/mar/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/trs/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/dxml/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui
)

target_link_directories(${DENG_COMPLETE_TARGET}
	PUBLIC ${VULKAN_SDK_PATH}/Lib)


# Linking
find_package(Bullet CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
target_link_libraries(${DENG_COMPLETE_TARGET}
    PRIVATE mar
    PRIVATE nwin-static
	PRIVATE Vulkan::Headers
	PRIVATE EnTT:EnTT
    PRIVATE ${BULLET_LIBRARIES}
    PRIVATE unofficial::shaderc::shaderc
	PRIVATE ODE::ODE
	PUBLIC pybind11::lto
	PUBLIC pybind11::embed
	PUBLIC pybind11::module
	PRIVATE cryptopp::cryptopp
	PUBLIC das-static
	PUBLIC dxml
	PRIVATE vulkan-1)

target_compile_definitions(${DENG_COMPLETE_TARGET}
    PUBLIC ImDrawIdx=unsigned\ int
    PUBLIC DENG_EDITOR
)


# Link trunked libraries
if(WIN32)
    target_compile_definitions(${DENG_COMPLETE_TARGET}
        PRIVATE IMGUI_API=_declspec\(dllexport\)
        INTERFACE IMGUI_API=_declspec\(dllimport\)
	)
endif()

add_dependencies(${DENG_COMPLETE_TARGET}
    das-static
    dastool
    nwin-static
	dxml)
	
	
## Minimal debug build with only Vulkan renderer
set(DENG_MINIMAL_TARGET deng-minimal)
set(DENG_MINIMAL_HEADERS
	include/deng/Api.h
	include/deng/BaseTypes.h
	include/deng/Components.h
	include/deng/ErrorDefinitions.h
	include/deng/Exceptions.h
	include/deng/GPUMemoryAllocator.h
	include/deng/IFramebuffer.h
	include/deng/IRenderer.h
	include/deng/IWindowContext.h
	include/deng/Missing.h
	include/deng/ProgramFilesManager.h
	include/deng/ModelUniforms.h
	include/deng/SDLWindowContext.h
	include/deng/Shader.h
	include/deng/ShaderDefinitions.h
	include/deng/VulkanDescriptorAllocator.h
	include/deng/VulkanFramebuffer.h
	include/deng/VulkanHelpers.h
	include/deng/VulkanInstanceCreator.h
	include/deng/VulkanPipelineCreator.h
	include/deng/VulkanRenderer.h
	include/deng/VulkanSwapchainCreator.h)
	
set(DENG_MINIMAL_SOURCES
	src/ErrorDefinitions.cpp
	src/GPUMemoryAllocator.cpp
	src/Missing.cpp
	src/ProgramFilesManager.cpp
	src/SDLWindowContext.cpp
	src/Shader.cpp
	src/ShaderDefinitions.cpp
	src/VulkanDescriptorAllocator.cpp
	src/VulkanFramebuffer.cpp
	src/VulkanHelpers.cpp
	src/VulkanInstanceCreator.cpp
	src/VulkanPipelineCreator.cpp
	src/VulkanRenderer.cpp
	src/VulkanSwapchainCreator.cpp)
	
add_library(${DENG_MINIMAL_TARGET} SHARED
			${DENG_MINIMAL_HEADERS}
			${DENG_MINIMAL_SOURCES})
			
# Compile definitions
target_compile_definitions(${DENG_MINIMAL_TARGET} 
	PRIVATE DENG_COMPLETE_EXPORT_LIBRARY
	PUBLIC SDL_MAIN_HANDLED)

# Include directories
find_package(SDL2 CONFIG REQUIRED)
target_include_directories(${DENG_MINIMAL_TARGET} 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/libdas/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/mar/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/trs/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/vcpkg/installed/x64-windows/include)

target_link_directories(${DENG_MINIMAL_TARGET}
	PUBLIC ${VULKAN_SDK_PATH}/Lib)


# Linking
target_link_libraries(${DENG_MINIMAL_TARGET}
    PRIVATE mar
	PRIVATE Vulkan::Headers
    PRIVATE unofficial::shaderc::shaderc
	PRIVATE 
	$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
	$<IF:$<TARGET_EXISTS:SDL2::SDL2-static>,SDL2::SDL2-static,SDL2::SDL2>
	PUBLIC das-static
	PUBLIC dxml
	PRIVATE vulkan-1)

add_dependencies(${DENG_COMPLETE_TARGET}
    das-static
    dastool
	dxml)
			


