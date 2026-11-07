# Roadmap

## Current foundation

- GLFW runtime/window lifetime management
- OpenGL 4.5 context and minimal function loading
- Minimal graphics device contract and factory
- Frame clear, framebuffer resize, and point/line/triangle drawing
- Standalone triangle sample and lifecycle/API tests

## Near-term quality work

- Add CI builds for the compiler/platform combinations the project chooses to
  support.
- Expand diagnostics and tests for context-creation and shader-failure paths.
- Consider a generated or maintained OpenGL loader if the backend grows
  substantially.

## Deliberate non-goals for this stage

Vulkan, animation, asset import, editor integration, render graphs, advanced
resource abstractions, and platform-specific rendering features are not part
of this foundation. Add those only when a concrete consumer requires them.
