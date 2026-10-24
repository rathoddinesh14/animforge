# Contributing

This project is both a learning vehicle and a production-quality foundation. Contributions should advance understanding as much as functionality.

## Workflow

1. Open an issue describing the technique or fix (use the templates).
2. Work on a feature branch.
3. **Follow TDD** for all core math and animation code (see `docs/TESTING.md`).
4. Keep commits focused and well-described.
5. Ensure the CPU reference path still works when adding a GPU path.
6. Update the relevant documentation (especially `DESIGN_DECISIONS.md` and `BACKLOG.md`).
7. Open a pull request against `main`.

## Code Standards

- C++20 minimum.
- Follow `.clang-format`.
- Prefer clear, explicit code over clever abstractions.
- Hot paths should be data-oriented.
- No silent failures in math or pose evaluation; use assertions in debug.

## Testing

- **Catch2 v3** is the required testing framework.
- TDD is mandatory for core modules (math, pose evaluation, blending, IK, skinning).
- Write the failing test first, implement, then refactor.
- Visual validation via the sample viewer is required for any change that affects skinning or rendering.

See `docs/TESTING.md` for full details.

## Documentation

Every non-trivial design choice belongs in `docs/DESIGN_DECISIONS.md`.