#pragma once

#include <aga/rhi/IDevice.hpp>

struct GLFWwindow;

namespace aga::rhi {

class OpenGLDevice final : public IDevice {
public:
    OpenGLDevice(GLFWwindow* window, const DeviceDesc& desc);
    ~OpenGLDevice() override;

    [[nodiscard]] Backend backend() const noexcept override { return Backend::OpenGL; }

    void resize(int width, int height) override;
    void beginFrame(const math::Vec4& clearColor) override;
    
    void drawPoints(const float* vertices, std::size_t vertexCount) override;
    void drawLines(const float* vertices, std::size_t vertexCount) override;
    void drawTriangles(const float* vertices, std::size_t vertexCount) override;

    void endFrame() override;

    [[nodiscard]] std::string_view name() const noexcept override { return "OpenGL"; }

private:
    void drawVertices(unsigned mode,
                  const float* vertices,
                  std::size_t vertexCount);
    void createPipeline();
    void destroyPipeline();

    GLFWwindow* m_window = nullptr;
    int m_width = 0;
    int m_height = 0;

    unsigned m_program = 0;
    unsigned m_vao = 0;
    unsigned m_vbo = 0;
};

} // namespace aga::rhi
