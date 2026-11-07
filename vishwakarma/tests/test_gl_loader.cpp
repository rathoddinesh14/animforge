#include <catch2/catch_test_macros.hpp>

#include "GlLoader.hpp"

#include <stdexcept>
#include <string>

namespace {

void (*missingFunction(const char*))() {
    return nullptr;
}

} // namespace

TEST_CASE("OpenGL loader reports every missing required entry point", "[opengl][loader]") {
    std::string message;
    try {
        static_cast<void>(loadGl(missingFunction));
    } catch (const std::runtime_error& error) {
        message = error.what();
    }

    REQUIRE(message.find("glClearColor") != std::string::npos);
    REQUIRE(message.find("glCreateProgram") != std::string::npos);
    REQUIRE(message.find("glUseProgram") != std::string::npos);
}

TEST_CASE("OpenGL loader rejects a null resolver", "[opengl][loader]") {
    REQUIRE_THROWS_AS(loadGl(nullptr), std::invalid_argument);
}
