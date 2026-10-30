#include <catch2/catch_test_macros.hpp>

#include <aga/rhi/Types.hpp>

TEST_CASE("RHI backend enum values are stable", "[rhi]") {
    using aga::rhi::Backend;
    REQUIRE(static_cast<int>(Backend::OpenGL) == 0);
    REQUIRE(static_cast<int>(Backend::Vulkan) == 1);
}
