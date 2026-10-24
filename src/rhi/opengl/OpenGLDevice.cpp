#include "OpenGLDevice.hpp"

#include "GlLoader.hpp"

#include <stdexcept>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace aga::rhi {
namespace {

const char* kVertexShader = R"GLSL(
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
out vec4 vColor;
void main() {
    vColor = aColor;
    gl_Position = vec4(aPos, 1.0);
}
)GLSL";

const char* kFragmentShader = R"GLSL(
#version 450 core
in vec4 vColor;
out vec4 fragColor;
void main() {
    fragColor = vColor;
}
)GLSL";

[[nodiscard]] unsigned compileShader(unsigned type, const char* source) {
    const unsigned shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    int ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        glDeleteShader(shader);
        throw std::runtime_error(std::string("Shader compile failed: ") + log);
    }
    return shader;
}

} // namespace

OpenGLDevice::OpenGLDevice(GLFWwindow* window, const DeviceDesc& desc)
    : m_window(window), m_width(desc.framebufferWidth), m_height(desc.framebufferHeight) {
    if (!m_window) {
        throw std::runtime_error("OpenGLDevice: null window");
    }
    glfwMakeContextCurrent(m_window);

    if (!loadGl(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
        throw std::runtime_error("loadGl failed");
    }

    createPipeline();
    resize(m_width, m_height);
}

OpenGLDevice::~OpenGLDevice() {
    destroyPipeline();
}

void OpenGLDevice::createPipeline() {
    const unsigned vs = compileShader(GL_VERTEX_SHADER, kVertexShader);
    const unsigned fs = compileShader(GL_FRAGMENT_SHADER, kFragmentShader);
    m_program = glCreateProgram();
    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);
    glLinkProgram(m_program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    int ok = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(m_program, sizeof(log), nullptr, log);
        throw std::runtime_error(std::string("Program link failed: ") + log);
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    const int stride = static_cast<int>(7 * sizeof(float));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glBindVertexArray(0);
}

void OpenGLDevice::destroyPipeline() {
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}

void OpenGLDevice::resize(int width, int height) {
    m_width = width > 0 ? width : 1;
    m_height = height > 0 ? height : 1;
    glViewport(0, 0, m_width, m_height);
}

void OpenGLDevice::beginFrame(const math::Vec4& clearColor) {
    // GLM_FORCE_XYZW_ONLY: use .xyzw not .rgba
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLDevice::draw(PrimitiveTopology topology, const float* vertices,
                         std::size_t vertexCount) {
    if (!vertices || vertexCount == 0) {
        return;
    }

    GLenum mode = GL_TRIANGLES;
    switch (topology) {
    case PrimitiveTopology::PointList:
        mode = GL_POINTS;
        if (glPointSize) {
            glPointSize(12.0f);
        }
        break;
    case PrimitiveTopology::LineList:
        mode = GL_LINES;
        if ((vertexCount % 2) != 0) {
            return;
        }
        break;
    case PrimitiveTopology::LineStrip:
        mode = GL_LINE_STRIP;
        break;
    case PrimitiveTopology::TriangleList:
        mode = GL_TRIANGLES;
        if ((vertexCount % 3) != 0) {
            return;
        }
        break;
    case PrimitiveTopology::TriangleStrip:
        mode = GL_TRIANGLES;
        break;
    }

    const auto byteSize = static_cast<GLsizeiptr>(vertexCount * 7 * sizeof(float));
    glUseProgram(m_program);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, byteSize, vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(mode, 0, static_cast<GLsizei>(vertexCount));
    glBindVertexArray(0);
}

void OpenGLDevice::endFrame() {
    (void)m_window;
}

} // namespace aga::rhi
