# Coding Standards — AnimForge

This document defines how we write and document C++ code in this project.
It is adapted from the standards used in ImageRegistration and tailored for
real-time animation systems.

## 1. General principles

- Prefer clarity over cleverness.
- Keep public headers minimal and self-contained.
- Data structures and pure interfaces first; algorithms second.
- Core library stays free of windowing, full engine frameworks, and heavy I/O.
- Correctness and debuggability before premature optimization.

## 2. Naming conventions

| Item | Convention | Example |
|------|------------|---------|
| Types (classes, structs, enums) | `PascalCase` | `Skeleton`, `AnimationClip`, `Pose` |
| Functions / methods | `camelCase` | `evaluatePose()`, `sampleClip()` |
| Member variables | `m_` prefix | `m_joints`, `m_duration` |
| Local variables | `camelCase` | `localTransform`, `boneIndex` |
| Constants / enum enumerators | `kCamelCase` or `PascalCase` | `kMaxInfluences`, `LoopMode::Once` |
| Namespaces | `aga` (and focused sub-namespaces) | `aga::math`, `aga::animation` |
| Files | Match primary type; snake_case for tests | `Skeleton.hpp`, `test_pose.cpp` |
| Macros | `AGA_UPPER_SNAKE` | `AGA_ASSERT` |

### File naming rules

- Public headers: `include/aga/<module>/<TypeName>.hpp`
- Implementation: `src/<module>/<TypeName>.cpp` (or grouped files)
- Tests: `tests/<module>/test_<topic>.cpp`
- Prefer `.hpp` for C++ headers (not `.h`).

## 3. Headers and includes

- Use `#pragma once`.
- Include order (enforced by clang-format IncludeCategories):
  1. Related / project headers (`aga/...`)
  2. Third-party (`glm/...`, `catch2/...`)
  3. Standard library (`<vector>`, `<span>`, …)
- Prefer forward declarations in headers when sufficient.
- Do not put heavy implementation details in public headers unless required for templates/inline.

## 4. Documentation (Doxygen-style)

Public API **must** be documented.

Prefer block comments `/** ... */` for multi-line documentation.
Single-line `///` is acceptable for very short notes.

Document:

- Every public class / struct (responsibility, ownership)
- Every public function / method:
  - Brief purpose
  - `@param` for each parameter (meaning, units, constraints)
  - `@return` when not `void`
  - Pre/post-conditions or error behavior when relevant

### Example

```cpp
#pragma once

#include <aga/math/math.hpp>
#include <span>
#include <string>
#include <vector>

namespace aga::animation {

/**
 * Hierarchical skeleton used for skinning and pose evaluation.
 *
 * Owns joint data. Inverse-bind matrices are stored in model space.
 */
class Skeleton {
public:
    /**
     * @return Number of joints in the hierarchy.
     */
    [[nodiscard]] std::size_t jointCount() const noexcept;

    /**
     * Find a joint by name.
     * @param name Joint name (case-sensitive).
     * @return Index of the joint, or -1 if not found.
     */
    [[nodiscard]] int findJoint(std::string_view name) const;

private:
    std::vector<Joint> m_joints;
};

} // namespace aga::animation
```

## 5. Modern C++ (C++20)

- Mark functions `[[nodiscard]]` when ignoring the result is likely a bug.
- Prefer strong **const-correctness**.
- Use `noexcept` where the function genuinely does not throw.
- Prefer `enum class` over plain enums.
- Use `override` / `final` explicitly on virtual methods.
- Prefer value semantics; use `std::unique_ptr` for exclusive ownership; avoid raw owning pointers.
- Prefer `nullptr` over `NULL` or `0`.
- Prefer `std::span` for non-owning contiguous ranges.
- Avoid magic numbers; use named constants.
- Keep functions small and focused.
- Hot paths should favour contiguous data and clear ownership.

## 6. Error handling

| Kind of failure | Mechanism | Examples |
|-----------------|-----------|----------|
| Precondition / internal bug | `assert` / `AGA_ASSERT` in debug | Invalid joint index, empty skeleton |
| Recoverable operational failure | Result / status object or `std::optional` | Missing animation channel, unreachable IK target |
| Rare / boundary failure | C++ exceptions (documented) | Out of memory, asset load failure |

Rules:

- Do **not** use exceptions for normal control flow.
- Prefer `[[nodiscard]]` on functions that return status or optional results.
- Document preconditions clearly.

## 7. Ownership and lifetime

- Document who owns memory and how long references/pointers remain valid.
- Prefer returning by value for small types (`Vec3`, `Quat`, `Transform`).
- For non-owning views (`std::span`, raw pointers), document lifetime requirements at the interface.

## 8. Testing (TDD)

- Every new public type or non-trivial function must have Catch2 tests.
- Follow the TDD cycle: failing test → minimal implementation → refactor.
- Test names use ASCII only (no Unicode punctuation) so CTest filters work on Windows.
- Prefer focused unit tests; visual validation via samples for skinning/IK artefacts.
- See `docs/TESTING.md`.

## 9. Formatting & static analysis

- All code must match `.clang-format`.
- CI enforces formatting; format before pushing:
  ```bash
  clang-format -i $(git ls-files '*.cpp' '*.hpp')
  ```
- `.clang-tidy` is used in CI (pragmatic set of checks). Tighten over time.

## 10. Pull request expectations

See `.github/PULL_REQUEST_TEMPLATE.md`. At minimum:

- [ ] Public API documented (params + return)
- [ ] Tests added or updated (TDD)
- [ ] Follows these coding standards
- [ ] CI is green
- [ ] Design decisions updated when a non-trivial choice is made