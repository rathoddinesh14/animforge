#pragma once

#include <functional>
#include <memory>
#include <string>

namespace vishwakarma::platform::detail {
struct WindowState;
}

namespace vishwakarma::graphics::detail {
struct WindowAccess;
}

namespace vishwakarma::platform {

struct WindowDesc {
    int width = 1280;
    int height = 720;
    std::string title = "Vishwakarma";
    bool resizable = true;
    bool debugContext = false;
};

/// Owns a GLFW window and its OpenGL context.
///
/// All windows and devices must be created, used, and destroyed on the thread
/// that creates the first GLFW window (normally the application main thread).
class Window {
public:
    explicit Window(const WindowDesc& desc);
    ~Window() noexcept;

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    [[nodiscard]] bool shouldClose() const;
    void pollEvents();
    void swapBuffers();

    [[nodiscard]] int width() const;
    [[nodiscard]] int height() const;

    using ResizeCallback = std::function<void(int, int)>;
    void setResizeCallback(ResizeCallback cb);
    void setSize(int width, int height);

private:
    friend struct ::vishwakarma::graphics::detail::WindowAccess;

    std::shared_ptr<detail::WindowState> m_state;
};

} // namespace vishwakarma::platform
