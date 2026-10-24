# AnimForge

**Modern C++20/23 real-time animation systems with OpenGL and Vulkan.**

A learning- and production-oriented codebase focused on deep understanding and independent mastery of advanced game animation techniques.

## Goals

- Master skeletal animation, blending, inverse kinematics, procedural secondary motion, and GPU-driven animation.
- Build correct, debuggable, and performant systems before optimizing.
- Support both OpenGL (rapid experimentation) and Vulkan (explicit control).
- Windows-first (MSVC), with clean architecture for future Linux/macOS support.
- Prefer clarity, RAII, type safety, and data-oriented design over heavy abstraction.

## Current Status

**Phase 0 – Project Foundation** (this commit)

- Requirements, architecture, design decisions, backlog, and API outline documented.
- CMake + vcpkg skeleton ready for Windows.
- Directory structure and coding standards established.

Next: Phase 1 – Single skeleton + Linear Blend Skinning (CPU → GPU).

## Quick Start (Windows)

```bash
# Prerequisites: Visual Studio 2022, CMake ≥ 3.25, Git, vcpkg
git clone https://github.com/rathoddinesh14/animforge.git
cd animforge

# Bootstrap vcpkg (or use system-wide)
git clone https://github.com/microsoft/vcpkg.git external/vcpkg
./external/vcpkg/bootstrap-vcpkg.bat

# Configure & build (example with Visual Studio generator)
cmake --preset windows-msvc
cmake --build --preset windows-msvc-debug
```

## Documentation

| Document | Purpose |
|----------|---------|
| [docs/REQUIREMENTS.md](docs/REQUIREMENTS.md) | Functional & non-functional requirements |
| [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) | High-level system architecture |
| [docs/DESIGN_DECISIONS.md](docs/DESIGN_DECISIONS.md) | Key technical choices and rationale |
| [docs/API.md](docs/API.md) | Public API overview |
| [docs/BACKLOG.md](docs/BACKLOG.md) | Phased backlog / issues |
| [docs/MATH_CONVENTIONS.md](docs/MATH_CONVENTIONS.md) | Coordinate system, matrix layout, quaternion conventions |
| [docs/PERFORMANCE_BUDGETS.md](docs/PERFORMANCE_BUDGETS.md) | Target frame budgets and memory |
| [docs/ASSET_PIPELINE.md](docs/ASSET_PIPELINE.md) | Expected asset formats and conversion |
| [docs/TESTING.md](docs/TESTING.md) | TDD workflow and Catch2 usage |
| [docs/CODING_STANDARDS.md](docs/CODING_STANDARDS.md) | Naming, style, documentation, ownership |

## License

MIT License – see [LICENSE](LICENSE).

## Mentorship Philosophy

This repository is deliberately built as a teaching and mastery vehicle:

1. **Understand** the mathematics and algorithms.
2. **Implement** correctly and independently.
3. **Engineer** for edge cases, performance, and integration.

Working code alone does not equal mastery.