// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanModelLoader.cpp - Vulkan model loader application implementation
// author: Karl-Mihkel Ott

#include "imgui.h"
#include <Executables/ViewModelsApp.h>

namespace Executable {
    
    ModelLoaderApp::ModelLoaderApp(const std::string &_file_name, DENG::Window &_win, DENG::Renderer &_rend) : 
        m_loader(_file_name, _rend, 0, 0), 
        m_window(_win), 
        m_renderer(_rend),
        m_editor_camera(m_window, m_editor_camera_conf, "Editor camera", 0),
        m_imgui_user_data(m_use_camera)
    {
        m_loader.Attach();
        m_imgui_user_data.model_loaders.push_back(&m_loader);
        m_imgui.Attach(m_window, m_renderer, m_loader.GetUsedUniformBufferMemory(), ModelLoaderApp::_ImGuiCallback, &m_imgui_user_data);
        m_renderer.LoadShaders();
    }


    void ModelLoaderApp::_ImGuiCallback(void *_data) {
        ImGuiData *p_data = reinterpret_cast<ImGuiData*>(_data);

        //ImGui::ShowDemoWindow();
        ImGui::Begin("Object manager", &p_data->is_object_manager);
            for(auto model_it = p_data->model_loaders.begin(); model_it != p_data->model_loaders.end(); model_it++) {
                std::string model_name = (*model_it)->GetName();

                ImGui::Text("List of all loaded models");
                ImGui::Separator();

                if(ImGui::CollapsingHeader(model_name.c_str())) {
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Meshes contained within this model");

                    // for each mesh contained within model
                    std::vector<DENG::MeshLoader> &mesh_loaders = (*model_it)->GetMeshes();
                    for(auto mesh_it = mesh_loaders.begin(); mesh_it != mesh_loaders.end(); mesh_it++) {
                        std::string mesh_name = mesh_it->GetName();
                        if(ImGui::CollapsingHeader(mesh_name.c_str())) {
                            Libdas::Vector4<float> color = mesh_it->GetColor();
                            float color_arr[4] = { color.first, color.second, color.third, color.fourth };
                            ImGui::ColorEdit4("Color", color_arr);

                            color = { color_arr[0], color_arr[1], color_arr[2], color_arr[3] };
                            mesh_it->SetColor(color);
                        }
                    }
                    ImGui::Separator();

                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Animations contained within this model");

                    // for each animation contained show drop down menu
                    std::vector<DENG::ModelAnimation> &animations = (*model_it)->GetAnimations();
                    for(auto ani_it = animations.begin(); ani_it != animations.end(); ani_it++) {
                        std::string ani_name = ani_it->name;
                        if(ImGui::CollapsingHeader(ani_name.c_str())) {
                            if(ani_it->is_animated) {
                                if(ImGui::Button("Stop animation")) {
                                    for(auto smp_it = ani_it->samplers.begin(); smp_it != ani_it->samplers.end(); smp_it++)
                                        smp_it->Stop();
                                    ani_it->is_animated = false;
                                }
                            }
                            else {
                                if(ImGui::Button("Animate")) {
                                    for(auto smp_it = ani_it->samplers.begin(); smp_it != ani_it->samplers.end(); smp_it++)
                                        smp_it->Animate(true);
                                    ani_it->is_animated = true;
                                }
                            }
                        }
                    }
                    ImGui::Separator();
                }
            }
        ImGui::End();
    }


    void ModelLoaderApp::Run() {
        while(m_window.IsRunning()) {
            m_renderer.ClearFrame();

            m_cur_time = std::chrono::system_clock::now();
            std::chrono::duration<float, std::milli> delta_time = m_cur_time - m_beg_time;

            if(delta_time.count() >= m_key_interval) {
                if(m_window.IsKeyPressed(NEKO_KEY_Q))
                    break;

                if(m_window.IsKeyPressed(NEKO_KEY_F)) {
                    m_window.ChangeSizeHints(NEKO_HINT_FULL_SCREEN);
                    m_beg_time = std::chrono::system_clock::now();
                }


                // toggle camera mode
                if(m_window.IsKeyPressed(NEKO_KEY_ESCAPE)) {
                    m_use_camera = !m_use_camera;

                    if(!m_use_camera) {
                        m_editor_camera.DisableCamera();
                        std::cout << "Camera is now disabled" << std::endl;
                    }
                    else {
                        m_editor_camera.EnableCamera();
                        std::cout << "Camera is now enabled" << std::endl;
                    }

                    m_beg_time = std::chrono::system_clock::now();
                }
            }


            DENG::ModelCameraUbo ubo;
            m_editor_camera.Update(m_renderer);
            m_loader.Update();
            m_imgui.Update(m_loader.GetUsedMainBufferMemory());
            m_renderer.RenderFrame();
            m_window.Update();
        }
    }

}


int main() {
    // loader.conf file parsing
    std::ifstream file("loader.conf");
    std::string file_name;
    file.seekg(0, std::ios_base::end);
    const size_t fsize = file.tellg();
    file_name.resize(fsize);
    file.seekg(0, std::ios_base::beg);
    file.read(file_name.data(), fsize);
    file.close();

    if(file_name.back() == '\n')
        file_name = file_name.substr(0, file_name.size() - 1);

    // Prompt the user, what backend to use
    DENG::PythonScriptExecutor py;
    int32_t backend = py.ExecuteFunction<int32_t>("BackendChooser", "Prompt");

    // python layer to prompt api selection dialog
    DENG::RendererConfig conf = { false, true };
    if(backend == USE_VULKAN) {
        DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_VULKAN, "VulkanModelLoader");
        DENG::VulkanRenderer rend(win, conf);
        Executable::ModelLoaderApp app(file_name, win, rend);
        app.Run();
    } else if(backend == USE_OPENGL) {
        DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_OPENGL, "OpenGLModelLoader");
        DENG::OpenGLRenderer rend(win, conf);
        Executable::ModelLoaderApp app(file_name, win, rend);
        app.Run();
    }
    
    return EXIT_SUCCESS;
}
