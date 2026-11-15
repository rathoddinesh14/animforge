# Integration notes for the spdlog backend

Apply these changes to the current `main` branch. This bundle contains the
new/changed files; it is not a complete clone of the repository.

## 1. `cmake/Dependencies.cmake`

Add this section after the GLM section (before Catch2):

```cmake
# ---------------------------------------------------------------------------
# spdlog (core logging backend)
# ---------------------------------------------------------------------------
find_package(spdlog CONFIG QUIET)
if(NOT spdlog_FOUND)
    message(STATUS "spdlog not found via find_package - fetching with FetchContent")
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.15.3
        GIT_SHALLOW    TRUE
    )
    set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
    set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(spdlog)
endif()
```

## 2. Root `CMakeLists.txt`

Add the backend source to `aga_core`:

```cmake
target_sources(aga_core PRIVATE
    src/math/math_dummy.cpp
    src/core/logging.cpp
)
```

Add spdlog to `aga_core`'s public link dependencies because the public logging
header uses `fmt::format_string` and consumers need the transitive formatting
headers:

```cmake
target_link_libraries(aga_core
    PUBLIC
        glm::glm
        spdlog::spdlog
)
```

## 3. `tests/CMakeLists.txt`

Add the test source to `aga_tests`:

```cmake
add_executable(aga_tests
    math/test_glm_integration.cpp
    math/test_math_helpers.cpp
    samples/test_geometry.cpp
    core/test_logging.cpp
)
```

## 4. Documentation

- Add `docs/LOGGING.md`.
- Add a Logging section to `docs/ARCHITECTURE.md` under `aga::core`.
- Add a design decision stating that AnimForge owns the stable logging API
  while spdlog is the replaceable backend.
- Update the Phase 0 logging backlog item to say "Backend-neutral logging API
  with spdlog implementation". Do not claim custom assertion macros are done;
  assertion policy remains the standard `assert`/future project assertion
  decision.

## 5. Deliberately not included

- No custom logger implementation, queue, formatter, sink, or file rotation.
- No public spdlog include or spdlog type in AnimForge headers.
- No assertion framework or custom assertion macros.
- No direct spdlog calls in samples, tests, or animation modules.
