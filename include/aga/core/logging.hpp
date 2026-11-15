#pragma once

#include <fmt/format.h>

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

namespace aga::log {

/**
 * Severity levels supported by AnimForge's logging API.
 *
 * This enum is intentionally independent of any logging backend.
 */
enum class Level { trace, debug, info, warn, error, critical, off };

/**
 * Backend-neutral configuration for the process-wide AnimForge logger.
 *
 * The default configuration writes colored messages to the console.
 * Set filePath to a non-empty path to additionally enable a rotating file.
 */
struct Config {
    Level level{Level::info};
    bool enableConsole{true};
    std::string filePath{};
    std::size_t maxFileSize{5U * 1024U * 1024U};
    std::size_t maxFiles{3U};
    bool flushOnError{true};
    std::string pattern{"[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v"};
};

/**
 * Initialize or reconfigure the logging backend.
 *
 * Call during application startup, before starting worker threads.
 * If this function is not called, the first log message lazily initializes
 * the default console logger.
 */
void initialize(const Config& config = {});

/** Change the minimum severity emitted by the active backend. */
void setLevel(Level level);

/** Flush all buffered log messages. */
void flush();

namespace detail {

/**
 * Backend boundary. Application code must use the AGA_LOG_* macros instead.
 */
void write(Level level, std::string_view message, const char* file, int line, const char* function);

} // namespace detail

/**
 * Format and submit a log message.
 *
 * fmt is used only as the stable formatting contract at this boundary;
 * no spdlog type or header is exposed to AnimForge source files.
 */
template <typename... Args>
void log(Level level, const char* file, int line, const char* function,
         fmt::format_string<Args...> format, Args&&... args) {
    detail::write(level, fmt::format(format, std::forward<Args>(args)...), file, line, function);
}

} // namespace aga::log

#define AGA_LOG_TRACE(...)                                                                         \
    ::aga::log::log(::aga::log::Level::trace, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define AGA_LOG_DEBUG(...)                                                                         \
    ::aga::log::log(::aga::log::Level::debug, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define AGA_LOG_INFO(...)                                                                          \
    ::aga::log::log(::aga::log::Level::info, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define AGA_LOG_WARN(...)                                                                          \
    ::aga::log::log(::aga::log::Level::warn, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define AGA_LOG_ERROR(...)                                                                         \
    ::aga::log::log(::aga::log::Level::error, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define AGA_LOG_CRITICAL(...)                                                                      \
    ::aga::log::log(::aga::log::Level::critical, __FILE__, __LINE__, __func__, __VA_ARGS__)
