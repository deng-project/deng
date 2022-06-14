# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: deng.cmake - deng CMake configuration file
# author: Karl-Mihkel Ott

set(DENG_SHARED_TARGET deng-shared)
set(DENG_STATIC_TARGET deng-static)
set(DENG_HEADERS
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
    include/RenderState.h
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
    src/RenderState.cpp
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


# Library configurations
add_library(${DENG_STATIC_TARGET} STATIC
    ${DENG_HEADERS}
    ${DENG_SOURCES}
)

add_library(${DENG_SHARED_TARGET} SHARED
    ${DENG_HEADERS}
    ${DENG_SOURCES}
)

# Compile definitions
target_compile_definitions(${DENG_STATIC_TARGET} 
    PUBLIC DENG_EXPORT_LIBRARY
    PUBLIC DENG_STATIC
)
target_compile_definitions(${DENG_SHARED_TARGET} PRIVATE DENG_EXPORT_LIBRARY)


# Include directories
target_include_directories(${DENG_SHARED_TARGET} 
    PUBLIC include
    PUBLIC deps
    PUBLIC deps/nekowin/include/third_party
    PUBLIC deps/libdas/include
    PUBLIC deps/imgui
)

target_include_directories(${DENG_STATIC_TARGET}
    PUBLIC include
    PUBLIC deps
    PUBLIC deps/nekowin/include/third_party
    PUBLIC deps/libdas/include
    PUBLIC deps/imgui
)

# Linking
target_link_libraries(${DENG_STATIC_TARGET}
    PRIVATE nwin-static
    PRIVATE das-static
)

target_link_libraries(${DENG_SHARED_TARGET}
    PRIVATE nwin-static
    PRIVATE das-static
)

# Universal compile definitions for imgui
target_compile_definitions(${DENG_STATIC_TARGET}
    PUBLIC ImDrawIdx=unsigned\ int)

target_compile_definitions(${DENG_SHARED_TARGET}
    PUBLIC ImDrawIdx=unsigned\ int)


# Link trunked libraries
if(WIN32)
    target_compile_definitions(${DENG_SHARED_TARGET}
        PRIVATE IMGUI_API=_declspec\(dllexport\)
        INTERFACE IMGUI_API=_declspec\(dllimport\)
    )

	if(CMAKE_BUILD_TYPE MATCHES Debug)
		target_link_directories(${DENG_STATIC_TARGET} 
            PUBLIC deps/trunk/Lib/Debug
            PUBLIC deps/trunk/Lib/Debug/Python)
		target_link_directories(${DENG_SHARED_TARGET} 
            PUBLIC deps/trunk/Lib/Debug
            PUBLIC deps/trunk/Lib/Debug/Python)
		
		target_link_libraries(${DENG_STATIC_TARGET} 
			PUBLIC python39_d
			PRIVATE shaderc_combined
		)
		
		target_link_libraries(${DENG_SHARED_TARGET} 
			PUBLIC python39_d
			PRIVATE shaderc_combined
		)

	elseif(CMAKE_BUILD_TYPE MATCHES Release)
		target_link_directories(${DENG_STATIC_TARGET} 
            PUBLIC deps/trunk/Lib/Release
            PUBLIC deps/trunk/Lib/Release/Python)
		target_link_directories(${DENG_SHARED_TARGET} 
            PUBLIC deps/trunk/Lib/Release
            PUBLIC deps/trunk/Lib/Release/Python)
		
		target_link_libraries(${DENG_STATIC_TARGET} 
            PRIVATE python39
			PRIVATE shaderc_combined
		)
		
		target_link_libraries(${DENG_SHARED_TARGET} 
            PRIVATE python39
		    PRIVATE shaderc_combined
		)
	endif()
	
	target_link_libraries(${DENG_STATIC_TARGET} 
		PUBLIC vulkan-1)
	
	target_link_libraries(${DENG_SHARED_TARGET} 
		PUBLIC vulkan-1)
elseif(UNIX AND NOT MACOS)
    target_link_directories(${DENG_STATIC_TARGET}
        PUBLIC deps/trunk/Lib
        PUBLIC deps/trunk/Python)

    target_link_directories(${DENG_SHARED_TARGET}
        PUBLIC deps/trunk/Lib
        PUBLIC deps/trunk/Python)

    target_link_libraries(${DENG_STATIC_TARGET}
        PRIVATE python3.9
        PRIVATE util
        PRIVATE shaderc_combined)

    target_link_libraries(${DENG_SHARED_TARGET}
        PRIVATE python3.9
        PRIVATE util
        PRIVATE shaderc_combined)
endif()


# Check if debug mode is used
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${DENG_SHARED_TARGET} PRIVATE _DEBUG)
    target_compile_definitions(${DENG_STATIC_TARGET} PRIVATE _DEBUG)
endif()


add_dependencies(${DENG_STATIC_TARGET}
    COPY_TARGET
    das-static
    dastool
    nwin-static
    ${IMGUI_TARGET}
)

add_dependencies(${DENG_SHARED_TARGET}
    COPY_TARGET
    das-static
    dastool
    nwin-static
    ${IMGUI_TARGET}
)
