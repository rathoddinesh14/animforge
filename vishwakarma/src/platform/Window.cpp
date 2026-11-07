#include <vishwakarma/platform/Window.hpp>

#include "WindowState.hpp"

#include <cstdio>
#include <exception>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

namespace vishwakarma::platform::detail {
namespace {

void glfwErrorCallback(int code, const char* description) {
    std::fprintf(stderr, "GLFW error %d: %s\n", code,
                 description ? description : "no description");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) noexcept {
    auto* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    if (!state) {
        return;
    }

    state->width = width;
    state->height = height;
    if (state->resizeCallback && !state->callbackException) {
        try {
            state->resizeCallback(width, height);
        } catch (...) {
            state->callbackException = std::current_exception();
        }
    }
}

} // namespace

struct GlfwRuntime {
    explicit GlfwRuntime(std::thread::id thread) : ownerThread(thread) {
        glfwSetErrorCallback(glfwErrorCallback);
        if (glfwInit() != GLFW_TRUE) {
            glfwTerminate();
            throw std::runtime_error("GLFW initialization failed; see the GLFW error above");
        }
    }

    ~GlfwRuntime() noexcept {
        if (std::this_thread::get_id() != ownerThread) {
            std::terminate();
        }
        glfwTerminate();
    }

    std::thread::id ownerThread;
};

namespace {

struct RuntimeRegistry {
    std::mutex mutex;
    std::weak_ptr<GlfwRuntime> runtime;
};

RuntimeRegistry& runtimeRegistry() {
    static RuntimeRegistry registry;
    return registry;
}

std::shared_ptr<GlfwRuntime> acquireRuntime() {
    const auto currentThread = std::this_thread::get_id();
    auto& registry = runtimeRegistry();
    std::lock_guard lock(registry.mutex);
    if (auto runtime = registry.runtime.lock()) {
        if (runtime->ownerThread != currentThread) {
            throw std::logic_error("GLFW runtime must be used on its owning thread");
        }
        return runtime;
    }

    auto runtime = std::make_shared<GlfwRuntime>(currentThread);
    registry.runtime = runtime;
    return runtime;
}

void requireOwnerThread(const WindowState& state) {
    if (std::this_thread::get_id() != state.ownerThread) {
        throw std::logic_error("GLFW window used from a thread other than its owner");
    }
}

} // namespace

WindowState::~WindowState() noexcept {
    if (std::this_thread::get_id() != ownerThread) {
        std::terminate();
    }
    if (window) {
        glfwSetFramebufferSizeCallback(window, nullptr);
        glfwSetWindowUserPointer(window, nullptr);
        if (glfwGetCurrentContext() == window) {
            glfwMakeContextCurrent(nullptr);
        }
        glfwDestroyWindow(window);
        window = nullptr;
    }
}

} // namespace vishwakarma::platform::detail

namespace vishwakarma::platform {

Window::Window(const WindowDesc& desc) {
    if (desc.width <= 0 || desc.height <= 0) {
        throw std::invalid_argument("Window dimensions must be positive");
    }

    auto state = std::make_shared<detail::WindowState>();
    state->ownerThread = std::this_thread::get_id();
    state->runtime = detail::acquireRuntime();

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, desc.debugContext ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, desc.resizable ? GLFW_TRUE : GLFW_FALSE);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    state->window = glfwCreateWindow(desc.width, desc.height, desc.title.c_str(), nullptr, nullptr);
    if (!state->window) {
        throw std::runtime_error("GLFW window creation failed; see the GLFW error above");
    }

    glfwMakeContextCurrent(state->window);
    if (glfwGetCurrentContext() != state->window) {
        throw std::runtime_error("GLFW failed to make the new OpenGL context current");
    }
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(state->window, state.get());
    glfwSetFramebufferSizeCallback(state->window, detail::framebufferSizeCallback);
    glfwGetFramebufferSize(state->window, &state->width, &state->height);
    m_state = std::move(state);
}

Window::~Window() noexcept {
    if (m_state && std::this_thread::get_id() != m_state->ownerThread) {
        std::terminate();
    }
    if (m_state) {
        m_state->resizeCallback = {};
    }
}

bool Window::shouldClose() const {
    detail::requireOwnerThread(*m_state);
    return glfwWindowShouldClose(m_state->window) == GLFW_TRUE;
}

void Window::pollEvents() {
    detail::requireOwnerThread(*m_state);
    glfwPollEvents();
    if (m_state->callbackException) {
        auto exception = std::exchange(m_state->callbackException, nullptr);
        std::rethrow_exception(exception);
    }
}

void Window::swapBuffers() {
    detail::requireOwnerThread(*m_state);
    glfwSwapBuffers(m_state->window);
}

int Window::width() const {
    detail::requireOwnerThread(*m_state);
    return m_state->width;
}

int Window::height() const {
    detail::requireOwnerThread(*m_state);
    return m_state->height;
}

void Window::setResizeCallback(ResizeCallback callback) {
    detail::requireOwnerThread(*m_state);
    m_state->resizeCallback = std::move(callback);
}

void Window::setSize(int width, int height) {
    detail::requireOwnerThread(*m_state);
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Window dimensions must be positive");
    }
    glfwSetWindowSize(m_state->window, width, height);
}

} // namespace vishwakarma::platform
