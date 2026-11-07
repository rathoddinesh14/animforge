#include "OpenGLDevice.hpp"

#include "GlLoader.hpp"
#include "../../platform/WindowState.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <GLFW/glfw3.h>

namespace vishwakarma::graphics {
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

[[nodiscard]] std::string shaderLog(const GLFunctions& gl, GLuint shader) {
    GLint length = 0;
    gl.getShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::vector<GLchar> log(static_cast<std::size_t>(std::max(length, 1)), '\0');
    GLsizei written = 0;
    gl.getShaderInfoLog(shader, static_cast<GLsizei>(log.size()), &written, log.data());
    return std::string(log.data(), static_cast<std::size_t>(std::max(written, 0)));
}

[[nodiscard]] std::string programLog(const GLFunctions& gl, GLuint program) {
    GLint length = 0;
    gl.getProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    std::vector<GLchar> log(static_cast<std::size_t>(std::max(length, 1)), '\0');
    GLsizei written = 0;
    gl.getProgramInfoLog(program, static_cast<GLsizei>(log.size()), &written, log.data());
    return std::string(log.data(), static_cast<std::size_t>(std::max(written, 0)));
}

[[nodiscard]] GLuint compileShader(const GLFunctions& gl, GLenum type, const char* source) {
    const GLuint shader = gl.createShader(type);
    if (shader == 0) {
        throw std::runtime_error("OpenGL failed to allocate a shader object");
    }

    gl.shaderSource(shader, 1, &source, nullptr);
    gl.compileShader(shader);
    GLint compiled = 0;
    gl.getShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == 0) {
        const auto log = shaderLog(gl, shader);
        gl.deleteShader(shader);
        throw std::runtime_error("OpenGL shader compilation failed: " + log);
    }
    return shader;
}

} // namespace

OpenGLDevice::OpenGLDevice(std::shared_ptr<platform::detail::WindowState> windowState)
    : m_windowState(std::move(windowState)), m_ownerThread(std::this_thread::get_id()) {
    if (!m_windowState || !m_windowState->window) {
        throw std::invalid_argument("OpenGLDevice requires a live Vishwakarma window");
    }
    if (m_ownerThread != m_windowState->ownerThread) {
        throw std::logic_error("OpenGL device must be created on the window's owning thread");
    }

    m_width = m_windowState->width;
    m_height = m_windowState->height;
    makeContextCurrent();
    m_gl = loadGl(glfwGetProcAddress);
    m_glLoaded = true;

    try {
        createPipeline();
        resize(m_width, m_height);
    } catch (...) {
        glfwMakeContextCurrent(m_windowState->window);
        if (glfwGetCurrentContext() == m_windowState->window) {
            destroyPipeline();
        }
        throw;
    }
}

OpenGLDevice::~OpenGLDevice() noexcept {
    if (m_glLoaded) {
        if (std::this_thread::get_id() != m_ownerThread) {
            std::terminate();
        }
        glfwMakeContextCurrent(m_windowState->window);
        if (glfwGetCurrentContext() == m_windowState->window) {
            destroyPipeline();
        }
    }
}

void OpenGLDevice::makeContextCurrent() const {
    if (std::this_thread::get_id() != m_ownerThread) {
        throw std::logic_error("OpenGL device used from a thread other than its owner");
    }
    if (!m_windowState || !m_windowState->window) {
        throw std::logic_error("OpenGL device has no live window context");
    }
    if (glfwGetCurrentContext() != m_windowState->window) {
        glfwMakeContextCurrent(m_windowState->window);
    }
    if (glfwGetCurrentContext() != m_windowState->window) {
        throw std::runtime_error("Failed to make the OpenGL device context current");
    }
}

void OpenGLDevice::createPipeline() {
    const GLuint vertexShader = compileShader(m_gl, GL_VERTEX_SHADER, kVertexShader);
    GLuint fragmentShader = 0;
    try {
        fragmentShader = compileShader(m_gl, GL_FRAGMENT_SHADER, kFragmentShader);
        m_program = m_gl.createProgram();
        if (m_program == 0) {
            throw std::runtime_error("OpenGL failed to allocate a shader program");
        }
        m_gl.attachShader(m_program, vertexShader);
        m_gl.attachShader(m_program, fragmentShader);
        m_gl.linkProgram(m_program);

        GLint linked = 0;
        m_gl.getProgramiv(m_program, GL_LINK_STATUS, &linked);
        if (linked == 0) {
            throw std::runtime_error("OpenGL shader program linking failed: " +
                                     programLog(m_gl, m_program));
        }
    } catch (...) {
        m_gl.deleteShader(vertexShader);
        if (fragmentShader != 0) {
            m_gl.deleteShader(fragmentShader);
        }
        if (m_program != 0) {
            m_gl.deleteProgram(m_program);
            m_program = 0;
        }
        throw;
    }
    m_gl.deleteShader(vertexShader);
    m_gl.deleteShader(fragmentShader);

    m_gl.genVertexArrays(1, &m_vao);
    m_gl.genBuffers(1, &m_vbo);
    if (m_vao == 0 || m_vbo == 0) {
        throw std::runtime_error("OpenGL failed to allocate vertex array or buffer objects");
    }

    m_gl.bindVertexArray(m_vao);
    m_gl.bindBuffer(GL_ARRAY_BUFFER, m_vbo);
    const auto stride = static_cast<GLsizei>(7 * sizeof(float));
    m_gl.enableVertexAttribArray(0);
    m_gl.vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    m_gl.enableVertexAttribArray(1);
    m_gl.vertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride,
                             reinterpret_cast<const void*>(3 * sizeof(float)));
    m_gl.bindVertexArray(0);
}

void OpenGLDevice::destroyPipeline() noexcept {
    if (m_vbo != 0) {
        m_gl.deleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_vao != 0) {
        m_gl.deleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_program != 0) {
        m_gl.deleteProgram(m_program);
        m_program = 0;
    }
}

void OpenGLDevice::resize(int width, int height) {
    makeContextCurrent();
    m_width = width > 0 ? width : 1;
    m_height = height > 0 ? height : 1;
    m_gl.viewport(0, 0, m_width, m_height);
}

void OpenGLDevice::beginFrame(const Color& clearColor) {
    makeContextCurrent();
    m_gl.clearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    m_gl.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLDevice::drawPoints(const float* vertices, std::size_t vertexCount) {
    if (vertexCount == 0) {
        return;
    }
    if (m_gl.pointSize) {
        makeContextCurrent();
        m_gl.pointSize(12.0f);
    }
    drawVertices(GL_POINTS, vertices, vertexCount);
}

void OpenGLDevice::drawLines(const float* vertices, std::size_t vertexCount) {
    if (vertexCount < 2 || vertexCount % 2 != 0) {
        return;
    }
    drawVertices(GL_LINES, vertices, vertexCount);
}

void OpenGLDevice::drawTriangles(const float* vertices, std::size_t vertexCount) {
    if (vertexCount < 3 || vertexCount % 3 != 0) {
        return;
    }
    drawVertices(GL_TRIANGLES, vertices, vertexCount);
}

void OpenGLDevice::drawVertices(unsigned mode, const float* vertices, std::size_t vertexCount) {
    if (vertices == nullptr) {
        throw std::invalid_argument("Vertex data must not be null for a non-empty draw");
    }
    if (vertexCount == 0) {
        return;
    }
    if (vertexCount > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()) ||
        vertexCount > std::numeric_limits<std::size_t>::max() / (7 * sizeof(float))) {
        throw std::length_error("Vertex count exceeds OpenGL draw limits");
    }

    makeContextCurrent();
    const auto byteSize = static_cast<GLsizeiptr>(vertexCount * 7 * sizeof(float));
    m_gl.useProgram(m_program);
    m_gl.bindVertexArray(m_vao);
    m_gl.bindBuffer(GL_ARRAY_BUFFER, m_vbo);
    m_gl.bufferData(GL_ARRAY_BUFFER, byteSize, vertices, GL_DYNAMIC_DRAW);
    m_gl.drawArrays(mode, 0, static_cast<GLsizei>(vertexCount));
    m_gl.bindVertexArray(0);
}

void OpenGLDevice::endFrame() {}

} // namespace vishwakarma::graphics
