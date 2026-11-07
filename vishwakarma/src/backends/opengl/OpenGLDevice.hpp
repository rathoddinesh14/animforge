#pragma once

#include <vishwakarma/graphics/IDevice.hpp>
#include <vishwakarma/platform/Window.hpp>

#include "GlLoader.hpp"

#include <memory>
#include <thread>

namespace vishwakarma::platform::detail {
struct WindowState;
}

namespace vishwakarma::graphics {

class OpenGLDevice final : public IDevice {
public:
    explicit OpenGLDevice(std::shared_ptr<platform::detail::WindowState> windowState);
    ~OpenGLDevice() noexcept override;

    [[nodiscard]] Backend backend() const noexcept override { return Backend::OpenGL; }

    void resize(int width, int height) override;
    void beginFrame(const Color& clearColor) override;

    void drawPoints(const float* vertices, std::size_t vertexCount) override;
    void drawLines(const float* vertices, std::size_t vertexCount) override;
    void drawTriangles(const float* vertices, std::size_t vertexCount) override;

    void endFrame() override;

    [[nodiscard]] std::string_view name() const noexcept override { return "OpenGL"; }

private:
    void makeContextCurrent() const;
    void drawVertices(unsigned mode, const float* vertices, std::size_t vertexCount);
    void createPipeline();
    void destroyPipeline() noexcept;

    std::shared_ptr<platform::detail::WindowState> m_windowState;
    std::thread::id m_ownerThread;
    GLFunctions m_gl;
    int m_width = 0;
    int m_height = 0;
    bool m_glLoaded = false;

    unsigned m_program = 0;
    unsigned m_vao = 0;
    unsigned m_vbo = 0;
};

} // namespace vishwakarma::graphics
