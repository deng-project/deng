# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: GPUMemoryAllocator.cmake - GPUMemoryAllocator class test configuration
# author: Karl-Mihkel Ott

set(GPU_MEMORY_ALLOCATOR_TARGET GPUMemoryAllocatorTest)
set(GPU_MEMORY_ALLOCATOR_HEADERS
	include/deng/GPUMemoryAllocator.h)
set(GPU_MEMORY_ALLOCATOR_SRC
	src/GPUMemoryAllocator.cpp
	tests/GPUMemoryAllocatorTest.cpp)
	
add_executable(${GPU_MEMORY_ALLOCATOR_TARGET}
	${GPU_MEMORY_ALLOCATOR_HEADERS}
	${GPU_MEMORY_ALLOCATOR_SRC})
	
target_include_directories(${GPU_MEMORY_ALLOCATOR_TARGET}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
	

set_target_properties(${GPU_MEMORY_ALLOCATOR_TARGET} PROPERTIES FOLDER ${MANUAL_TEST_DIR})
