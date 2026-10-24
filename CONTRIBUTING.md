# Contributing

This project is both a learning vehicle and a production-quality foundation. Contributions should advance understanding as much as functionality.

## Workflow

1. Open an issue describing the technique or fix (use the templates).
2. Work on a feature branch.
3. Keep commits focused and well-described.
4. Ensure the CPU reference path still works when adding a GPU path.
5. Update the relevant documentation (especially DESIGN_DECISIONS.md and BACKLOG.md).
6. Open a pull request against `main`.

## Code Standards

- C++20 minimum.
- Follow `.clang-format`.
- Prefer clear, explicit code over clever abstractions.
- Hot paths should be data-oriented.
- No silent failures in math or pose evaluation; use assertions in debug.

## Testing

- Unit tests for all new math and evaluation logic.
- Visual validation via the sample viewer when the change affects rendering or skinning.

## Documentation

Every non-trivial design choice belongs in `docs/DESIGN_DECISIONS.md`.