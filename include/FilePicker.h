// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: FilePicker.h - File picker gui class header
// author: Karl-Mihkel Ott

#ifndef FILE_PICKER_H
#define FILE_PICKER_H

#ifdef FILE_PICKER_CPP
    #include <cstring>
    #include <string>
    #include <vector>
    #include <array>
    #include <mutex>
    #include <variant>
    #include <set>
    #include <unordered_map>
    #include <chrono>
    #include <algorithm>
    #include <filesystem>
    #include <atomic>
#if defined(__linux__)
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
#elif defined(_WIN32)
    // windows includes
#endif
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
    #include <Renderer.h>
    #include <VulkanRenderer.h>
    #include <OpenGLRenderer.h>
    #include <ImGuiLayer.h>
    #include <RenderState.h>
#endif

namespace DENG {

#ifdef FILE_PICKER_CPP
    struct _FilePickerGuiData {
        bool once = true;
        bool select = false;
        bool is_open = true;
        bool saved = false;
        bool show_hidden = false;
        Window *win = nullptr;
        std::string active_path = "";
        std::string title = "";
        std::string selected_file = "";
        std::string selected_dir = "";
        std::vector<std::string> types;
        std::vector<bool> selected_file_table;
        std::vector<bool> selected_lib_table;
        std::set<std::string> contents;
    };
#endif

    class DENG_API FilePicker {
        private:
            std::string m_picked_file;
            std::string m_saved_file;

        private:
            // imgui methods
            static bool _FilterDirContent(const std::string &_pth, const std::vector<std::string> &_types, bool _allow_hidden); 
            static void _ShowImGuiAddressBar(void *_gui);
            static void _ShowImGuiFiles(void *_gui);
            static void _ShowImGuiSidebar(void *_gui);
            static std::string _GetHomeDirectory();
            static void _FilePickerCoreUI(void *_gui);

            // general methods
            std::vector<std::string> _ParseFileTypeList(const std::string &_type_list);
            void _Run(Window &_win, Renderer &_rend, ImGuiLayer &_imgui, void *_gui);

        public:
            FilePicker() = default;
            void SelectFile(const std::string &_type_list, int32_t _width, int32_t _height, const std::string &_title, std::atomic<bool> *p_finished = nullptr);
            void SaveFile(const std::string &_type_list, const std::string &_title);

            inline const std::string &GetPickedFile() {
                return m_picked_file;
            }

            inline const std::string &GetSavedFile() {
                return m_saved_file;
            }
    };
}

#endif
