# OpenGL function loading

Vishwakarma requests OpenGL entry points from GLFW after making its context
current. The private loader in `src/backends/opengl/GlLoader.hpp` declares the
function pointer types and OpenGL constants used by the backend, while the
corresponding `.cpp` resolves each required entry point with
`glfwGetProcAddress`.

The loader deliberately contains only the functions used by the current
OpenGL backend. A missing required entry point causes device initialization to
fail rather than allowing a later call through a null pointer.

`GLFW_INCLUDE_NONE` prevents GLFW from including system OpenGL declarations,
which could conflict with the loader's function pointer variables.

When adding an OpenGL operation, add its function pointer type, declaration,
definition, and loading call to the loader, then use it only after
`loadGl()` succeeds. No OpenGL headers are exposed by Vishwakarma's public API.
