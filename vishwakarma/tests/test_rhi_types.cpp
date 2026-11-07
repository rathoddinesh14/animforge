#include <catch2/catch_test_macros.hpp>

#include <vishwakarma/graphics/Types.hpp>

TEST_CASE("Graphics backend enum values are stable", "[graphics]") {
    using vishwakarma::graphics::Backend;
    REQUIRE(static_cast<int>(Backend::OpenGL) == 0);
    REQUIRE(static_cast<int>(Backend::Vulkan) == 1);
}
