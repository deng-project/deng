# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: copy.cmake - CMake copy targets
# author: Karl-Mihkel Ott

# Custom copying target
if(MSVC)
	message(STATUS "Debug output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")
	message(STATUS "Runtime output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}")
	
    if(CMAKE_BUILD_TYPE MATCHES Debug)
		add_custom_command(COPY_TARGET
			${CMAKE_COMMAND} -E remove_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/python3.10
			${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/python3.10
			${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/Lib ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/python3.10
			${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/*.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
		)
	elseif(CMAKE_BUILD_TYPE MATCHES Release)
		add_custom_target(COPY_TARGET
			${CMAKE_COMMAND} -E remove_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/python3.10
			${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/python3.10
			${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/Lib ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/python3.10
			${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/deps/cpython-${PYTHON_VERSION}/*.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
		)
	endif()
endif()
