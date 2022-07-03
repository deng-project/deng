// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ViewModelsApp.cpp - View models application implementation
// author: Karl-Mihkel Ott

#include <Executables/GameEditor.h>

namespace Executable {

    /*************************/
    /***** ImGuiCallback *****/
    /*************************/

    void ImGuiCallback::_OpenFile(EditorGuiData *_data) {
        _data->fp.SelectFile(".das", FILEPICKER_WIDTH, FILEPICKER_HEIGHT, "DENG: Open a 3D model");
        const std::string file = _data->fp.GetPickedFile();
        _data->imgui->SetContext(_data->imgui->GetContext());

        if(file != "") {
            _data->model_loaders.emplace_back(file, *_data->rend, _data->camera_offset, VIEWPORT_NAME);
            _data->rend->LoadShaders();
        }

        DENG::RenderState *rs = DENG::RenderState::GetInstance();
        if(rs->GetPrimary() == DENG::RENDERER_TYPE_OPENGL)
            _data->win->glMakeCurrent();
    }


    void ImGuiCallback::_CreateInspector(EditorGuiData *_data) {
        switch(_data->inspector.entity_type) {
            case EditorInspectorData::ENTITY_TYPE_ANIMATION: 
            {
                DENG::Animation *ani = reinterpret_cast<DENG::Animation*>(_data->inspector.entity);
                ImGui::Text(ani->inspector_title.c_str());

                ImGui::Checkbox(ani->unbind_checkbox_id.c_str(), &ani->is_unbound);
                if(ani->is_unbound) ImGui::BeginDisabled();
                ImGui::Checkbox(ani->repeat_checkbox_id.c_str(), &ani->is_repeated);

                if(!ani->is_animated && ImGui::Button(ani->animate_button_id.c_str())) {
                    ani->is_animated = !ani->is_animated;
                    // animate
                    for(auto it = ani->samplers.begin(); it != ani->samplers.end(); it++)
                        it->Animate(ani->is_repeated);
                } else if(ani->is_animated && ImGui::Button(ani->stop_animation_button_id.c_str())) {
                    ani->is_animated = !ani->is_animated;
                    // stop animation
                    for(auto it = ani->samplers.begin(); it != ani->samplers.end(); it++)
                        it->Stop();
                }
                if(ani->is_unbound) ImGui::EndDisabled();

                break;
            }

            case EditorInspectorData::ENTITY_TYPE_SCENE:
            {
                DENG::SceneLoader *scene = reinterpret_cast<DENG::SceneLoader*>(_data->inspector.entity);
                const std::string txt = "Scene: " + scene->GetName();
                ImGui::Text(txt.c_str());
                break;
            }

            case EditorInspectorData::ENTITY_TYPE_NODE:
            {
                DENG::NodeLoader *node = reinterpret_cast<DENG::NodeLoader*>(_data->inspector.entity);
                const std::string txt = "Node: " + node->GetName();
                ImGui::Text(txt.c_str());

                // custom transformations
                ImGui::Text("Translation: ");
                Libdas::Vector3<float> &t = node->GetCustomTranslation();
                ImGui::DragFloat("X##translation", &t.first, 0.2f);
                ImGui::DragFloat("Y##translation", &t.second, 0.2f);
                ImGui::DragFloat("Z##translation", &t.third, 0.2f);
                node->SetCustomTranslation(t);

                ImGui::Text("Rotation (quaternion): ");
                Libdas::Quaternion &rot = node->GetCustomRotation();
                const float sc45 = 0.707106f;
                ImGui::SliderFloat("X##rot", &rot.x, -sc45, sc45, "%.002f", ImGuiSliderFlags_Logarithmic);
                ImGui::SliderFloat("Y##rot", &rot.y, -sc45, sc45, "%.002f", ImGuiSliderFlags_Logarithmic);
                ImGui::SliderFloat("Z##rot", &rot.z, -sc45, sc45, "%.002f", ImGuiSliderFlags_Logarithmic);
                ImGui::SliderFloat("W##rot", &rot.w, -1, 1, "%.002f", ImGuiSliderFlags_Logarithmic);
                node->SetCustomRotation(rot);

                float &s = node->GetCustomScale();
                ImGui::Text("Scale: ");
                ImGui::DragFloat("##scale", &s, 0.2f);
                node->SetCustomScale(s);
                break;
            }

            case EditorInspectorData::ENTITY_TYPE_MESH:
            {
                DENG::MeshLoader *mesh = reinterpret_cast<DENG::MeshLoader*>(_data->inspector.entity);
                ImGui::Text(mesh->GetInspectorName().c_str());

                // color picker
                ImGui::Checkbox(mesh->GetColorCheckboxId().c_str(), &mesh->GetUseColor());
                if(mesh->GetUseColor()) {
                    Libdas::Vector4<float> color = mesh->GetColor();
                    ImGui::ColorEdit4(mesh->GetColorPickerId().c_str(), &color.first);
                    mesh->SetColor(color);
                } else if(ImGui::Button(mesh->GetTextureButtonId().c_str())) {
                    ImGui::OpenPopup(mesh->GetTexturePickerId().c_str());
                }

                if(ImGui::BeginPopup(mesh->GetTexturePickerId().c_str())) {
                    ImGui::Text("Pick textures");
                    uint32_t &used = mesh->GetUsedTextureCount();
                    const std::string info = "This mesh can use " + std::to_string(mesh->GetSupportedTextureCount()) + " texture; " + std::to_string(mesh->GetSupportedTextureCount() - used) + " are available";
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), info.c_str());

                    std::vector<bool> &tbl = mesh->GetTextureTable();
                    tbl.resize(_data->model_loaders[_data->inspector.model_id].GetAttachedTextures().size());
                    for(auto it = _data->model_loaders[_data->inspector.model_id].GetAttachedTextures().begin(); it != _data->model_loaders[_data->inspector.model_id].GetAttachedTextures().end(); it++) {
                        const size_t index = it - _data->model_loaders[_data->inspector.model_id].GetAttachedTextures().begin();
                        const std::string title = "Use texture no. " + std::to_string(index);
                        bool prev = tbl[index];
                        bool box = tbl[index];
                        if(mesh->GetSupportedTextureCount() - used || box) {
                            ImGui::Checkbox(title.c_str(), &box);
                            tbl[index] = box;
                        } else {
                            ImGui::BeginDisabled(true);
                            ImGui::Checkbox(title.c_str(), &box);
                            ImGui::EndDisabled();
                        }

                        if(prev != box && box) used++;
                        else if(prev != box && !box) used--;
                        ImGui::Image(const_cast<char*>(it->c_str()), ImVec2(64, 64));
                    }

                    if(ImGui::Button(mesh->GetTextureSaveId().c_str())) {
                        std::vector<std::string> names;
                        names.reserve(_data->model_loaders[_data->inspector.model_id].GetAttachedTextures().size());
                        for(auto it = _data->model_loaders[_data->inspector.model_id].GetAttachedTextures().begin(); it != _data->model_loaders[_data->inspector.model_id].GetAttachedTextures().end(); it++) {
                            const size_t index = it - _data->model_loaders[_data->inspector.model_id].GetAttachedTextures().begin();
                            if(tbl[index]) {
                                names.push_back(*it);
                            }
                        }

                        mesh->UseTextures(names);
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                break;
            }

            default:
                break;
        }
    }


    void ImGuiCallback::_CreateRecursiveNodeTree(uint32_t _model_id, DENG::NodeLoader &_node, EditorGuiData *_data) {
        if(ImGui::TreeNode(_node.GetName().c_str())) {
            if(ImGui::IsItemToggledOpen()) {
                _data->inspector.entity_type = EditorInspectorData::ENTITY_TYPE_NODE;
                _data->inspector.entity = &_node;
                _data->inspector.model_id = _model_id;
            }

            DENG::MeshLoader *mesh = _node.GetMeshLoader();

            // check if mesh is present
            if(mesh && ImGui::Selectable(mesh->GetName().c_str())) {
                _data->inspector.entity_type = EditorInspectorData::ENTITY_TYPE_MESH;
                _data->inspector.entity = mesh;
                _data->inspector.model_id = _model_id;
            }

            // for each child node
            for(auto child_it = _node.GetChildNodes().begin(); child_it != _node.GetChildNodes().end(); child_it++) {
                _CreateRecursiveNodeTree(_model_id, *child_it, _data);
            }

            ImGui::TreePop();
        }
    }


    void ImGuiCallback::_CreateHierarchy(EditorGuiData *_data) {
        // for each model loader show hierarchy of components
        for(auto model_it = _data->model_loaders.begin(); model_it != _data->model_loaders.end(); model_it++) {
            const uint32_t index = static_cast<uint32_t>(model_it - _data->model_loaders.begin());
            if(ImGui::TreeNode(model_it->GetName().c_str())) {

                // animations tree
                if(model_it->GetAnimations().size() && ImGui::TreeNode("Animations")) {
                    for(auto ani_it = model_it->GetAnimations().begin(); ani_it != model_it->GetAnimations().end(); ani_it++) {
                        if(ImGui::Selectable(ani_it->name.c_str())) {
                            _data->inspector.entity_type = EditorInspectorData::ENTITY_TYPE_ANIMATION;
                            _data->inspector.entity = &(*ani_it);
                            _data->inspector.model_id = index;
                        }
                    }

                    ImGui::TreePop();
                }

                // scene tree
                // NOTE: size check is not necessary since scenes must be present in every das model
                if(ImGui::TreeNode("Scenes")) {
                    for(auto sc_it = model_it->GetScenes().begin(); sc_it != model_it->GetScenes().end(); sc_it++) {
                        if(ImGui::TreeNode(sc_it->GetName().c_str())) {
                            if(ImGui::IsItemToggledOpen()) {
                                _data->inspector.entity_type = EditorInspectorData::ENTITY_TYPE_SCENE;
                                _data->inspector.entity = &(*sc_it);
                                _data->inspector.model_id = index;
                            }

                            for(auto root_it = sc_it->GetRootNodes().begin(); root_it != sc_it->GetRootNodes().end(); root_it++)
                                _CreateRecursiveNodeTree(index, *root_it, _data);
                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }
        }
    }


    void ImGuiCallback::_ShowCoreUI(void *_gui) {
        EditorGuiData *gui_data = reinterpret_cast<EditorGuiData*>(_gui);

        // docking configuration
        {
            ImVec2 size = { static_cast<float>(gui_data->win->GetSize().x), static_cast<float>(gui_data->win->GetSize().y) };
            ImGui::SetNextWindowSize(size, ImGuiCond_Always);
            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
            ImGui::Begin("Editor context", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar);
                if(ImGui::BeginMenuBar()) {
                    if(ImGui::BeginMenu("File")) {
                        ImGui::MenuItem("New");         // TASK: create new project
                        ImGui::MenuItem("Open");        // TASK: open existing project
                        if(ImGui::MenuItem("Import"))   // TASK: import resources (das, gltf, obj, stl)
                            _OpenFile(gui_data);
                        ImGui::MenuItem("Export");      // TASK: export resources (das)
                        ImGui::MenuItem("Save");        // TASK: save project
                        ImGui::MenuItem("Save as");     // TASK: save project as
                        if(ImGui::MenuItem("Quit"))     // TASK: quit editor
                            gui_data->quit = true;
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenuBar();
                }

                ImGuiID id = ImGui::GetID("EditorDockspace");
                ImGui::DockSpace(id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
                if(gui_data->once) {
                    ImGui::DockBuilderRemoveNode(id);
                    ImGui::DockBuilderAddNode(id, ImGuiDockNodeFlags_None);

                    ImGuiID inspector = ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.25f, nullptr, &id);
                    ImGuiID viewport = ImGui::DockBuilderSplitNode(id, ImGuiDir_Left, 0.75f, nullptr, &id);
                    ImGuiID hierarchy = ImGui::DockBuilderSplitNode(viewport, ImGuiDir_Left, 0.15f, nullptr, &viewport);
                    ImGuiID assets = ImGui::DockBuilderSplitNode(viewport, ImGuiDir_Down, 0.25f, nullptr, &viewport);

                    ImGui::DockBuilderDockWindow("inspector", inspector);
                    ImGui::DockBuilderDockWindow("viewport", viewport);
                    ImGui::DockBuilderDockWindow("hierarchy", hierarchy);
                    ImGui::DockBuilderDockWindow("assets", assets);

                    gui_data->once = false;
                }
            ImGui::End();
        }

        ImGui::Begin("inspector");
            _CreateInspector(gui_data);
        ImGui::End();

        // viewport window
        ImGui::Begin("viewport");
            ImVec2 size = ImGui::GetContentRegionAvail();
            ImGui::Image(gui_data->viewport, size);
        ImGui::End();

        ImGui::Begin("hierarchy");
            _CreateHierarchy(gui_data);
        ImGui::End();

        ImGui::Begin("assets");
            ImGui::Text("Assets");
        ImGui::End();
    }


    ModelLoaderApp::ModelLoaderApp(DENG::Window &_win, DENG::Renderer &_rend) : 
        m_window(_win), 
        m_renderer(_rend),
        m_editor_camera(m_renderer, m_window, m_editor_camera_conf, "Editor camera"),
        m_grid(50.0f, 50.0f, 0.5f, 0.5f, m_editor_camera.GetUboOffset(), VIEWPORT_NAME)
    {
        m_editor_camera.DisableCamera();
        m_imgui_data.viewport = (void*) reinterpret_cast<size_t>(m_viewport_framebuffer_name.c_str());
        m_imgui_data.rend = &m_renderer;
        m_imgui_data.win = &m_window;
        m_imgui_data.imgui = &m_imgui;
        m_imgui_data.camera_offset = m_editor_camera.GetUboOffset();

        // add #VIEWPORT_NAME framebuffer to renderer
        DENG::FramebufferDrawData fb;
        fb.image_name = VIEWPORT_NAME; 
        fb.extent = { WIDTH, HEIGHT };

        m_renderer.PushFramebuffer(fb);
        m_imgui.Attach(m_window, m_renderer, ImGuiCallback::_ShowCoreUI, &m_imgui_data);
        m_grid.Attach(m_renderer);
        m_renderer.LoadShaders();
    }


    void ModelLoaderApp::Run() {
        while(!m_imgui_data.quit && m_window.IsRunning()) {
            m_renderer.ClearFrame();

            m_cur_time = std::chrono::system_clock::now();
            std::chrono::duration<float, std::milli> delta_time = m_cur_time - m_beg_time;

            // check for any input events
            if(delta_time.count() >= m_key_interval) {
                if(m_window.IsKeyPressed(NEKO_KEY_Q))
                    break;

                if(m_window.IsKeyPressed(NEKO_KEY_F)) {
                    if(m_window.GetHints() & NEKO_HINT_FULL_SCREEN)
                        m_window.ChangeSizeHints(NEKO_HINT_RESIZEABLE);
                    else m_window.ChangeSizeHints(NEKO_HINT_FULL_SCREEN);
                    m_beg_time = std::chrono::system_clock::now();
                }

                if(m_window.IsKeyPressed(NEKO_KEY_ESCAPE)) {
                    m_use_camera = !m_use_camera;
                    if(!m_use_camera) {
                        m_editor_camera.DisableCamera();
                        LOG("Camera is now disabled");
                    } else {
                        m_editor_camera.EnableCamera();
                        LOG("Camera is now enabled");
                    }
                    m_beg_time = std::chrono::system_clock::now();
                }
            }

            for(auto it = m_imgui_data.model_loaders.begin(); it != m_imgui_data.model_loaders.end(); it++) {
                it->Update();
            }

            m_editor_camera.Update();
            m_grid.Update(m_renderer, { 0.0f, 0.5f, 0.0f, 1.0f });
            m_imgui.Update();
            m_renderer.RenderFrame();
            m_window.Update();
        }
    }
}

int main(int argv, char *argc[]) {
    // lol
    if(argv < 1)
        std::exit(1);

    DENG::Window::Initialise();

    // Prompt the user, what backend to use
    DENG::PythonScriptExecutor py(argc[0]);
    int32_t backend = py.ExecuteFunction<int32_t>("BackendChooser", "Prompt");

    DENG::RendererConfig conf = { false, { 0.0f, 0.0f, 0.0f, 0.0f } };
    if(backend == USE_VULKAN) {
        DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_VULKAN, "DENG: Untitled project (Vulkan)");
        DENG::Vulkan::Initialise();
        DENG::VulkanRenderer rend(win, conf);
        Executable::ModelLoaderApp app(win, rend);
        app.Run();
    } else if(backend == USE_OPENGL) {
        DENG::Window win(WIDTH, HEIGHT, NEKO_HINT_RESIZEABLE | NEKO_HINT_API_OPENGL, "DENG: Untitled project (OpenGL)");
        DENG::OpenGL::Initialise(win);
        DENG::OpenGLRenderer rend(win, conf);
        Executable::ModelLoaderApp app(win, rend);
        app.Run();
    }
    
    DENG::Window::Deinitialise();
    return EXIT_SUCCESS;
}
