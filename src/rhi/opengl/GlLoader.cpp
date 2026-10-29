#include "GlLoader.hpp"

PFNGLCLEARCOLORPROC glClearColor = nullptr;
PFNGLCLEARPROC glClear = nullptr;
PFNGLVIEWPORTPROC glViewport = nullptr;
PFNGLDRAWARRAYSPROC glDrawArrays = nullptr;
PFNGLPOINTSIZEPROC glPointSize = nullptr;
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLDELETESHADERPROC glDeleteShader = nullptr;
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;

namespace {

template <typename T>
bool loadProc(T& fn, GLADloadfunc load, const char* name) {
    fn = reinterpret_cast<T>(load(name));
    return fn != nullptr;
}

} // namespace

bool loadGl(GLADloadfunc load) {
    bool ok = true;
    ok = loadProc(glClearColor, load, "glClearColor") && ok;
    ok = loadProc(glClear, load, "glClear") && ok;
    ok = loadProc(glViewport, load, "glViewport") && ok;
    ok = loadProc(glDrawArrays, load, "glDrawArrays") && ok;
    ok = loadProc(glPointSize, load, "glPointSize") && ok;
    ok = loadProc(glGenBuffers, load, "glGenBuffers") && ok;
    ok = loadProc(glBindBuffer, load, "glBindBuffer") && ok;
    ok = loadProc(glBufferData, load, "glBufferData") && ok;
    ok = loadProc(glDeleteBuffers, load, "glDeleteBuffers") && ok;
    ok = loadProc(glGenVertexArrays, load, "glGenVertexArrays") && ok;
    ok = loadProc(glBindVertexArray, load, "glBindVertexArray") && ok;
    ok = loadProc(glDeleteVertexArrays, load, "glDeleteVertexArrays") && ok;
    ok = loadProc(glEnableVertexAttribArray, load, "glEnableVertexAttribArray") && ok;
    ok = loadProc(glVertexAttribPointer, load, "glVertexAttribPointer") && ok;
    ok = loadProc(glCreateShader, load, "glCreateShader") && ok;
    ok = loadProc(glShaderSource, load, "glShaderSource") && ok;
    ok = loadProc(glCompileShader, load, "glCompileShader") && ok;
    ok = loadProc(glGetShaderiv, load, "glGetShaderiv") && ok;
    ok = loadProc(glGetShaderInfoLog, load, "glGetShaderInfoLog") && ok;
    ok = loadProc(glDeleteShader, load, "glDeleteShader") && ok;
    ok = loadProc(glCreateProgram, load, "glCreateProgram") && ok;
    ok = loadProc(glAttachShader, load, "glAttachShader") && ok;
    ok = loadProc(glLinkProgram, load, "glLinkProgram") && ok;
    ok = loadProc(glGetProgramiv, load, "glGetProgramiv") && ok;
    ok = loadProc(glGetProgramInfoLog, load, "glGetProgramInfoLog") && ok;
    ok = loadProc(glDeleteProgram, load, "glDeleteProgram") && ok;
    ok = loadProc(glUseProgram, load, "glUseProgram") && ok;
    return ok;
}
