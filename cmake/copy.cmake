# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: copy.cmake - CMake copy targets
# author: Karl-Mihkel Ott

# Custom copying target
if(WIN32 AND MSVC)
    # Set include directories for trunked dependencies
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/trunk/Include)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}")

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_custom_target(COPY_TARGET 
            ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/deps/trunk/Lib/Debug/Python/python39_d.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/scripts/BackendChooser.py ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Python
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Python
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Python/Lib
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Python/DLLs
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/deps/trunk/Lib/Python ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Python/Lib
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/deps/trunk/Lib/Debug/Python ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/Python/DLLs
        )
    elseif(CMAKE_BUILD_TYPE MATCHES Release)
        add_custom_target(COPY_TARGET
            ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/deps/trunk/Lib/Release/Python/python39.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/scripts/BackendChooser.py ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Python
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Python
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Python/Lib
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Python/DLLs
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/deps/trunk/Lib/Python ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Python/Lib
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/deps/trunk/Lib/Release/Python ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/Python/DLLs
        )
    endif()
elseif(UNIX AND NOT APPLE)
    message(FATAL_ERROR "[For developer] Please add GNU/Linux dependencies to trunked dependency list")
endif()