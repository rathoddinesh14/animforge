# Vishwakarma integration

AnimForge consumes the pinned `Vishwakarma::Graphics` CMake target for its
OpenGL samples. The pinned revision is
`2b5f2f8a156f669552a60973ea9ab1a15c47ff97` from
`rathoddinesh14/vishwakarma`.

The dependency provides `platform::Window`, the graphics device factory and
contract, OpenGL context/function loading, and OpenGL rendering. The OpenGL
sample launch loop remains in AnimForge because it adapts AnimForge's sample
geometry and executable conventions to the public API.

AnimForge intentionally does not build the dependency's own sample or test
targets when fetching it. AnimForge has a small graphics integration test; the
library's full window and loader lifecycle suite remains owned by Vishwakarma.
OpenGL sample executables accept `--frames N` to render a bounded number of
frames and exit through the normal device/window shutdown path.

For local work against an adjacent Vishwakarma checkout, override FetchContent
without changing the pinned revision:

```sh
cmake -S . -B build/local -DFETCHCONTENT_SOURCE_DIR_VISHWAKARMA=/path/to/vishwakarma
```

The Vulkan sample path remains separate: it uses Vulkan directly and owns its
GLFW no-API sample window. The OpenGL migration does not add a Vulkan backend
to Vishwakarma or change AnimForge's animation systems.
