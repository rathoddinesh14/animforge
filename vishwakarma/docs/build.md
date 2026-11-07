# Build and test

## Requirements

- CMake 3.25+
- A C++20 compiler and C compiler (GLFW may be built from source)
- OpenGL development libraries
- GLFW 3.3+; if unavailable, CMake fetches GLFW 3.4
- Catch2 3 when tests are enabled; CMake fetches Catch2 3.7.1 if needed

When building GLFW on Linux, install the platform development dependencies
required by GLFW (X11 with Xrandr, Xinerama, Xi, and Xcursor in the default
configuration). A display server is required to run the window/context tests
and triangle sample. Headless Linux CI can use Xvfb with Mesa software
rendering.

## Configure, build, and test

From the Vishwakarma repository root:

```sh
cmake -S . -B build -DVISHWAKARMA_BUILD_SAMPLES=ON -DVISHWAKARMA_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Set either `VISHWAKARMA_BUILD_SAMPLES` or `VISHWAKARMA_BUILD_TESTS` to `OFF` to
omit that target group. `Vishwakarma::Graphics` is also available to a parent
CMake project through `add_subdirectory()`.

## Runtime checks

The triangle supports a bounded smoke run that renders one frame and exits
normally:

```sh
./build/samples/vishwakarma_triangle --frames 1
```

For headless Linux with Xvfb installed:

```sh
xvfb-run -a env LIBGL_ALWAYS_SOFTWARE=1 ./build/samples/vishwakarma_triangle --frames 1
xvfb-run -a env LIBGL_ALWAYS_SOFTWARE=1 ctest --test-dir build --output-on-failure
```

The tests cover window/runtime lifetime, framebuffer resize callbacks,
OpenGL initialization and drawing, device-outlives-wrapper safety, and the
existing graphics/geometry checks.

The verified environment for this snapshot is Linux (Ubuntu 24.04), GCC 13.3,
CMake 3.28, GLFW 3.4, and Mesa software rendering under Xvfb. Other platforms
and drivers have not been validated here.
