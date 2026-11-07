#pragma once

#include <vishwakarma/platform/Window.hpp>

#include <GLFW/glfw3.h>

#include <exception>
#include <memory>
#include <thread>

namespace vishwakarma::platform::detail {

struct GlfwRuntime;

struct WindowState {
    std::shared_ptr<GlfwRuntime> runtime;
    GLFWwindow* window = nullptr;
    std::thread::id ownerThread;
    int width = 0;
    int height = 0;
    Window::ResizeCallback resizeCallback;
    std::exception_ptr callbackException;

    ~WindowState() noexcept;
};

} // namespace vishwakarma::platform::detail

namespace vishwakarma::graphics::detail {

struct WindowAccess {
    [[nodiscard]] static std::shared_ptr<platform::detail::WindowState>
    retain(platform::Window& window) {
        return window.m_state;
    }
};

} // namespace vishwakarma::graphics::detail
