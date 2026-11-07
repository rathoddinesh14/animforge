#pragma once

#include <functional>
#include <string>

struct GLFWwindow;

namespace vishwakarma::platform {

struct WindowDesc {
    int width = 1280;
    int height = 720;
    std::string title = "Vishwakarma";
    bool resizable = true;
};

/// GLFW window and OpenGL context.
class Window {
public:
    explicit Window(const WindowDesc& desc);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    [[nodiscard]] GLFWwindow* handle() const noexcept { return m_window; }
    [[nodiscard]] bool shouldClose() const;
    void pollEvents();
    void swapBuffers();

    [[nodiscard]] int width() const noexcept { return m_width; }
    [[nodiscard]] int height() const noexcept { return m_height; }

    using ResizeCallback = std::function<void(int, int)>;
    void setResizeCallback(ResizeCallback cb);

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* m_window = nullptr;
    int m_width = 0;
    int m_height = 0;
    ResizeCallback m_resizeCallback;
};

} // namespace vishwakarma::platform
