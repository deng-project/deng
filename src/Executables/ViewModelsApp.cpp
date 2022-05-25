// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ViewModelsApp.cpp - View models application implementation
// author: Karl-Mihkel Ott

#include <Executables/ViewModelsApp.h>

namespace Executable {
    
    ModelLoaderApp::ModelLoaderApp(const std::string &_file_name, DENG::Window &_win, DENG::Renderer &_rend) : 
        m_window(_win), 
        m_renderer(_rend),
        m_editor_camera(m_renderer, m_window, m_editor_camera_conf, "Editor camera"),
        m_loader(_file_name, _rend, m_editor_camera.GetUboOffset()), 
        m_imgui_user_data(m_use_camera)
    {
        m_imgui_user_data.model_loaders.push_back(&m_loader);
        m_imgui_user_data.texture_picker_data.resize(m_loader.GetAttachedTextures().size(), false);
        m_imgui_user_data.enabled_texture_count = 0;
        m_imgui_user_data.p_camera = &m_editor_camera;
        m_imgui.Attach(m_window, m_renderer, ModelLoaderApp::_ImGuiCallback, &m_imgui_user_data);
        m_renderer.LoadShaders();
    }


    void ModelLoaderApp::_ImGuiShowTransformationProperties(DENG::NodeLoader &_node) {
        ImGui::Text("Custom transformations:");
        float scale = _node.GetCustomScale();
        ImGui::DragFloat("Scale", &scale);
        _node.SetCustomScale(scale);

        Libdas::Vector3<float> translation = _node.GetCustomTranslation();
        ImGui::Text("Translation:");
        ImGui::DragFloat("Translation X", &translation.first);
        ImGui::DragFloat("Translation Y", &translation.second);
        ImGui::DragFloat("Translation Z", &translation.third);
        _node.SetCustomTranslation(translation);

        Libdas::Point3D<float> rotation;
        ImGui::Text("Rotation:");
        ImGui::DragFloat("Rotation X", &rotation.x, 1.0f, 0.0f, 360.0f);
        ImGui::DragFloat("Rotation Y", &rotation.y, 1.0f, 0.0f, 360.0f);
        ImGui::DragFloat("Rotation Z", &rotation.z, 1.0f, 0.0f, 360.0f);
    }


    void ModelLoaderApp::_ImGuiShowTexturePicker(ImGuiData *_p_data, DENG::ModelLoader *_p_loader, DENG::MeshLoader &_mesh) {
        if(ImGui::BeginPopup("Texture picker")) {
            ImGui::Text("Pick textures");
            const std::string info_str = "This mesh can use " + std::to_string(_mesh.GetSupportedTextureCount()) + " textures; " + std::to_string(_mesh.GetSupportedTextureCount() - _p_data->enabled_texture_count) + " left";
            ImGui::TextColored(ImVec4(1, 1, 0, 1), info_str.c_str());

            for(auto it = _p_loader->GetAttachedTextures().begin(); it != _p_loader->GetAttachedTextures().end(); it++) {
                const size_t index = it - _p_loader->GetAttachedTextures().begin();
                const std::string title = "Use texture no. " + std::to_string(index);
                bool box = _p_data->texture_picker_data[index];
                const bool prev = _p_data->texture_picker_data[index];
                if(!box && !(_mesh.GetSupportedTextureCount() - _p_data->enabled_texture_count)) {
                    ImGui::BeginDisabled(true);
                    ImGui::Checkbox(title.c_str(), &box);
                    ImGui::EndDisabled();
                } else {
                    ImGui::Checkbox(title.c_str(), &box);
                    _p_data->texture_picker_data[index] = box;
                }

                if(!prev && box) _p_data->enabled_texture_count++;
                else if(prev && !box) _p_data->enabled_texture_count--;
                ImGui::Image(const_cast<char*>(it->c_str()), ImVec2(64, 64));
            }

            if(ImGui::Button("Save")) {
                std::vector<std::string> names;
                names.reserve(_p_loader->GetAttachedTextures().size());
                for(auto it = _p_loader->GetAttachedTextures().begin(); it != _p_loader->GetAttachedTextures().end(); it++) {
                    const size_t index = it - _p_loader->GetAttachedTextures().begin();
                    if(_p_data->texture_picker_data[index])
                        names.push_back(*it);
                }

                if(names.empty())
                    names.push_back(MISSING_TEXTURE_NAME);

                _mesh.UseTextures(names);
            }

            ImGui::EndPopup();
        }
    }


    void ModelLoaderApp::_ImGuiRecursiveNodeIteration(ImGuiData *_p_data, DENG::ModelLoader *_p_loader, DENG::NodeLoader &_node) {
        if(ImGui::TreeNode(_node.GetName().c_str())) {
            _ImGuiShowTransformationProperties(_node);
            // check if mesh is present
            if(_node.GetMeshLoader()) {
                DENG::MeshLoader *mp_mesh = _node.GetMeshLoader();
                std::string str = "Mesh: " + mp_mesh->GetName();
                ImGui::Text(str.c_str());

                if(mp_mesh) {
                    bool v = !mp_mesh->GetUseColor();
                    ImGui::Checkbox("Use textures instead", &v);
                    mp_mesh->SetUseColor(!v);
                    if(ImGui::Button("Select textures")) {
                        _p_data->texture_picker_data.resize(_p_loader->GetAttachedTextures().size());
                        ImGui::OpenPopup("Texture picker");
                    }
                    _ImGuiShowTexturePicker(_p_data, _p_loader, *mp_mesh);

                    v = mp_mesh->GetDisableJointTransforms();
                    ImGui::Checkbox("Disable joint transforms", &v);
                    mp_mesh->SetDisableJointTransforms(v);

                    Libdas::Vector4<float> color = mp_mesh->GetColor();
                    ImGui::ColorEdit4("Mesh color", reinterpret_cast<float*>(&color));
                    mp_mesh->SetColor(color);
                }
            }

            for(auto sub_it = _node.GetChildNodes().begin(); sub_it != _node.GetChildNodes().end(); sub_it++)
                _ImGuiRecursiveNodeIteration(_p_data, _p_loader, *sub_it);

            ImGui::TreePop();
        }
    }


    void ModelLoaderApp::_ImGuiCallback(void *_data) {
        ImGuiData *p_data = reinterpret_cast<ImGuiData*>(_data);
        p_data->max_id = 1;

        //ImGui::ShowDemoWindow();
        ImGui::Begin("Object manager", &p_data->is_object_manager);
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "List of all objects");
            ImGui::Separator();
            for(auto model_it = p_data->model_loaders.begin(); model_it != p_data->model_loaders.end(); model_it++) {
                ImGui::Text("Models: ");
                std::string model_name = (*model_it)->GetName();

                if(ImGui::TreeNode(model_name.c_str())) {

                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Scenes contained within this model");
                    for(auto sc_it = (*model_it)->GetScenes().begin(); sc_it != (*model_it)->GetScenes().end(); sc_it++) {
                        if(ImGui::TreeNode(sc_it->GetName().c_str())) {
                            // scenes can contain nodes
                            _ImGuiRecursiveNodeIteration(p_data, *model_it, sc_it->GetRootNode());
                            ImGui::TreePop();
                        }
                    }

                    ImGui::Separator();

                    ImGui::Text("Animations");
                    for(auto ani_it = (*model_it)->GetAnimations().begin(); ani_it != (*model_it)->GetAnimations().end(); ani_it++) {
                        const std::string &animation_name = ani_it->name;

                        if(ImGui::TreeNode(animation_name.c_str())) {
                            for(auto smp_it = ani_it->samplers.begin(); smp_it != ani_it->samplers.end(); smp_it++) {
                                const size_t index = smp_it - ani_it->samplers.begin();
                                const std::string smp_title = "Sampler" + std::to_string(index);
                                ImGui::Text(smp_title.c_str());
                            }


                            ImGui::Checkbox("Bind with animation", &ani_it->is_bound);
                            if(ani_it->samplers.front().GetAnimationStatus()) {
                                if(ImGui::Button("Stop animation")) {
                                    for(auto smp_it = ani_it->samplers.begin(); smp_it != ani_it->samplers.end(); smp_it++)
                                        smp_it->Stop();
                                }
                            } else {
                                if(ImGui::Button("Animate")) {
                                    for(auto smp_it = ani_it->samplers.begin(); smp_it != ani_it->samplers.end(); smp_it++)
                                        smp_it->Animate(true);
                                }
                            }

                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }

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
            m_imgui.Update();
            m_renderer.RenderFrame();
            m_window.Update();
        }
    }

}


int main() {
    // Prompt the user, what backend to use
    DENG::PythonScriptExecutor py;
    int32_t backend = py.ExecuteFunction<int32_t>("BackendChooser", "Prompt");

    // loader.conf file parsing
    std::ifstream file("loader.conf");
    std::string file_name;
    file.seekg(0, std::ios_base::end);
    const size_t fsize = file.tellg();
    file_name.resize(fsize);
    file.seekg(0, std::ios_base::beg);
    file.read(file_name.data(), fsize);
    file.close();

    if (file_name.back() == '\n')
        file_name = file_name.substr(0, file_name.size() - 1);

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
