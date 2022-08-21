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
    include/deng/AnimationSampler.h
    include/deng/Api.h
    include/deng/BaseTypes.h
    include/deng/BufferAlignment.h
    include/deng/Camera3D.h
    include/deng/EditorCamera.h
    include/deng/ErrorDefinitions.h
    include/deng/FilePicker.h
    include/deng/FirstPersonCamera.h
    include/deng/GPUMemoryManager.h
    include/deng/GridGenerator.h
    include/deng/HardwareInfo.h
    include/deng/ImGuiLayer.h
    include/deng/MeshLoader.h
    include/deng/Missing.h
    include/deng/ModelLoader.h
    include/deng/ModelShaderManager.h
    include/deng/ModelShaderGenerator.h
    include/deng/ModelUniforms.h
    include/deng/NodeLoader.h
    include/deng/OpenGLBufferLoader.h
    include/deng/OpenGLFramebuffer.h
    include/deng/OpenGLRenderer.h
    include/deng/OpenGLShaderLoader.h
    include/deng/PythonScriptExecutor.h
    include/deng/Renderer.h
    include/deng/RenderState.h
    include/deng/SceneLoader.h
    include/deng/ShaderDefinitions.h
    include/deng/SkeletonLoader.h
    include/deng/ThirdPersonCamera.h
    include/deng/Version.h
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
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/nekowin/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/libdas/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/mar/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/trs/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/dxml/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui
)

# Vulkan sdk includes
if(WIN32)
	target_include_directories(${DENG_COMPLETE_TARGET}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/VulkanSDK-${VULKAN_SDK_VERSION}/Include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/shaderc-${SHADERC_VERSION}/include)
		
	if(CMAKE_BUILD_TYPE MATCHES Debug)
		add_custom_command(TARGET ${DENG_COMPLETE_TARGET} POST_BUILD
			COMMAND ${CMAKE_COMMAND} 
			ARGS -E remove_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/python3.10
			COMMAND ${CMAKE_COMMAND} 
			ARGS -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/python3.10
			COMMAND ${CMAKE_COMMAND} 
			ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/Lib ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/python3.10
			COMMAND ${CMAKE_COMMAND} 
			ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/python310_d.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
			COMMAND ${CMAKE_COMMAND}
			ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/libcrypto-1_1.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
			COMMAND ${CMAKE_COMMAND}
			ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/libssl-1_1.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
			COMMAND ${CMAKE_COMMAND}
			ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/libffi-7.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
		)
	elseif(CMAKE_BUILD_TYPE MATCHES Release)
		add_custom_target(TARGET ${DENG_COMPLETE_TARGET} POST_BUILD
			COMMAND ${CMAKE_COMMAND} 
			ARGS-E remove_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/python3.10
			COMMAND ${CMAKE_COMMAND} 
			ARGS -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/python3.10
			COMMAND ${CMAKE_COMMAND} 
			ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/Lib ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/python3.10
			COMMAND ${CMAKE_COMMAND} 
			ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/python310.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
			COMMAND ${CMAKE_COMMAND}
			ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/libcrypto-1_1.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
			COMMAND ${CMAKE_COMMAND}
			ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/libssl-1_1.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
			COMMAND ${CMAKE_COMMAND}
			ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/libffi-7.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}	
		)
	endif()
elseif(UNIX)
    target_include_directories(${DENG_COMPLETE_TARGET}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/VulkanSDK-${VULKAN_SDK_VERSION}/include)
endif()

# CPython includes
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_include_directories(${DENG_COMPLETE_TARGET}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/include/python3.10d)
else()
    target_include_directories(${DENG_COMPLETE_TARGET}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/include/python3.10)
endif()

# Linking
target_link_libraries(${DENG_COMPLETE_TARGET}
    PRIVATE mar
    PRIVATE nwin-static
    PRIVATE das-static
	PRIVATE dxml)

# Universal compile definitions for imgui
target_compile_definitions(${DENG_COMPLETE_TARGET}
    PUBLIC ImDrawIdx=unsigned\ int
    PUBLIC DENG_EDITOR)


# Link trunked libraries
if(WIN32)
    target_compile_definitions(${DENG_COMPLETE_TARGET}
        PRIVATE IMGUI_API=_declspec\(dllexport\)
        INTERFACE IMGUI_API=_declspec\(dllimport\)
	)

    target_link_directories(${DENG_COMPLETE_TARGET}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/VulkanSDK-${VULKAN_SDK_VERSION}/Lib
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/shaderc-${SHADERC_VERSION}/lib
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION})

	if(CMAKE_BUILD_TYPE MATCHES Debug)
		target_link_libraries(${DENG_COMPLETE_TARGET} 
			PUBLIC python310_d)
	elseif(CMAKE_BUILD_TYPE MATCHES Release)
		target_link_libraries(${DENG_COMPLETE_TARGET} 
            PUBLIC python310)
	endif()

    target_link_libraries(${DENG_COMPLETE_TARGET}
        PUBLIC shaderc_combined
        PUBLIC vulkan-1)
	
elseif(UNIX AND NOT MACOS)
    target_link_directories(${DENG_COMPLETE_TARGET}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/VulkanSDK-${VULKAN_SDK_VERSION}/lib
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/lib)

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        target_link_libraries(${DENG_COMPLETE_TARGET}
            PUBLIC python3.10d)
    elseif(CMAKE_BUILD_TYPE MATCHES Release)
        target_link_libraries(${DENG_COMPLETE_TARGET}
            PUBLIC python3.10)
    endif()

    target_link_libraries(${DENG_COMPLETE_TARGET}
        PUBLIC util
        PUBLIC shaderc_combined)
endif()

add_dependencies(${DENG_COMPLETE_TARGET}
    das-static
    dastool
    nwin-static
	dxml
)
