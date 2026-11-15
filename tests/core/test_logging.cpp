#include <aga/core/logging.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("logging API supports initialization and formatted messages") {
    aga::log::Config config;
    config.enableConsole = false;
    config.level = aga::log::Level::trace;

    REQUIRE_NOTHROW(aga::log::initialize(config));
    REQUIRE_NOTHROW(AGA_LOG_TRACE("trace {}", 1));
    REQUIRE_NOTHROW(AGA_LOG_DEBUG("debug {}", 2));
    REQUIRE_NOTHROW(AGA_LOG_INFO("info {}", 3));
    REQUIRE_NOTHROW(AGA_LOG_WARN("warning {}", 4));
    REQUIRE_NOTHROW(AGA_LOG_ERROR("error {}", 5));
    REQUIRE_NOTHROW(AGA_LOG_CRITICAL("critical {}", 6));
    REQUIRE_NOTHROW(aga::log::flush());
}
