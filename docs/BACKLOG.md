# Backlog / Phased Issues

Use GitHub Issues with labels: `phase-0`, `phase-1`, `phase-2`, … and `math`, `animation`, `rendering`, `vulkan`, `opengl`, `docs`, `ci`.

## Phase 0 – Foundation (Current)
- [x] Repository structure & documentation
- [ ] CMake + vcpkg skeleton builds on Windows
- [ ] Basic logging & assertion macros
- [ ] Math library (Vec3, Quat, Mat4, Transform) + unit tests
- [ ] MATH_CONVENTIONS.md finalized and enforced

## Phase 1 – Single Skeleton + Linear Blend Skinning
- [ ] glTF skeleton + mesh loader (minimal, or via tinygltf)
- [ ] Skeleton + AnimationClip data structures
- [ ] Hierarchical pose evaluation (local → model)
- [ ] CPU linear blend skinning (reference path)
- [ ] OpenGL GPU skinning (UBO or SSBO)
- [ ] Sample 01: Hello Skeleton (debug draw)
- [ ] Sample 02: CPU Skinning
- [ ] Sample 03: GPU Skinning (OpenGL)
- [ ] Root motion extraction (basic)

## Phase 2 – Blending & State Machines
- [ ] Cross-fade blender
- [ ] Additive layers
- [ ] Bone masks / partial body
- [ ] Simple state machine
- [ ] Lightweight blend tree
- [ ] Sample: Locomotion with transitions

## Phase 3 – Inverse Kinematics
- [ ] Two-bone analytic IK
- [ ] FABRIK solver
- [ ] CCD solver
- [ ] Joint constraints
- [ ] Sample: Foot IK / Look-at IK

## Phase 4 – Procedural & Secondary Motion
- [ ] Spring-damper systems
- [ ] Procedural look-at / aim
- [ ] Simple secondary motion on top of skeletal
- [ ] Sample: Cloth attachment / hair / tail

## Phase 5 – GPU-Driven & Advanced
- [ ] Compute shader skinning
- [ ] Animation texture / buffer sampling
- [ ] Vulkan skinning path complete
- [ ] Instanced crowd rendering
- [ ] Animation compression (research + implement)
- [ ] Performance budgets measured and documented

## Cross-Cutting (Ongoing)
- [ ] CI (Windows MSVC first)
- [ ] Clang-format + warning-as-error
- [ ] Debug visualization improvements
- [ ] Documentation updates per phase
- [ ] Linux / macOS port (after Phase 2)

## Nice-to-Have / Research
- Dual quaternion skinning
- Motion matching foundations
- Full-body IK
- Multi-threaded pose evaluation
- Networked animation considerations

---

When creating GitHub Issues, copy the relevant checklist items and expand them with acceptance criteria.