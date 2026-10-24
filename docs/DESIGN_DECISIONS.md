# Design Decisions

This document records the major technical choices and the reasoning behind them. Decisions can be revisited when evidence (profiling, complexity, or learning goals) warrants it.

## 1. Language & Standard
**Choice**: C++20 (minimum), C++23 features used when widely supported on MSVC.  
**Rationale**: Concepts, ranges, `std::span`, designated initializers, and better constexpr support significantly improve clarity and safety for math and data structures. We avoid bleeding-edge features that break MSVC stability.

## 2. Build System
**Choice**: CMake ≥ 3.25 + CMake Presets + vcpkg.  
**Rationale**: Industry standard, excellent Visual Studio integration, reproducible dependencies via manifests. Avoids custom build scripts.

## 3. Math Library
**Choice**: **GLM** as the primary mathematics library, configured and exposed through `aga::math`.  
**Rationale**:  
- GLM is designed to match OpenGL/GLSL conventions (column-major, right-handed).  
- Excellent consistency with both OpenGL and Vulkan (Vulkan only requires a Y-flip + [0,1] depth in the projection matrix).  
- Mature, widely understood, header-only, and available via vcpkg.  
- We force the exact configuration we need via compile definitions and a thin `aga/math/math.hpp` header so that every translation unit sees the same conventions.  
- A pure custom math layer remains an option later if we need maximum control or smaller binary size; for now GLM gives the best productivity / correctness trade-off while we master animation techniques.

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

## 10. Testing Framework & Philosophy
**Choice**: **Catch2 v3** + strict Test-Driven Development (TDD).  
**Rationale**:  
- Catch2’s expression-based assertions and optional BDD style (`SCENARIO` / `GIVEN` / `WHEN` / `THEN`) produce highly readable tests that themselves teach the mathematics and expected behaviour.  
- Low boilerplate keeps the focus on the algorithm rather than the test harness.  
- Excellent fit for a mastery-oriented codebase where tests should be easy to read and reason about.  
- Available via vcpkg and integrates cleanly with CMake.  

Google Test remains a possible future addition if we need its death-test facilities or extremely advanced matchers, but Catch2 is the default.

**TDD Workflow (mandatory for core modules)**:
1. Write a failing test that expresses the desired behaviour or mathematical property.
2. Implement the minimal code that makes the test pass.
3. Refactor while keeping the test green.
4. Only then move to the next behaviour.

This applies especially to: math types, pose evaluation, blending, IK solvers, and skinning.

Visual regression via the sample viewer remains essential for skinning and rendering artefacts that pure unit tests cannot catch.

## Open Questions (to be decided later)
- Exact animation compression format.
- Whether to adopt a full ECS or keep a simpler hierarchical model for characters.
- Multi-threaded evaluation strategy for large crowds.