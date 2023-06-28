# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: libdeng.cmake - libdeng CMake configuration file
# author: Karl-Mihkel Ott
	
## Minimal debug build with only Vulkan renderer
set(DENG_MINIMAL_TARGET deng-minimal)
set(DENG_IMGUI_HEADERS 
	ThirdParty/imgui/imgui.h
	ThirdParty/imgui/imconfig.h
	ThirdParty/imgui/imgui_internal.h
	ThirdParty/imgui/imstb_rectpack.h
	ThirdParty/imgui/imstb_textedit.h
	ThirdParty/imgui/imstb_truetype.h)
	
set(DENG_IMGUI_SOURCES
	ThirdParty/imgui/imgui.cpp
    ThirdParty/imgui/imgui_demo.cpp
    ThirdParty/imgui/imgui_draw.cpp
    ThirdParty/imgui/imgui_tables.cpp
    ThirdParty/imgui/imgui_widgets.cpp)
	
set(DENG_MINIMAL_HEADERS
	Include/deng/Api.h
	Include/deng/App.h
	Include/deng/CameraTransformer.h
	Include/deng/Components.h
	Include/deng/ErrorDefinitions.h
	Include/deng/Event.h
	Include/deng/Exceptions.h
	Include/deng/FileTextureBuilder.h
	Include/deng/FileSystemShader.h
	Include/deng/GPUMemoryAllocator.h
	Include/deng/IFramebuffer.h
	Include/deng/ILayer.h
	Include/deng/ImGuiLayer.h
	Include/deng/ImGuiResourceBuilders.h
	Include/deng/InitializerListBuilder.h
	Include/deng/InputEvents.h
	Include/deng/IRenderer.h
	Include/deng/IShader.h
	Include/deng/IWindowContext.h
	Include/deng/MathConstants.h
	Include/deng/Missing.h
	Include/deng/MissingTextureBuilder.h
	Include/deng/ProgramFilesManager.h
	Include/deng/RenderResources.h
	Include/deng/ResourceEvents.h
	Include/deng/ResourceIdTable.h
	Include/deng/Scene.h
	Include/deng/SceneEvents.h
	Include/deng/SceneRenderer.h
	Include/deng/SDLWindowContext.h
	Include/deng/SID.h
	Include/deng/SkyboxBuilders.h
	Include/deng/UberShader.h
	Include/deng/VulkanFramebuffer.h
	Include/deng/VulkanHelpers.h
	Include/deng/VulkanInstanceCreator.h
	Include/deng/VulkanPipelineCreator.h
	Include/deng/VulkanRenderer.h
	Include/deng/VulkanSwapchainCreator.h
	Include/deng/WindowEvents.h)
	
set(DENG_MINIMAL_SOURCES
	Sources/App.cpp
	Sources/CameraTransformer.cpp
	Sources/ErrorDefinitions.cpp
	Sources/FileTextureBuilder.cpp
	Sources/FileSystemShader.cpp
	Sources/GPUMemoryAllocator.cpp
	Sources/ImGuiLayer.cpp
	Sources/ImGuiResourceBuilders.cpp
	Sources/Missing.cpp
	Sources/MissingTextureBuilder.cpp
	Sources/ProgramFilesManager.cpp
	Sources/Scene.cpp
	Sources/SceneRenderer.cpp
	Sources/SDLWindowContext.cpp
	Sources/Singletons.cpp
	Sources/SkyboxBuilders.cpp
	Sources/UberShader.cpp
	Sources/VulkanFramebuffer.cpp
	Sources/VulkanHelpers.cpp
	Sources/VulkanInstanceCreator.cpp
	Sources/VulkanPipelineCreator.cpp
	Sources/VulkanRenderer.cpp
	Sources/VulkanSwapchainCreator.cpp)
	
if (NOT DENG_STATIC)
	add_library(${DENG_MINIMAL_TARGET} SHARED
				${DENG_MINIMAL_HEADERS}
				${DENG_MINIMAL_SOURCES}
				${DENG_IMGUI_HEADERS}
				${DENG_IMGUI_SOURCES})
else()
	add_library(${DENG_MINIMAL_TARGET} STATIC
				${DENG_MINIMAL_HEADERS}
				${DENG_MINIMAL_SOURCES}
				${DENG_IMGUI_HEADERS}
				${DENG_IMGUI_SOURCES})
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

# shaderc dependency madness
find_package(glslang CONFIG REQUIRED)
find_package(SPIRV-Tools CONFIG REQUIRED)
find_package(SPIRV-Tools-opt CONFIG REQUIRED)
find_package(SPIRV-Tools-link CONFIG REQUIRED)
find_package(SPIRV-Tools-lint CONFIG REQUIRED)
find_package(unofficial-shaderc_util CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)

# Include directories
target_include_directories(${DENG_MINIMAL_TARGET}
	PUBLIC ${VULKAN_SDK_PATH}/Include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/libdas/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/mar/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/trs/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vcpkg/installed/x64-windows/include)

target_link_directories(${DENG_MINIMAL_TARGET}
	PUBLIC ${VULKAN_SDK_PATH}/Lib)

add_dependencies(${DENG_MINIMAL_TARGET}
    das-static
    dastool
	dxml)
	
# Linking
target_link_libraries(${DENG_MINIMAL_TARGET}
	PRIVATE mar
	PRIVATE unofficial::shaderc::shaderc
	PRIVATE ${BULLET_LIBRARIES}
	PRIVATE
	$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
	$<IF:$<TARGET_EXISTS:SDL2::SDL2-static>,SDL2::SDL2-static,SDL2::SDL2>
	PUBLIC das-static
	PUBLIC dxml
	PRIVATE vulkan-1)
			


