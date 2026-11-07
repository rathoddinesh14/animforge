# Vishwakarma

Vishwakarma is an independent C++20 graphics foundation. It provides GLFW
window management, an OpenGL 4.5 core device, a minimal OpenGL function loader,
and a triangle sample. Its public headers do not expose GLFW or OpenGL types.

See [architecture](docs/architecture.md), [lifecycle](docs/lifecycle.md),
[build instructions](docs/build.md), and the [roadmap](docs/roadmap.md).

## Requirements

- CMake 3.25 or newer
- A C++20 compiler
- OpenGL development libraries
- GLFW 3.3 or newer (CMake fetches GLFW 3.4 if it is not installed)
- Catch2 3 is fetched only when tests are enabled and no package is available

On Linux, building GLFW from source also requires the X11 development packages
for Xrandr, Xinerama, Xi, and Xcursor.

## Build

```sh
cmake -S . -B build -DVISHWAKARMA_BUILD_SAMPLES=ON -DVISHWAKARMA_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

The `Vishwakarma::Graphics` target can also be consumed from another CMake
project with `add_subdirectory()`. Public headers are available beneath
`include/vishwakarma`; GLFW, OpenGL, and thread dependencies are private to the
library's implementation and propagated only as link requirements when needed.

Run the triangle sample from a graphical desktop session. For an automated
one-frame render followed by normal RAII shutdown:

```sh
./build/samples/vishwakarma_triangle --frames 1
```

## Extraction inventory

- `platform/Window`: GLFW window creation, OpenGL context setup, event polling,
  buffer swapping, and framebuffer resize notifications.
- `graphics`: the minimal device contract, OpenGL device factory, and
  interleaved primitive vertex layout constant.
- `backends/opengl`: GL function loading and OpenGL device implementation,
  including frame clearing, viewport updates, and point/line/triangle draws.
- `samples/00_triangle`: standalone colored triangle rendering sample and its
  reusable primitive geometry/helper code.
- `tests`: graphics API, factory, vertex layout, and sample geometry tests.
- `cmake`: dependency resolution and compiler warning configuration.

## Excluded

Animation, asset, editor, and application-specific systems remain in AnimForge.
The AnimForge math library was not copied; the device API uses a small
`std::array<float, 4>` color value instead. Existing line/point primitive
geometries are retained as sample utilities, but separate line and point
applications are not included. Other graphics backends and advanced rendering
systems are outside the current scope.

## License

MIT. See [LICENSE](LICENSE).
