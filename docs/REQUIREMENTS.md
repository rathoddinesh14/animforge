# Requirements

## 1. Functional Requirements

### FR-1 Core Animation Runtime
- Load hierarchical skeleton from glTF 2.0 (primary) and optionally FBX via Assimp.
- Evaluate local and model-space poses from keyframe animation clips.
- Support linear blend skinning (LBS) with up to 4 influences per vertex (configurable).
- Play, pause, seek, loop, and speed-control individual clips.
- Extract and apply root motion.

### FR-2 Blending & Layering
- Cross-fade blending between clips with configurable transition curves.
- Additive animation layers.
- Partial-body / masked blending (bone masks).
- Simple state machine and lightweight blend tree.

### FR-3 Inverse Kinematics
- Two-bone analytic IK.
- FABRIK and CCD iterative solvers.
- Constraint support (hinge, ball-socket, twist limits).
- Full-body IK foundation (later phase).

### FR-4 Procedural & Secondary Motion
- Spring-damper systems for secondary motion.
- Procedural look-at / aim.
- Basic soft-body / cloth attachment points (later).

### FR-5 Rendering Integration
- OpenGL 4.5+ path: CPU skinning → GPU skinning (UBO/SSBO).
- Vulkan 1.2+ path: explicit buffer management, descriptor sets, compute skinning option.
- Debug visualization: skeleton, bone weights, blend weights, IK chains, root motion.

### FR-6 Tooling
- Minimal animation viewer sample.
- Runtime inspection of pose data.

## 2. Non-Functional Requirements

### NFR-1 Platform
- **Primary**: Windows 10/11, MSVC 2022 (C++20/23), CMake ≥ 3.25.
- Future: Linux (GCC/Clang), macOS (Apple Silicon + Intel).
- No hard dependency on a full engine; samples are self-contained.

### NFR-2 Performance Targets (initial)
- 60 FPS @ 1080p with ≥ 50 skinned characters (moderate bone count) on mid-range GPU.
- Pose evaluation (CPU) < 0.5 ms for a typical humanoid (60–80 bones) at 60 Hz.
- Memory: animation data streamed/compressed where possible; avoid unnecessary copies.

### NFR-3 Code Quality
- Modern C++20/23, RAII, const-correctness, clear ownership.
- Data-oriented design preferred for hot paths.
- No unnecessary abstraction layers.
- Unit tests for core math and pose evaluation.
- Clang-format + compiler warnings as errors in CI.

### NFR-4 Debuggability
- Visual debug drawing of every major system.
- Logging of pose evaluation failures and numerical issues.
- Ability to dump intermediate matrices/poses for offline analysis.

### NFR-5 Extensibility
- Clean separation between core animation evaluation and rendering backend.
- Pluggable interpolation methods and skinning techniques.
- Asset pipeline documented so custom formats can be added later.

## 3. Out of Scope (Initial Releases)
- Full motion matching / database search.
- Networked animation replication.
- Advanced cloth/soft-body simulation (beyond simple attachments).
- Editor / authoring tools (runtime + viewer only).
- Mobile / console specific optimizations.