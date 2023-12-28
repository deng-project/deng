# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: VulkanGraphicsInstantiatorTest.cmake - Smoke test for VulkanGraphicsInstantiator class
# author: Karl-Mihkel Ott

set(VK_INSTANTIATOR_TEST_TARGET VulkanGraphicsInstantiatorTest)
set(VK_INSTANTIATOR_TEST_HEADERS
	Include/deng/Api.h
	Include/deng/ErrorDefinitions.h
	Include/deng/Exceptions.h
	Include/deng/Handle.h
	Include/deng/IGraphicsInstantiator.h
	Include/deng/VulkanGraphicsInstantiator.h
	Include/deng/IWindowContext.h
	Include/deng/SDLWindowContext.h)

set(VK_INSTANTIATOR_TEST_SOURCES
	Sources/ErrorDefinitions.cpp
	Sources/SDLWindowContext.cpp
	Sources/Singletons.cpp
	Sources/VulkanGraphicsInstantiator.cpp
	Tests/Sources/VulkanGraphicsInstantiatorTest.cpp)

enable_testing()
find_package(GTest CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

add_executable(${VK_INSTANTIATOR_TEST_TARGET}
	${VK_INSTANTIATOR_TEST_HEADERS}
	${VK_INSTANTIATOR_TEST_SOURCES})

target_link_libraries(${VK_INSTANTIATOR_TEST_TARGET}
	PRIVATE GTest::gtest
	PRIVATE GTest::gmock
	PRIVATE
	$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
	$<IF:$<TARGET_EXISTS:SDL2::SDL2-static>,SDL2::SDL2-static,SDL2::SDL2>
	PRIVATE vulkan-1)

target_compile_definitions(${VK_INSTANTIATOR_TEST_TARGET}
	PRIVATE DENG_STATIC)

target_include_directories(${VK_INSTANTIATOR_TEST_TARGET}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include
	PRIVATE ${VULKAN_SDK_PATH}/Include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/cvar/Include)

target_link_directories(${VK_INSTANTIATOR_TEST_TARGET}
	PRIVATE ${VULKAN_SDK_PATH}/Lib)

add_test(${VK_INSTANTIATOR_TEST_TARGET} ${VK_INSTANTIATOR_TEST_TARGET})
set_target_properties(${VK_INSTANTIATOR_TEST_TARGET} PROPERTIES FOLDER "UnitTests")