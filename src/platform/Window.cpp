#include <aga/platform/Window.hpp>

#include <stdexcept>

#include <GLFW/glfw3.h>

namespace aga::platform {

namespace {

void glfwErrorCallback(int code, const char* description) {
    // Logging subsystem comes later; keep visible in debug samples.
    (void)code;
    (void)description;
}

} // namespace

Window::Window(const WindowDesc& desc) : m_width(desc.width), m_height(desc.height) {
    if (!glfwInit()) {
        throw std::runtime_error("glfwInit failed");
    }
    glfwSetErrorCallback(glfwErrorCallback);

    // OpenGL 4.5 core - matches our target for skinning samples later.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    m_window = glfwCreateWindow(desc.width, desc.height, desc.title.c_str(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("glfwCreateWindow failed");
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // vsync on for samples
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
}

Window::~Window() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window) == GLFW_TRUE;
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_window);
}

void Window::setResizeCallback(ResizeCallback cb) {
    m_resizeCallback = std::move(cb);
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self) {
        return;
    }
    self->m_width = width;
    self->m_height = height;
    if (self->m_resizeCallback) {
        self->m_resizeCallback(width, height);
    }
}

} // namespace aga::platform
