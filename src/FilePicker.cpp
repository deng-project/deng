// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: FilePicker.cpp - File picker gui class implementation
// author: Karl-Mihkel Ott

#define FILE_PICKER_CPP
#include "deng/FilePicker.h"


namespace DENG {

    /**************************/
    /***** ImGui methods ******/
    /**************************/
    bool FilePicker::_FilterDirContent(const std::string &_pth, const std::vector<std::string> &_types, bool _allow_hidden) {
        // check if the directory content is hidden
        std::filesystem::path fs(_pth);
        if(!_allow_hidden && fs.filename().u8string()[0] == '.')
            return false;

        if(!std::filesystem::is_directory(fs) && _types.size() && _types[0] != "*") {
            // check if file extension is in types
            const std::string ext = fs.extension().u8string();
            auto it = std::find(_types.begin(), _types.end(), ext);
            if(it == _types.end())
                return false;
        }

        return true;
    }


    void FilePicker::_ShowImGuiAddressBar(void *_gui) {
        _FilePickerGuiData *gui_data = reinterpret_cast<_FilePickerGuiData*>(_gui);
        if(ImGui::Button("Go back")) {
            std::filesystem::path fs(gui_data->active_path);
            gui_data->active_path = fs.parent_path().u8string();
        }
        ImGui::SameLine();
        ImGui::Text("%s", gui_data->active_path.c_str());
    }


    void FilePicker::_ShowImGuiFiles(void *_gui) {
        _FilePickerGuiData *gui_data = reinterpret_cast<_FilePickerGuiData*>(_gui);
        std::filesystem::path active{gui_data->active_path};
        const size_t dirkwlen = 7;
        const size_t filekwlen = 8;

        size_t count = 0;
        gui_data->contents.clear();
        for(auto const &it : std::filesystem::directory_iterator(active, std::filesystem::directory_options::skip_permission_denied)) {
            ++count;
            try {
                if (std::filesystem::is_directory(it)) {
                    gui_data->contents.insert("[DIR]  " + it.path().u8string());
                }
                else {
                    gui_data->contents.insert("[FILE]  " + it.path().u8string());
                }
            } catch (std::filesystem::filesystem_error& e) {
                std::cerr << e.what() << std::endl;
            }
        }
        
        gui_data->selected_file_table.resize(count);

        count = 0;
        ImGui::BeginChild("Files child context", ImVec2(0.0f, -50.0f));
            for(auto &it : gui_data->contents) {
                std::string rel_path;
                std::string abs_path;
                if(it.find("[DIR]  ") != std::string::npos) {
                    abs_path = it.substr(dirkwlen);
                    std::filesystem::path pth(abs_path);
                    rel_path = "[DIR]  " + pth.filename().u8string();
                } else {
                    abs_path = it.substr(filekwlen);
                    std::filesystem::path pth(abs_path);
                    rel_path = "[FILE]  " + pth.filename().u8string();
                }

                // check if the directory content passes filtering
                if(_FilterDirContent(abs_path, gui_data->types, gui_data->show_hidden) &&
                   ImGui::Selectable(rel_path.c_str(), gui_data->selected_file_table[count], ImGuiSelectableFlags_AllowDoubleClick)) 
                {
                    std::fill(gui_data->selected_file_table.begin(), gui_data->selected_file_table.end(), false);
                    gui_data->selected_file_table[count] = true;
                    if(std::filesystem::is_directory(abs_path)) {
                        gui_data->selected_dir = abs_path;
                        gui_data->selected_file = "";
                    } else {
                        gui_data->selected_file = abs_path;
                        gui_data->selected_dir = "";
                    }

                    if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        if(std::filesystem::is_directory(abs_path)) {
                            gui_data->active_path = gui_data->selected_dir;
                        } else {
                            gui_data->saved = true;
                            break;
                        }
                    }
                }

                count++;
            }
        ImGui::EndChild();

        // iterate the root file system
        if(ImGui::Button("Open")) {
            if(gui_data->selected_file != "")
                gui_data->saved = true;
            else if(gui_data->selected_dir != "") 
                gui_data->active_path = gui_data->selected_dir;
        }
    }


    void FilePicker::_ShowImGuiSidebar(void *_gui) {
        _FilePickerGuiData *gui_data = reinterpret_cast<_FilePickerGuiData*>(_gui);

        ImGui::Text("Libraries:");
        ImGui::Separator();
        // check if the home path contains libraries such as Documents, Pictures, Videos, Downloads, Music, Desktop
        std::array<std::string, 6> libs = {
            "Documents", "Pictures", "Videos",
            "Downloads", "Music", "Desktop"
        };

        std::vector<std::string> available_libs;
        available_libs.reserve(6);

        std::string home = _GetHomeDirectory();

        // check which libraries are present
        for(auto &it : libs) {
            if(std::filesystem::exists(home + "/" + it)) {
                available_libs.push_back(home + "/" + it);
            }
        }

        gui_data->selected_lib_table.resize(available_libs.size());
        for(auto it = available_libs.begin(); it != available_libs.end(); it++) {
            std::filesystem::path pth(*it);
            const size_t index = it - available_libs.begin();
            if(ImGui::Selectable(pth.filename().u8string().c_str(), gui_data->selected_lib_table[index])) {
                std::fill(gui_data->selected_lib_table.begin(), gui_data->selected_lib_table.end(), false);
                gui_data->active_path = *it;
            }
        }
    }


    void FilePicker::_FilePickerCoreUI(void *_gui) {
        _FilePickerGuiData *gui_data = reinterpret_cast<_FilePickerGuiData*>(_gui);

        ImVec2 size = { static_cast<float>(gui_data->win->GetSize().x), static_cast<float>(gui_data->win->GetSize().y) };
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::Begin("FilePicker context", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
            // create docking space
            ImGuiID id = ImGui::GetID("FilePickerDockspace");
            ImGui::DockSpace(id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoTabBar);
            if(gui_data->once) {
                ImGui::DockBuilderRemoveNode(id);
                ImGui::DockBuilderAddNode(id, ImGuiDockNodeFlags_None);
                ImGuiID files = ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.7f, nullptr, &id);
                ImGuiID addressbar = ImGui::DockBuilderSplitNode(files, ImGuiDir_Up, 0.1f, nullptr, &files);
                ImGuiID sidebar = ImGui::DockBuilderSplitNode(id, ImGuiDir_Left, 0.3f, nullptr, &id);

                ImGui::DockBuilderDockWindow("files", files);
                ImGui::DockBuilderDockWindow("sidebar", sidebar);
                ImGui::DockBuilderDockWindow("addressbar", addressbar);
                ImGui::DockBuilderFinish(id);
                gui_data->once = false;
            }

        ImGui::End();

        ImGui::Begin("files", NULL);
            _ShowImGuiFiles(_gui);
        ImGui::End();
        ImGui::Begin("addressbar", NULL);
            _ShowImGuiAddressBar(_gui);
        ImGui::End();
        ImGui::Begin("sidebar", NULL);
            _ShowImGuiSidebar(_gui);
        ImGui::End();
    }


    /******************************/
    /***** FilePicker methods *****/
    /******************************/
    std::vector<std::string> FilePicker::_ParseFileTypeList(const std::string &_type_list) {
        // seperation symbol is '|' eg. ".ext|.ext1"
        std::vector<std::string> exts;
        const char sep = '|';
        std::size_t prev_pos = 0;
        std::size_t pos = 0;
        while((pos = _type_list.find(sep, prev_pos)) != std::string::npos) {
            exts.push_back(_type_list.substr(prev_pos, pos));
            prev_pos = pos + 1;
        }

        // check if string has content and extract it
        if(prev_pos < _type_list.size() - 1)
            exts.push_back(_type_list.substr(prev_pos));

        return exts;
    }


    std::string FilePicker::_GetHomeDirectory() {
#if defined(__linux__)
        // for multithread safety use getpwuid_r
        const char *homedir = NULL;
        if((homedir = getenv("HOME")) == NULL) {
            homedir = getpwuid(getuid())->pw_dir;
        }

        return std::string(homedir);
#elif defined(_WIN32)
        const char *homedir = getenv("USERPROFILE");
        return std::string(homedir);
#endif
    }


    void FilePicker::_Run(Window &_win, Renderer &_rend, ImGuiLayer &_imgui, void *_gui) {
        _FilePickerGuiData *gui_data = reinterpret_cast<_FilePickerGuiData*>(_gui);
        while (_win.IsRunning()) {
            _win.Update();
            gui_data->conf.canvas_size = {
                static_cast<uint32_t>(_win.GetSize().x),
                static_cast<uint32_t>(_win.GetSize().y)
            };

            _imgui.Update();
            _rend.RenderFrame();

            if(gui_data->saved)
                break;
        }
    }


    void FilePicker::SelectFile(const std::string &_type_list, int32_t _width, int32_t _height, const std::string &_title) {
        _FilePickerGuiData gui_data;
        gui_data.types = _ParseFileTypeList(_type_list);
        gui_data.active_path = _GetHomeDirectory();
        gui_data.select = true;
        gui_data.title = _title;

        // check the primary renderer api
        RenderState *rs = RenderState::GetInstance();
        switch(rs->GetPrimary()) {
            case RENDERER_TYPE_OPENGL:
                {
                    Window win(_width, _height, NEKO_HINT_FIXED_SIZE, _title.c_str());
                    gui_data.win = &win;

                    gui_data.conf.canvas_size = { 
                        static_cast<uint32_t>(_width), 
                        static_cast<uint32_t>(_height) 
                    };

                    gui_data.conf.title = win.GetTitle();

                    OpenGL::Initialise(win);
                    OpenGLRenderer rend(gui_data.conf);
                    ImGuiLayer imgui;
                    imgui.Attach(win, rend, FilePicker::_FilePickerCoreUI, reinterpret_cast<void*>(&gui_data));
                    rend.LoadShaders();
                    _Run(win, rend, imgui, reinterpret_cast<void*>(&gui_data));
                }
                break;

            case RENDERER_TYPE_VULKAN:
                {
                    Window win(_width, _height, NEKO_HINT_FIXED_SIZE, _title.c_str());
                    Vulkan::Initialise();
                    gui_data.win = &win;
                    gui_data.conf.canvas_size = { 
                        static_cast<uint32_t>(_width),
                        static_cast<uint32_t>(_height)
                    };

                    gui_data.conf.title = win.GetTitle();
#ifdef _WIN32
                    gui_data.conf.win32_hwnd = win.GetWin32HWND();
                    gui_data.conf.win32_instance = win.GetWin32Instance();
#else
                    gui_data.conf.xlib_win = win.GetXlibWindow();
                    gui_data.conf.xlib_dpy = win.GetXlibDisplay();
#endif

                    VulkanRenderer rend(gui_data.conf);
                    ImGuiLayer imgui;
                    imgui.Attach(win, rend, FilePicker::_FilePickerCoreUI, reinterpret_cast<void*>(&gui_data));
                    rend.LoadShaders();
                    _Run(win, rend, imgui, reinterpret_cast<void*>(&gui_data));
                }
                break;

            default:
                DENG_ASSERT(false);
                break;
        }

        m_picked_file = gui_data.selected_file;
    }
}
