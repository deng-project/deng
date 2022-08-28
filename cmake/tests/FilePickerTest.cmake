# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: FilePickerTest.cmake - ImGui custom file picker dialog test application
# author: Karl-Mihkel Ott

set(FILE_PICKER_TARGET FilePickerTest)
set(FILE_PICKER_SOURCES tests/FilePickerTest.cpp)

add_executable(${FILE_PICKER_TARGET} ${FILE_PICKER_SOURCES})
add_dependencies(${FILE_PICKER_TARGET} ${DENG_COMPLETE_TARGET})
target_link_libraries(${FILE_PICKER_TARGET} PRIVATE ${DENG_COMPLETE_TARGET})
