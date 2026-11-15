# Logging

AnimForge source files must use the project-owned logging API in
`<aga/core/logging.hpp>`. They must not include spdlog headers or call spdlog
directly.

## Usage

```cpp
#include <aga/core/logging.hpp>

void loadAnimation(const std::string& path) {
    AGA_LOG_INFO("Loading animation: {}", path);

    if (path.empty()) {
        AGA_LOG_ERROR("Animation path is empty");
        return;
    }

    AGA_LOG_DEBUG("Animation path validated: {}", path);
}
```

Available macros: `AGA_LOG_TRACE`, `AGA_LOG_DEBUG`, `AGA_LOG_INFO`,
`AGA_LOG_WARN`, `AGA_LOG_ERROR`, and `AGA_LOG_CRITICAL`.

## Application startup

The default configuration writes colored messages to the console. Configure
the logger once during startup, before starting worker threads:

```cpp
aga::log::Config logging;
logging.level = aga::log::Level::debug;
logging.enableConsole = true;
logging.filePath = "logs/animforge.log";
logging.maxFileSize = 5U * 1024U * 1024U;
logging.maxFiles = 3U;

aga::log::initialize(logging);
```

If initialization is omitted, the first log call lazily initializes the
default console logger. `aga::log::setLevel()` changes the runtime threshold;
`aga::log::flush()` flushes pending messages.

## Backend boundary

- Application and animation code depend on `aga::log` and `AGA_LOG_*` only.
- `src/core/logging.cpp` is the only file that includes spdlog headers.
- The public API exposes no spdlog classes, sinks, logger pointers, or enums.
- `fmt` is the formatting contract at the API boundary. It is not a logger
  abstraction and keeps compile-time checked `{}` formatting available.
- To replace spdlog later, replace the implementation in
  `src/core/logging.cpp` and update the dependency in
  `cmake/Dependencies.cmake`. Existing call sites remain unchanged.

## Design constraints

Do not add convenience wrappers around individual spdlog sinks or expose
backend-specific configuration in public headers. Add a project-level option
only when it represents a stable AnimForge requirement rather than a spdlog
feature.
