// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanModelLoader.cpp - Vulkan model loader application implementation
// author: Karl-Mihkel Ott

#include <Executables/ViewModelsApp.h>

namespace Executable {
    
    ModelLoaderApp::ModelLoaderApp(const std::string &_file_name, DENG::Window &_win, DENG::Renderer &_rend) : 
        m_window(_win), 
        m_renderer(_rend),
        m_editor_camera(m_renderer, m_window, m_editor_camera_conf, "Editor camera", 0),
        m_loader(_file_name, _rend, 0, m_renderer.AlignUniformBufferOffset(sizeof(DENG::ModelCameraUbo)), 0), 
        m_imgui_user_data(m_use_camera)
    {
        m_imgui_user_data.model_loaders.push_back(&m_loader);
        m_imgui_user_data.p_camera = &m_editor_camera;
        m_imgui.Attach(m_window, m_renderer, DENG::MeshLoader::GetUboOffset(), ModelLoaderApp::_ImGuiCallback, &m_imgui_user_data);
        m_renderer.LoadShaders();
    }


    void ModelLoaderApp::_ImGuiRecursiveNodeIteration(const DENG::NodeLoader &_node) {
        if(ImGui::CollapsingHeader(_node.GetName().c_str())) {
            // check if mesh is present
            if(_node.GetMeshLoader()) {
                const DENG::MeshLoader *mp_mesh = _node.GetMeshLoader();
                std::string title = "Mesh: " + mp_mesh->GetName();
                ImGui::Text(title.c_str());
            }

            for(auto sub_it = _node.GetChildNodes().begin(); sub_it != _node.GetChildNodes().end(); sub_it++)
                _ImGuiRecursiveNodeIteration(*sub_it);
        }
    }


    void ModelLoaderApp::_ImGuiCallback(void *_data) {
        ImGuiData *p_data = reinterpret_cast<ImGuiData*>(_data);

        //ImGui::ShowDemoWindow();
        ImGui::Begin("Object manager", &p_data->is_object_manager);
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "List of all objects");
            ImGui::Separator();
            for(auto model_it = p_data->model_loaders.begin(); model_it != p_data->model_loaders.end(); model_it++) {
                ImGui::Text("Scene hierarchy");
                std::string model_name = (*model_it)->GetName();

                if(ImGui::CollapsingHeader(model_name.c_str())) {
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Scenes contained within this model");
                    for(auto sc_it = (*model_it)->GetScenes().begin(); sc_it != (*model_it)->GetScenes().end(); sc_it++) {
                        if(ImGui::CollapsingHeader(sc_it->GetName().c_str())) {
                            // scenes can contain nodes
                            for(auto node_it = sc_it->GetNodes().begin(); node_it != sc_it->GetNodes().end(); node_it++)
                                _ImGuiRecursiveNodeIteration(*node_it);
                        }
                    }
                }

                ImGui::Separator();

                ImGui::Text("Animations");

                for(auto ani_it = (*model_it)->GetAnimations().begin(); ani_it != (*model_it)->GetAnimations().end(); ani_it++) {
                    const std::string &animation_name = ani_it->first;
                    if(ani_it->second.front().GetAnimationStatus()) {
                        if(ImGui::Button("Stop animation")) {
                            for(auto smp_it = ani_it->second.begin(); smp_it != ani_it->second.end(); smp_it++)
                                smp_it->Stop();
                        }
                    } else {
                        if(ImGui::Button("Animate")) {
                            for(auto smp_it = ani_it->second.begin(); smp_it != ani_it->second.end(); smp_it++)
                                smp_it->Animate(true);
                        }
                    }

                    if(ImGui::CollapsingHeader(animation_name.c_str())) {
                        for(auto smp_it = ani_it->second.begin(); smp_it != ani_it->second.end(); smp_it++) {
                            const size_t index = smp_it - ani_it->second.begin();
                            const std::string smp_title = "Sampler" + std::to_string(index);
                            ImGui::Text(smp_title.c_str());
                        }
                    }
                }

                ImGui::Separator();

#ifdef _DEBUG
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Camera projection matrix: ");
                    DENG::ModelCameraUbo &ubo = p_data->p_camera->GetCameraUbo();
                    //DENG::ModelCameraUbo &ubo = p_data->p_camera->GetCameraUbo();
                    Libdas::Matrix4<float> &prj = ubo.projection_matrix;
                    Libdas::Matrix4<float> &view = ubo.view_matrix;
                    std::string ms;
                    std::stringstream strm;
                    strm << prj.row1.first << " " << prj.row1.second << " " << prj.row1.third << " " << prj.row1.fourth << "\n" <<
                            prj.row2.first << " " << prj.row2.second << " " << prj.row2.third << " " << prj.row2.fourth << "\n" <<
                            prj.row3.first << " " << prj.row3.second << " " << prj.row3.third << " " << prj.row3.fourth << "\n" <<
                            prj.row4.first << " " << prj.row4.second << " " << prj.row4.third << " " << prj.row4.fourth;

                    ms = strm.str();
                    ImGui::Text(ms.c_str());
                    ImGui::Separator();
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Camera view matrix: ");
                    
                    strm.str("");
                    strm.clear();
                    strm << view.row1.first << " " << view.row1.second << " " << view.row1.third << " " << view.row1.fourth << "\n" <<
                            view.row2.first << " " << view.row2.second << " " << view.row2.third << " " << view.row2.fourth << "\n" <<
                            view.row3.first << " " << view.row3.second << " " << view.row3.third << " " << view.row3.fourth << "\n" <<
                            view.row4.first << " " << view.row4.second << " " << view.row4.third << " " << view.row4.fourth;
                    ms = strm.str();
                    ImGui::Text(ms.c_str());
#endif
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
            m_editor_camera.Update();
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
    DENG::RendererConfig conf = { false, { 0.0f, 0.0f, 0.0f, 0.0f } };
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
