# Architecture

Vishwakarma is a small graphics foundation with four layers:

1. `platform::Window` owns a GLFW window and OpenGL context.
2. `graphics::IDevice` defines the backend-neutral frame, resize, and primitive
   draw operations using only Vishwakarma types.
3. `graphics::createDevice` binds an OpenGL device to a window without exposing
   native window handles in its public signature.
4. `backends/opengl` privately loads the required OpenGL 4.5 entry points and
   implements the device contract.

`Vishwakarma::Graphics` is the CMake library target. Public headers live under
`include/vishwakarma`; GLFW and OpenGL headers are confined to implementation
files. A device retains private shared window state, which keeps its context
alive even if the `Window` wrapper is destroyed first.

The current vertex format is seven interleaved floats per vertex (`x y z r g b
a`). The OpenGL backend uploads those vertices for the requested primitive
topology. It is intentionally not a general render graph, resource manager, or
asset system.

The dependency set is C++20, CMake, GLFW, OpenGL, and the platform thread
library. Catch2 is needed only for tests. See [build.md](build.md).
