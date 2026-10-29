# Testing Strategy – AnimForge

## Guiding Principle

**Test-Driven Development (TDD) is the default workflow for all core animation and math code.**

We write the test first, watch it fail, implement the minimal correct solution, then refactor.  
Working code without tests is not considered complete.

## Framework

**Catch2 v3** is the primary testing framework.

Reasons (see also `DESIGN_DECISIONS.md`):
- Highly readable, low-boilerplate tests
- Excellent support for expressing mathematical properties and edge cases
- BDD-style sections when they improve clarity
- Fast feedback loop ideal for TDD

## What We Test

| Layer                        | Test Type          | Priority |
|-----------------------------|--------------------|----------|
| Math (Vec3, Quat, Mat4, Transform, interpolation) | Unit (Catch2)     | Critical |
| Pose evaluation (local → model) | Unit              | Critical |
| Blending & layering         | Unit               | High     |
| IK solvers                  | Unit + property    | High     |
| Skinning (CPU reference)    | Unit               | Critical |
| Animation clip sampling     | Unit               | High     |
| Root motion extraction      | Unit               | Medium   |
| OpenGL / Vulkan backends    | Integration + visual | Medium |
| Full samples                | Visual inspection  | Required |

## TDD Cadence for a New Technique

1. **Write the specification as a test**  
   Example: “Given a two-bone chain and a reachable target, FABRIK converges to within ε of the target in ≤ N iterations.”

2. **Run → observe failure** (red).

3. **Implement the minimal correct version**.

4. **Run → green**.

5. **Refactor** (naming, structure, performance) while staying green.

6. Only then move to the next behaviour or edge case.

## Directory Layout

```
tests/
├── CMakeLists.txt
├── math/
│   ├── test_vec3.cpp
│   ├── test_quat.cpp
│   ├── test_mat4.cpp
│   └── test_transform.cpp
├── animation/
│   ├── test_pose.cpp
│   ├── test_clip_sampling.cpp
│   ├── test_blending.cpp
│   └── test_ik_fabrik.cpp
└── ...
```

## Running Tests

```bash
cmake --preset windows-msvc
cmake --build --preset windows-msvc-debug
ctest --test-dir build/windows-msvc -C Debug --output-on-failure
```

Or run the test executable directly for more detailed Catch2 output.

## Style Guidelines

- Prefer `REQUIRE` for conditions that must hold (test aborts on failure).
- Use `CHECK` when you want to continue and collect multiple failures.
- Use `Approx` (or Catch2’s floating-point matchers) for all floating-point comparisons. Never use raw `==` on floats.
- Name tests after the behaviour or mathematical property, not the function name.
- One logical behaviour per `TEST_CASE` (or `SCENARIO`).

## Visual Validation

Unit tests cannot catch volume loss, candy-wrapper artefacts, or incorrect bone weights visually.  
Every skinning and IK change must also be validated in the sample viewer.

## Current unit test coverage (non-GPU)

| Area | File |
|------|------|
| Math / GLM conventions | `tests/math/test_glm_integration.cpp` |
| RHI enums | `tests/rhi/test_rhi_types.cpp` |
| Draw topology validation | `tests/rhi/test_draw_validation.cpp` |
| Sample geometry | `tests/samples/test_geometry.cpp` |

GPU samples are not run in CI (no display). They must build when `AGA_BUILD_SAMPLES=ON`.
