# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: deng.cmake - deng CMake configuration file
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
	include/deng/deng.h
    include/deng/AnimationSampler.h
    include/deng/Api.h
    include/deng/BaseTypes.h
    include/deng/BufferAlignment.h
    include/deng/Camera3D.h
	include/deng/CubeGenerator.h
    include/deng/EditorCamera.h
	include/deng/Entity.h
    include/deng/ErrorDefinitions.h
    include/deng/FilePicker.h
    include/deng/FirstPersonCamera.h
    include/deng/GPUMemoryManager.h
    include/deng/GridGenerator.h
    include/deng/HardwareInfo.h
    include/deng/ImGuiLayer.h
	include/deng/InputRegistry.h
    include/deng/MeshLoader.h
    include/deng/Missing.h
    include/deng/ModelLoader.h
    include/deng/ModelShaderGenerator.h
    include/deng/ModelShaderManager.h
    include/deng/ModelUniforms.h
    include/deng/NodeLoader.h
    include/deng/OpenGLBufferLoader.h
    include/deng/OpenGLFramebuffer.h
    include/deng/OpenGLRenderer.h
    include/deng/OpenGLShaderLoader.h
	include/deng/OpenGLTextureData.h
    include/deng/ProjectDataManager.h
    include/deng/PythonScriptExecutor.h
	include/deng/Registry.h
    include/deng/Renderer.h
    include/deng/RenderState.h
    include/deng/SceneLoader.h
	include/deng/ScriptableEntity.h
    include/deng/ShaderDefinitions.h
    include/deng/SkeletonLoader.h
	include/deng/Skybox.h
	include/deng/SphereGenerator.h
    include/deng/ThirdPersonCamera.h
    include/deng/Version.h
	include/deng/VertexNormalVisualizer.h
    include/deng/VulkanDescriptorAllocator.h
    include/deng/VulkanDescriptorSetLayoutCreator.h
    include/deng/VulkanFramebuffer.h
    include/deng/VulkanHelpers.h
    include/deng/VulkanInstanceCreator.h
    include/deng/VulkanPipelineCreator.h
    include/deng/VulkanRenderer.h
    include/deng/VulkanSwapchainCreator.h
    include/deng/Window.h
)

set(DENG_COMPLETE_SOURCES
    src/AnimationSampler.cpp
    src/Camera3D.cpp
	src/CubeGenerator.cpp
    src/EditorCamera.cpp
	src/Entity.cpp
    src/FilePicker.cpp
    src/FirstPersonCamera.cpp
    src/GPUMemoryManager.cpp
    src/GridGenerator.cpp
    src/ImGuiLayer.cpp
    src/MeshLoader.cpp
    src/Missing.cpp
    src/ModelLoader.cpp
    src/ModelShaderGenerator.cpp
    src/ModelShaderManager.cpp
    src/NodeLoader.cpp
    src/OpenGLBufferLoader.cpp
    src/OpenGLFramebuffer.cpp
    src/OpenGLRenderer.cpp
    src/OpenGLShaderLoader.cpp
    src/ProjectDataManager.cpp
	src/Registry.cpp
    src/RenderState.cpp
    src/SceneLoader.cpp
    src/ShaderDefinitions.cpp
    src/SkeletonLoader.cpp
	src/Skybox.cpp
	src/SphereGenerator.cpp
    src/ThirdPersonCamera.cpp
	src/VertexNormalVisualizer.cpp
    src/VulkanDescriptorAllocator.cpp
    src/VulkanDescriptorSetLayoutCreator.cpp
    src/VulkanFramebuffer.cpp
    src/VulkanHelpers.cpp
    src/VulkanInstanceCreator.cpp
    src/VulkanPipelineCreator.cpp
    src/VulkanRenderer.cpp
    src/VulkanSwapchainCreator.cpp
    src/Window.cpp
)

# Library configurations
add_library(${DENG_COMPLETE_TARGET} SHARED
    ${DENG_COMPLETE_HEADERS}
	${DENG_IMGUI_HEADERS}
    ${DENG_COMPLETE_SOURCES}
	${DENG_IMGUI_SRC}
)

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
	PUBLIC C:/VulkanSDK/1.3.236.0/Lib)


# Linking
target_link_libraries(${DENG_COMPLETE_TARGET}
    PRIVATE mar
    PRIVATE nwin-static
	PRIVATE Vulkan::Headers Vulkan::Registry
    PRIVATE unofficial::shaderc::shaderc
	PRIVATE ODE::ODE
	PRIVATE pybind11::lto
	PRIVATE pybind11::embed
	PRIVATE pybind11::module
	PRIVATE cryptopp::cryptopp
	PUBLIC das-static
	PUBLIC dxml
	PRIVATE vulkan-1)

target_compile_definitions(${DENG_COMPLETE_TARGET}
    PUBLIC ImDrawIdx=unsigned\ int
    PUBLIC DENG_EDITOR)


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
	dxml
)
