#ifndef WINDOWS_H
#define WINDOWS_H

namespace deng {   

    class Window
    {
    private:
        GLFWwindow *m_game_window;
        const char *m_title;
        vec2<uint32_t> m_size;
        dengInputMode m_input_mode;
        vec2<double> m_pixel_size;

    public:
        Window(const uint32_t &x, const uint32_t &y, const char *title);
        ~Window();
        GLFWwindow *getWindow();
        const char *getTitle();
        dengInputMode getInputMode();
        vec2<uint32_t> getSize();
        vec2<double> getPixelSize();
        void setInputMode(const dengInputMode &new_input_mode);
    };
}

#endif