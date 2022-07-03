# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: deng.cmake - deng CMake configuration file
# author: Karl-Mihkel Ott

set(DENG_COMPLETE_TARGET deng-complete)
set(DENG_COMPLETE_HEADERS
    deps/imgui/imgui.h
    deps/imgui/imconfig.h
    deps/imgui/imgui_internal.h
    deps/imgui/imstb_rectpack.h
    deps/imgui/imstb_textedit.h
    deps/imgui/imstb_truetype.h
    include/AnimationSampler.h
    include/Api.h
    include/BaseTypes.h
    include/BufferAlignment.h
    include/Camera3D.h
    include/EditorCamera.h
    include/ErrorDefinitions.h
    include/FilePicker.h
    include/FirstPersonCamera.h
    include/GPUMemoryManager.h
    include/GridGenerator.h
    include/ImGuiLayer.h
    include/MeshLoader.h
    include/Missing.h
    include/ModelLoader.h
    include/ModelShaderManager.h
    include/ModelShaderGenerator.h
    include/ModelUniforms.h
    include/NodeLoader.h
    include/OpenGLBufferLoader.h
    include/OpenGLFramebuffer.h
    include/OpenGLRenderer.h
    include/OpenGLShaderLoader.h
    include/PythonScriptExecutor.h
    include/Renderer.h
    include/RenderState.h
    include/SceneLoader.h
    include/ShaderDefinitions.h
    include/SkeletonLoader.h
    include/ThirdPersonCamera.h
    include/VulkanDescriptorAllocator.h
    include/VulkanDescriptorSetLayoutCreator.h
    include/VulkanFramebuffer.h
    include/VulkanHelpers.h
    include/VulkanInstanceCreator.h
    include/VulkanPipelineCreator.h
    include/VulkanRenderer.h
    include/VulkanSwapchainCreator.h
    include/Window.h
)

set(DENG_COMPLETE_SOURCES
    deps/imgui/imgui.cpp
    deps/imgui/imgui_demo.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp
    src/AnimationSampler.cpp
    src/Camera3D.cpp
    src/EditorCamera.cpp
    src/FilePicker.cpp
    src/FirstPersonCamera.cpp
    src/GPUMemoryManager.cpp
    src/GridGenerator.cpp
    src/ImGuiLayer.cpp
    src/MeshLoader.cpp
    src/Missing.cpp
    src/ModelLoader.cpp
    src/ModelShaderManager.cpp
    src/ModelShaderGenerator.cpp
    src/NodeLoader.cpp
    src/OpenGLBufferLoader.cpp
    src/OpenGLFramebuffer.cpp
    src/OpenGLRenderer.cpp
    src/OpenGLShaderLoader.cpp
    src/PythonScriptExecutor.cpp
    src/RenderState.cpp
    src/SceneLoader.cpp
    src/ShaderDefinitions.cpp
    src/SkeletonLoader.cpp
    src/ThirdPersonCamera.cpp
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
    ${DENG_COMPLETE_SOURCES}
)

# Compile definitions
target_compile_definitions(${DENG_COMPLETE_TARGET} PRIVATE DENG_COMPLETE_EXPORT_LIBRARY)


# Include directories
target_include_directories(${DENG_COMPLETE_TARGET} 
    PUBLIC include
    PUBLIC deps
    PUBLIC deps/nekowin/include/third_party
    PUBLIC deps/libdas/include
    PUBLIC deps/imgui
)

# Linking
target_link_libraries(${DENG_COMPLETE_TARGET}
    PRIVATE nwin-static
    PRIVATE das-static
)

# Universal compile definitions for imgui
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

	if(CMAKE_BUILD_TYPE MATCHES Debug)
		target_link_directories(${DENG_COMPLETE_TARGET} 
            PUBLIC deps/trunk/Lib/Debug
            PUBLIC deps/trunk/Lib/Debug/Python)
		
		target_link_libraries(${DENG_COMPLETE_TARGET} 
			PUBLIC python39_d
			PRIVATE shaderc_combined
		)

	elseif(CMAKE_BUILD_TYPE MATCHES Release)
		target_link_directories(${DENG_COMPLETE_TARGET} 
            PUBLIC deps/trunk/Lib/Release
            PUBLIC deps/trunk/Lib/Release/Python)
		
		target_link_libraries(${DENG_COMPLETE_TARGET} 
            PRIVATE python39
		    PRIVATE shaderc_combined
		)
	endif()
	
	target_link_libraries(${DENG_COMPLETE_TARGET} 
		PUBLIC vulkan-1)
elseif(UNIX AND NOT MACOS)
    target_link_directories(${DENG_COMPLETE_TARGET}
        PUBLIC deps/trunk/Lib
        PUBLIC deps/trunk/Python)

    target_link_libraries(${DENG_COMPLETE_TARGET}
        PRIVATE python3.9
        PRIVATE util
        PRIVATE shaderc_combined)
endif()


# Check if debug mode is used
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${DENG_COMPLETE_TARGET} PRIVATE _DEBUG)
endif()


add_dependencies(${DENG_COMPLETE_TARGET}
    COPY_TARGET
    das-static
    dastool
    nwin-static
    ${IMGUI_TARGET}
)
