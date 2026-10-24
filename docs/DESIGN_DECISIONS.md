# Design Decisions

This document records the major technical choices and the reasoning behind them. Decisions can be revisited when evidence (profiling, complexity, or learning goals) warrants it.

## 1. Language & Standard
**Choice**: C++20 (minimum), C++23 features used when widely supported on MSVC.  
**Rationale**: Concepts, ranges, `std::span`, designated initializers, and better constexpr support significantly improve clarity and safety for math and data structures. We avoid bleeding-edge features that break MSVC stability.

## 2. Build System
**Choice**: CMake ≥ 3.25 + CMake Presets + vcpkg.  
**Rationale**: Industry standard, excellent Visual Studio integration, reproducible dependencies via manifests. Avoids custom build scripts.

## 3. Math Library
**Choice**: Thin custom math layer (`aga::math`) with optional GLM for samples and interop.  
**Rationale**:  
- Full control over conventions (column-major, right-handed, Y-up).  
- No hidden costs or unexpected operator behavior.  
- Easy to teach and audit the exact operations used in skinning and IK.  
GLM remains available for rapid prototyping and comparison.

## 4. Asset Format
**Choice**: glTF 2.0 as primary runtime format. Assimp as optional importer for authoring formats.  
**Rationale**: glTF is modern, well-specified, has excellent skinning and animation support, and is easy to inspect. Assimp gives flexibility during development without locking the runtime to a heavy library.

## 5. Skinning Technique (Initial)
**Choice**: Linear Blend Skinning (LBS) with ≤ 4 influences. Dual-quaternion skinning planned as a later alternative.  
**Rationale**: LBS is the industry baseline and must be mastered first. Dual-quaternion addresses volume loss but adds complexity; we implement it only after LBS is solid and measured.

## 6. OpenGL vs Vulkan Priority
**Choice**: OpenGL path first for rapid iteration and learning, Vulkan path developed in parallel once the core evaluation is correct.  
**Rationale**: OpenGL allows faster feedback while the mathematics and data structures are still evolving. Vulkan forces explicit resource management and is the long-term production target.

## 7. Animation Graph Complexity
**Choice**: Start with a simple layered blender + lightweight state machine. Full motion matching later.  
**Rationale**: Most production problems (foot sliding, transitions, additive layers) can already be studied with a good blender and state machine. Motion matching is a large topic that benefits from a solid foundation.

## 8. Memory & Performance Strategy
**Choice**:  
- Contiguous arrays of transforms.  
- Avoid virtual calls in the pose evaluation hot path.  
- CPU reference path always present for validation.  
**Rationale**: Data-oriented design keeps cache behavior predictable and makes GPU upload straightforward. Having a correct CPU path is non-negotiable for debugging.

## 9. Platform Support
**Choice**: Windows (MSVC) first. Linux and macOS are secondary goals.  
**Rationale**: User’s current primary environment. Clean platform abstraction keeps future ports low-cost.

## 10. Testing Philosophy
**Choice**: Unit tests for math, pose evaluation, and blending. Visual regression via the sample viewer.  
**Rationale**: Numerical correctness of interpolation and hierarchical transforms is critical. Visual inspection catches skinning and IK artifacts that unit tests miss.

## Open Questions (to be decided later)
- Exact animation compression format.
- Whether to adopt a full ECS or keep a simpler hierarchical model for characters.
- Multi-threaded evaluation strategy for large crowds.