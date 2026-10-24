#include <catch2/catch_test_macros.hpp>

#include <aga/rhi/Types.hpp>

TEST_CASE("RHI backend enum values are stable", "[rhi]") {
    using aga::rhi::Backend;
    REQUIRE(static_cast<int>(Backend::OpenGL) == 0);
    REQUIRE(static_cast<int>(Backend::Vulkan) == 1);
}

TEST_CASE("RHI topology enum covers debug primitives", "[rhi]") {
    using aga::rhi::PrimitiveTopology;
    REQUIRE(static_cast<int>(PrimitiveTopology::PointList) == 0);
    REQUIRE(static_cast<int>(PrimitiveTopology::LineList) == 1);
    REQUIRE(static_cast<int>(PrimitiveTopology::TriangleList) == 3);
}
