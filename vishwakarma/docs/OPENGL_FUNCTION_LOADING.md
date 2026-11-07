# OpenGL function loading

Vishwakarma requests OpenGL entry points from GLFW after making a device's
context current. The private loader in `src/backends/opengl/GlLoader.hpp`
declares the function pointer types and OpenGL constants used by the backend,
while the corresponding `.cpp` resolves each required entry point with
`glfwGetProcAddress`. The resulting `GLFunctions` table is retained per device,
so switching between contexts does not overwrite another device's dispatch
table.

The loader deliberately contains only the functions used by the current
OpenGL backend. A missing required entry point causes device initialization to
fail rather than allowing a later call through a null pointer.

`GLFW_INCLUDE_NONE` prevents GLFW from including system OpenGL declarations,
which could conflict with the loader's function pointer variables.

When adding an OpenGL operation, add its function pointer type and table
member, then add a load request in `loadGl()`. Use the function only from a
device after `loadGl()` has completed successfully. No OpenGL headers are
exposed by Vishwakarma's public API.
