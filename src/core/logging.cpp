#include <aga/core/logging.hpp>

#include <spdlog/logger.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

namespace {

std::shared_ptr<spdlog::logger> g_logger;
std::once_flag g_defaultLoggerOnce;

spdlog::level::level_enum toSpdlogLevel(aga::log::Level level) noexcept {
    switch (level) {
    case aga::log::Level::trace:
        return spdlog::level::trace;
    case aga::log::Level::debug:
        return spdlog::level::debug;
    case aga::log::Level::info:
        return spdlog::level::info;
    case aga::log::Level::warn:
        return spdlog::level::warn;
    case aga::log::Level::error:
        return spdlog::level::err;
    case aga::log::Level::critical:
        return spdlog::level::critical;
    case aga::log::Level::off:
        return spdlog::level::off;
    }

    return spdlog::level::info;
}

std::shared_ptr<spdlog::logger> makeLogger(const aga::log::Config& config) {
    std::vector<spdlog::sink_ptr> sinks;

    if (config.enableConsole) {
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    if (!config.filePath.empty()) {
        sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            config.filePath, config.maxFileSize, config.maxFiles));
    }

    if (sinks.empty()) {
        sinks.push_back(std::make_shared<spdlog::sinks::null_sink_mt>());
    }

    auto logger = std::make_shared<spdlog::logger>(
        "animforge", sinks.begin(), sinks.end());

    logger->set_level(toSpdlogLevel(config.level));
    logger->set_pattern(config.pattern);

    if (config.flushOnError) {
        logger->flush_on(spdlog::level::err);
    }

    return logger;
}

std::shared_ptr<spdlog::logger> getLogger() {
    std::call_once(g_defaultLoggerOnce, [] {
        aga::log::Config defaultConfig;
        std::atomic_store(&g_logger, makeLogger(defaultConfig));
    });

    return std::atomic_load(&g_logger);
}

} // namespace

namespace aga::log {

void initialize(const Config& config) {
    // Prevent lazy default initialization from overwriting this configuration.
    std::call_once(g_defaultLoggerOnce, [] {});

    // Construct first; publish only after successful construction.
    std::atomic_store(&g_logger, makeLogger(config));
}

void setLevel(Level level) {
    if (const auto logger = getLogger()) {
        logger->set_level(toSpdlogLevel(level));
    }
}

void flush() {
    if (const auto logger = getLogger()) {
        logger->flush();
    }
}

namespace detail {

void write(Level level,
           std::string_view message,
           const char* file,
           int line,
           const char* function) {
    const auto logger = getLogger();
    if (!logger) {
        return;
    }

    const spdlog::source_loc sourceLocation{file, line, function};
    logger->log(sourceLocation,
                toSpdlogLevel(level),
                "{}",
                spdlog::string_view_t{message.data(), message.size()});
}

} // namespace detail
} // namespace aga::log
