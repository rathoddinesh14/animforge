# OpenGL Function Pointers (`PFNGL…PROC`)

This document explains how OpenGL entry points are loaded at runtime, why types
like `PFNGLCLEARCOLORPROC` exist, and how AnimForge’s minimal loader
(`src/rhi/opengl/GlLoader.hpp`) uses them.

## 1. Problem

On modern platforms, OpenGL is not a single fixed DLL export table that every
program links against for all functions. After you create a context (GLFW does
this), you must **query the driver** for the address of each command you need.

If you call `glDrawArrays` without loading it, you either fail to link or you
call a null pointer and crash.

```text
Application                    Loader                     Driver / ICD
    |                            |                              |
    |  glfwMakeContextCurrent    |                              |
    |--------------------------->|                              |
    |  glfwGetProcAddress("glDrawArrays")                       |
    |--------------------------->|----------------------------->|
    |  function pointer <--------|<-----------------------------|
    |  glDrawArrays(...)         |                              |
    |------------------------------------------------------------>|
```

## 2. Naming: what `PFNGL…PROC` means

| Part | Meaning |
|------|---------|
| **PFN** | Pointer to FuNction |
| **GL** | OpenGL |
| **NAME** | Command name in capitals (`CLEARCOLOR`, `DRAWARRAYS`, …) |
| **PROC** | Procedure (function) |

Example:

```cpp
// Type: pointer to a function with glClearColor's signature
using PFNGLCLEARCOLORPROC = void (*)(GLfloat, GLfloat, GLfloat, GLfloat);

// Variable: holds the actual address after loading
PFNGLCLEARCOLORPROC glClearColor = nullptr;
```

Official extension headers and loaders (glad, GLEW) use the same convention.
Vulkan uses a similar idea: `PFN_vkCreateInstance`, etc.

## 3. Loading in AnimForge

`GlLoader.cpp` does not use glad2 (Python/jinja2). It loads only the symbols
`OpenGLDevice` needs:

```cpp
using GLADloadfunc = void* (*)(const char* name);

bool loadGl(GLADloadfunc load) {
    bool ok = true;
    ok = loadProc(glClearColor, load, "glClearColor") && ok;
    ok = loadProc(glDrawArrays, load, "glDrawArrays") && ok;
    // ...
    return ok;
}
```

At device creation:

```cpp
glfwMakeContextCurrent(window);
if (!loadGl(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
    throw std::runtime_error("loadGl failed");
}
```

`glfwGetProcAddress` is valid **only** with a current context.

## 4. Why `GLFW_INCLUDE_NONE`

If GLFW includes system `GL/gl.h`, the system header may declare `glClear` as a
**function**, while our loader defines `glClear` as a **function pointer
variable**. That is a redefinition error.

AnimForge defines `GLFW_INCLUDE_NONE` so GLFW does not pull in `GL/gl.h`. Our
loader owns the GL symbols.

## 5. Minimal example (educational)

```cpp
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// 1) Declare the pointer type and variable
using PFNGLCLEARCOLORPROC = void (*)(float, float, float, float);
PFNGLCLEARCOLORPROC glClearColor = nullptr;

// 2) After context is current, load
glClearColor = reinterpret_cast<PFNGLCLEARCOLORPROC>(
    glfwGetProcAddress("glClearColor"));

// 3) Call through the pointer
if (glClearColor) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}
```

Production code uses a table of many `PFNGL*PROC` entries (glad/GLEW) or a
trimmed loader like ours.

## 6. Mental model

| Concept | Role |
|---------|------|
| `PFNGLDRAWARRAYSPROC` | Type of the function pointer |
| `glDrawArrays` (variable) | Storage for the address |
| `"glDrawArrays"` (string) | Name queried from the driver |
| `glfwGetProcAddress` | Resolver (context-dependent) |

Calling `glDrawArrays(mode, first, count)` is syntactic sugar for invoking the
function pointer stored in that variable.

## 7. Relation to Vulkan

Vulkan is explicit about function pointers from the start (`vkGetInstanceProcAddr`,
`vkGetDeviceProcAddr`). OpenGL hid this behind loaders for decades; the mechanism
is the same idea: **resolve by name, call through a pointer**.

## 8. Debugging tips

| Symptom | Likely cause |
|---------|----------------|
| Crash in first GL call | Loader not run, or context not current |
| Link errors on `gl*` | Mixing system GL headers with pointer variables |
| `nullptr` after load | Typo in name string, or context lost |
| Works on machine A, not B | Different driver / incomplete context version |

## 9. Where this lives in the repo

| File | Role |
|------|------|
| `src/rhi/opengl/GlLoader.hpp` | Types, constants, `extern` pointers |
| `src/rhi/opengl/GlLoader.cpp` | Definitions + `loadGl` |
| `src/rhi/opengl/OpenGLDevice.cpp` | Calls `loadGl` after making context current |

When adding a new GL call in `OpenGLDevice`, add a matching `PFNGL…PROC`,
`extern` pointer, definition, and `loadProc` line in the loader.


## 10. Full `loadProc` pattern (as in GlLoader)

```cpp
template <typename T>
bool loadProc(T& fn, GLADloadfunc load, const char* name) {
    fn = reinterpret_cast<T>(load(name));
    return fn != nullptr;
}

// Usage inside loadGl:
ok = loadProc(glCreateShader, load, "glCreateShader") && ok;
ok = loadProc(glShaderSource, load, "glShaderSource") && ok;
```

If any load fails, `loadGl` returns `false` and `OpenGLDevice` throws. That is preferable to
calling through a null pointer later.

## 11. Adding a new GL entry point (checklist)

1. Add `constexpr` enums if you need new constants (e.g. `GL_POINTS`).
2. Add `using PFNGL…PROC = …;`
3. Add `extern PFNGL…PROC glName;`
4. Define `PFNGL…PROC glName = nullptr;` in `GlLoader.cpp`
5. Call `loadProc(glName, load, "glName")` in `loadGl`
6. Use `glName(...)` from `OpenGLDevice.cpp` only after `loadGl` succeeds

## 12. Example: why types and variables are separate

```cpp
// Without a typedef you would write:
void (*glDrawArrays)(GLenum, GLint, GLsizei) = nullptr;

// With the conventional alias:
using PFNGLDRAWARRAYSPROC = void (*)(GLenum, GLint, GLsizei);
PFNGLDRAWARRAYSPROC glDrawArrays = nullptr;
```

The alias keeps tables of function pointers readable and matches headers used across
the OpenGL ecosystem.

## 13. Related reading in this repo

- `docs/CODING_STANDARDS.md` - project style
- `docs/MATH_CONVENTIONS.md` - GLM / coordinate system
- `src/rhi/opengl/OpenGLDevice.cpp` - first real consumers of loaded pointers
