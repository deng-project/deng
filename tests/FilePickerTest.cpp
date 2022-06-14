// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: FilePickerTest.cpp - Testing application that display's custom imgui file picker dialog
// author: Karl-Mihkel Ott

#include <string>
#include <vector>
#include <cmath>
#include <cstring>
#include <chrono>
#ifdef _DEBUG
    #include <iostream>
#endif

#include <libdas/include/Points.h>
#include <libdas/include/Vector.h>
#include <libdas/include/Matrix.h>

#include <Api.h>
#include <Window.h>
#include <ErrorDefinitions.h>
#include <ShaderDefinitions.h>
#include <RenderState.h>
#include <Renderer.h>
#include <ImGuiLayer.h>
#include <FilePicker.h>

int main() {
    DENG::RenderState *rs = DENG::RenderState::GetInstance();
    rs->SetPrimary(DENG::RENDERER_TYPE_VULKAN);

    DENG::FilePicker fp;
    fp.SelectFile("*", 720, 480, "File picker test");
    std::cout << fp.GetPickedFile() << std::endl;

    DENG::RenderState::DeleteInstance();
    return 0;
}
