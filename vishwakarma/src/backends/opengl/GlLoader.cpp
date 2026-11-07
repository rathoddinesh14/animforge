#include "GlLoader.hpp"

#include <stdexcept>
#include <string>
#include <vector>

namespace {

template <typename T>
void loadProc(T& function, GLProcAddress resolver, const char* name,
              std::vector<std::string>& missing) {
    function = reinterpret_cast<T>(resolver(name));
    if (!function) {
        missing.emplace_back(name);
    }
}

} // namespace

GLFunctions loadGl(GLProcAddress resolver) {
    if (!resolver) {
        throw std::invalid_argument("OpenGL function loader received a null resolver");
    }

    GLFunctions functions;
    std::vector<std::string> missing;
    loadProc(functions.clearColor, resolver, "glClearColor", missing);
    loadProc(functions.clear, resolver, "glClear", missing);
    loadProc(functions.viewport, resolver, "glViewport", missing);
    loadProc(functions.drawArrays, resolver, "glDrawArrays", missing);
    loadProc(functions.pointSize, resolver, "glPointSize", missing);
    loadProc(functions.genBuffers, resolver, "glGenBuffers", missing);
    loadProc(functions.bindBuffer, resolver, "glBindBuffer", missing);
    loadProc(functions.bufferData, resolver, "glBufferData", missing);
    loadProc(functions.deleteBuffers, resolver, "glDeleteBuffers", missing);
    loadProc(functions.genVertexArrays, resolver, "glGenVertexArrays", missing);
    loadProc(functions.bindVertexArray, resolver, "glBindVertexArray", missing);
    loadProc(functions.deleteVertexArrays, resolver, "glDeleteVertexArrays", missing);
    loadProc(functions.enableVertexAttribArray, resolver, "glEnableVertexAttribArray", missing);
    loadProc(functions.vertexAttribPointer, resolver, "glVertexAttribPointer", missing);
    loadProc(functions.createShader, resolver, "glCreateShader", missing);
    loadProc(functions.shaderSource, resolver, "glShaderSource", missing);
    loadProc(functions.compileShader, resolver, "glCompileShader", missing);
    loadProc(functions.getShaderiv, resolver, "glGetShaderiv", missing);
    loadProc(functions.getShaderInfoLog, resolver, "glGetShaderInfoLog", missing);
    loadProc(functions.deleteShader, resolver, "glDeleteShader", missing);
    loadProc(functions.createProgram, resolver, "glCreateProgram", missing);
    loadProc(functions.attachShader, resolver, "glAttachShader", missing);
    loadProc(functions.linkProgram, resolver, "glLinkProgram", missing);
    loadProc(functions.getProgramiv, resolver, "glGetProgramiv", missing);
    loadProc(functions.getProgramInfoLog, resolver, "glGetProgramInfoLog", missing);
    loadProc(functions.deleteProgram, resolver, "glDeleteProgram", missing);
    loadProc(functions.useProgram, resolver, "glUseProgram", missing);

    if (!missing.empty()) {
        std::string message = "Missing required OpenGL entry points:";
        for (const auto& name : missing) {
            message += "\n  ";
            message += name;
        }
        throw std::runtime_error(message);
    }
    return functions;
}
