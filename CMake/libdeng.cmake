# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: libdeng.cmake - libdeng CMake configuration file
# author: Karl-Mihkel Ott
	
## Minimal debug build with only Vulkan renderer
set(DENG_MINIMAL_TARGET deng-minimal)

# ImGui sources
set(DENG_IMGUI_HEADERS 
	ThirdParty/imgui/imgui.h
	ThirdParty/imgui/imconfig.h
	ThirdParty/imgui/imgui_internal.h
	ThirdParty/imgui/imstb_rectpack.h
	ThirdParty/imgui/imstb_textedit.h
	ThirdParty/imgui/imstb_truetype.h
	Include/deng/ImGuiLayer.h
	Include/deng/ImGuiResourceBuilders.h
)
	
set(DENG_IMGUI_SOURCES
	ThirdParty/imgui/imgui.cpp
    ThirdParty/imgui/imgui_demo.cpp
    ThirdParty/imgui/imgui_draw.cpp
    ThirdParty/imgui/imgui_tables.cpp
    ThirdParty/imgui/imgui_widgets.cpp
	Sources/ImGuiLayer.cpp
	Sources/ImGuiResourceBuilders.cpp
)

# Graphics backend sources
set(DENG_RENDERER_BACKEND_HEADERS 
	Include/deng/GPUMemoryAllocator.h
	Include/deng/IFramebuffer.h
	Include/deng/IGraphicsInstantiator.h
	Include/deng/IGraphicsMemory.h
	Include/deng/IGraphicsPipeline.h
	Include/deng/IRenderer.h
)

set(DENG_RENDERER_BACKEND_SOURCES
	Sources/GPUMemoryAllocator.cpp
	Sources/IGraphicsMemory.cpp
	Sources/IGraphicsPipeline.cpp)
	
set(DENG_RENDERER_BACKEND_VULKAN_HEADERS
	Include/deng/VulkanFramebuffer.h
	Include/deng/VulkanGraphicsInstantiator.h
	Include/deng/VulkanGraphicsPipeline.h
	Include/deng/VulkanHelpers.h
	Include/deng/VulkanImage.h
	Include/deng/VulkanInstanceCreator.h
	Include/deng/VulkanManagedBuffer.h
	Include/deng/VulkanPipelineCreator.h
	Include/deng/VulkanRenderer.h
	Include/deng/VulkanSwapchainCreator.h)

set(DENG_RENDERER_BACKEND_VULKAN_SOURCES
	Sources/VulkanFramebuffer.cpp
	Sources/VulkanGraphicsInstantiator.cpp
	Sources/VulkanGraphicsPipeline.cpp
	Sources/VulkanHelpers.cpp
	Sources/VulkanImage.cpp
	Sources/VulkanInstanceCreator.cpp
	Sources/VulkanManagedBuffer.cpp
	Sources/VulkanPipelineCreator.cpp
	Sources/VulkanRenderer.cpp
	Sources/VulkanSwapchainCreator.cpp)

set(DENG_SHADER_HEADERS)

set(DENG_SHADER_SOURCES)

set(DENG_SHADER_COMPILER_HEADERS
	Include/deng/IGraphicsShaderCompiler.h)

set(DENG_SHADER_COMPILER_HLSL_HEADERS
	Include/deng/HLSLGraphicsShaderToSpirvCompiler.h)

set(DENG_SHADER_COMPILER_HLSL_SOURCES
	Sources/HLSLGraphicsShaderToSpirvCompiler.cpp)

set(DENG_SHADER_TOOLING_HEADERS
	Include/deng/IByteCodeToolset.h
	Include/deng/SpirvByteCodeToolset.h)

set(DENG_SHADER_TOOLING_SOURCES
	Sources/IByteCodeToolset.cpp
	Sources/SpirvByteCodeToolset.cpp)

set(DENG_UTIL_HEADERS
	Include/deng/spirv_reflect.h
	Include/deng/CameraTransformer.h
	Include/deng/ErrorDefinitions.h
	Include/deng/Exceptions.h
	Include/deng/FastTrigo.h
	Include/deng/FileTextureBuilder.h
	Include/deng/Handle.h
	Include/deng/InitializerListBuilder.h
	Include/deng/MathConstants.h
	Include/deng/Missing.h
	Include/deng/MissingTextureBuilder.h
	Include/deng/ProgramFilesManager.h
	Include/deng/SkyboxBuilders.h
	Include/deng/Strings.h
	Include/deng/xxhash64.h)

set(DENG_UTIL_SOURCES
	Sources/spirv_reflect.c
	Sources/CameraTransformer.cpp
	Sources/ErrorDefinitions.cpp
	Sources/FastTrigo.cpp
	Sources/FileTextureBuilder.cpp
	Sources/Missing.cpp
	Sources/MissingTextureBuilder.cpp
	Sources/ProgramFilesManager.cpp
	Sources/Singletons.cpp
	Sources/SkyboxBuilders.cpp)

set(DENG_CORE_HEADERS
	Include/deng/Api.h
	Include/deng/AssetManager.h
	Include/deng/App.h
	Include/deng/ILayer.h
	Include/deng/Mesh.h
	Include/deng/ResourceBuilders.h)

set(DENG_CORE_SOURCES
	Sources/App.cpp)

set(DENG_SCENE_HEADERS
	Include/deng/Components.h
	Include/deng/ResourceIdTable.h
	Include/deng/Scene.h
	Include/deng/SceneRenderer.h)

set(DENG_SCENE_SOURCES
	Sources/Scene.cpp
	Sources/SceneRenderer.cpp)

set(DENG_WINDOW_HEADERS
	Include/deng/IWindowContext.h)

set(DENG_WINDOW_SOURCES)

set(DENG_WINDOW_SDL_HEADERS
	Include/deng/SDLWindowContext.h)

set(DENG_WINDOW_SDL_SOURCES
	Sources/SDLWindowContext.cpp)

set(DENG_EVENT_HEADERS
	Include/deng/Event.h
	Include/deng/InputEvents.h
	Include/deng/ResourceEvents.h
	Include/deng/SceneEvents.h
	Include/deng/WindowEvents.h)
	
set(DENG_MINIMAL_SOURCES
	# ImGui
	${DENG_IMGUI_HEADERS}
	${DENG_IMGUI_SOURCES}
	# RendererBackend
	${DENG_RENDERER_BACKEND_HEADERS}
	${DENG_RENDERER_BACKEND_SOURCES}
	# RendererBackend/Vulkan
	${DENG_RENDERER_BACKEND_VULKAN_HEADERS}
	${DENG_RENDERER_BACKEND_VULKAN_SOURCES}
	# Shader
	${DENG_SHADER_HEADERS}
	${DENG_SHADER_SOURCES}
	# Shader/Tooling
	${DENG_SHADER_TOOLING_HEADERS}
	${DENG_SHADER_TOOLING_SOURCES}
	# Shader/Compiler
	${DENG_SHADER_COMPILER_HEADERS}
	# Shader/Compiler/HLSL
	${DENG_SHADER_COMPILER_HLSL_HEADERS}
	${DENG_SHADER_COMPILER_HLSL_SOURCES}
	# Util
	${DENG_UTIL_HEADERS}
	${DENG_UTIL_SOURCES}
	# Core
	${DENG_CORE_HEADERS}
	${DENG_CORE_SOURCES}
	# Scene
	${DENG_SCENE_HEADERS}
	${DENG_SCENE_SOURCES}
	# Window
	${DENG_WINDOW_HEADERS}
	${DENG_WINDOW_SOURCES}
	# Window/SDL
	${DENG_WINDOW_SDL_HEADERS}
	${DENG_WINDOW_SDL_SOURCES}
	# Event
	${DENG_EVENT_HEADERS})

if (NOT DENG_STATIC)
	add_library(${DENG_MINIMAL_TARGET} SHARED
				${DENG_MINIMAL_SOURCES})
else()
	add_library(${DENG_MINIMAL_TARGET} STATIC
				${DENG_MINIMAL_SOURCES})
endif()
			
if(WIN32 AND NOT DENG_STATIC)
    target_compile_definitions(${DENG_MINIMAL_TARGET}
		PRIVATE IMGUI_API=_declspec\(dllexport\)
        INTERFACE IMGUI_API=_declspec\(dllimport\)
	)
else()
	target_compile_definitions(${DENG_MINIMAL_TARGET}
		PUBLIC DENG_STATIC)
endif()
			
# Compile definitions
target_compile_definitions(${DENG_MINIMAL_TARGET}
	PRIVATE DENG_COMPLETE_EXPORT_LIBRARY
	PUBLIC ImDrawIdx=unsigned\ int
	PUBLIC ImTextureID=size_t
	PUBLIC NOMINMAX
	PUBLIC SDL_MAIN_HANDLED)
	
# Find package
find_package(Bullet CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(directx-dxc CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# SPIRV-Tools dependency hell
find_package(SPIRV-Tools CONFIG REQUIRED)
find_package(SPIRV-Tools-link CONFIG REQUIRED)
find_package(SPIRV-Tools-lint CONFIG REQUIRED)
find_package(SPIRV-Tools-opt CONFIG REQUIRED)

# Include directories
target_include_directories(${DENG_MINIMAL_TARGET}
	PUBLIC ${VULKAN_SDK_PATH}/Include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/stb
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/libdas/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/mar/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/trs/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vcpkg/installed/x64-windows/include)

target_link_directories(${DENG_MINIMAL_TARGET}
	PUBLIC ${VULKAN_SDK_PATH}/Lib)

add_dependencies(${DENG_MINIMAL_TARGET}
    cvar
	das2)
	
# Linking
target_link_libraries(${DENG_MINIMAL_TARGET}
	PUBLIC cvar
	PRIVATE das2
	PRIVATE Microsoft::DirectXShaderCompiler
	PUBLIC ${BULLET_LIBRARIES}
	PRIVATE
	$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
	$<IF:$<TARGET_EXISTS:SDL2::SDL2-static>,SDL2::SDL2-static,SDL2::SDL2>
	PRIVATE vulkan-1
	PRIVATE SPIRV-Tools-static
	PRIVATE SPIRV-Tools-link
	PRIVATE SPIRV-Tools-lint
	PRIVATE SPIRV-Tools-opt
	PRIVATE glm::glm)
			

source_group(ImGui FILES 
	${DENG_IMGUI_HEADERS}
	${DENG_IMGUI_SOURCES})

source_group(RendererBackend FILES
	${DENG_RENDERER_BACKEND_HEADERS}
	${DENG_RENDERER_BACKEND_SOURCES})

source_group(RendererBackend/Vulkan FILES
	${DENG_RENDERER_BACKEND_VULKAN_HEADERS}
	${DENG_RENDERER_BACKEND_VULKAN_SOURCES})

source_group(Shader FILES
	${DENG_SHADER_HEADERS}
	${DENG_SHADER_SOURCES})

source_group(Shader/Tooling FILES
	${DENG_SHADER_TOOLING_HEADERS}
	${DENG_SHADER_TOOLING_SOURCES})

source_group(Shader/Compiler FILES
	${DENG_SHADER_COMPILER_HEADERS})

source_group(Shader/Compiler/HLSL FILES
	${DENG_SHADER_COMPILER_HLSL_HEADERS}
	${DENG_SHADER_COMPILER_HLSL_SOURCES})

source_group(Util FILES
	${DENG_UTIL_HEADERS}
	${DENG_UTIL_SOURCES})

source_group(Core FILES
	${DENG_CORE_HEADERS}
	${DENG_CORE_SOURCES})

source_group(Scene FILES
	${DENG_SCENE_HEADERS}
	${DENG_SCENE_SOURCES})

source_group(Window FILES
	${DENG_WINDOW_HEADERS}
	${DENG_WINDOW_SOURCES})

source_group(Window/SDL FILES
	${DENG_WINDOW_SDL_HEADERS}
	${DENG_WINDOW_SDL_SOURCES})

source_group(Event FILES
	${DENG_EVENT_HEADERS})
