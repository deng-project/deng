// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: FilePickerTest.cpp - Testing application that display's custom imgui file picker dialog
// author: Karl-Mihkel Ott

// win32 and release mode is used - make sure that cmd does not spawn
#if defined(_WIN32) && !defined(__DEBUG)
    #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

#include <string>
#include <vector>
#include <cmath>
#include <cstring>
#include <chrono>
#include <unordered_map>
#ifdef __DEBUG
    #include <iostream>
#endif

#include "trs/Points.h"
#include "trs/Vector.h"
#include "trs/Matrix.h"

#include "deng/Api.h"
#include "deng/Window.h"
#include "deng/ErrorDefinitions.h"
#include "deng/ShaderDefinitions.h"
#include "deng/RenderState.h"
#include "deng/Renderer.h"
#include "deng/ImGuiLayer.h"
#include "deng/FilePicker.h"

int main() {
    DENG::Window::Initialise();
    DENG::RenderState *rs = DENG::RenderState::GetInstance();
    rs->SetPrimary(DENG::RENDERER_TYPE_VULKAN);

    DENG::FilePicker fp;
    fp.SelectFile("*", 720, 480, "File picker test");
    std::cout << fp.GetPickedFile() << std::endl;

    DENG::RenderState::DeleteInstance();
    DENG::Window::Deinitialise();
    return 0;
}
