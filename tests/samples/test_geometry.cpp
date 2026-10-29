#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "common/Geometry.hpp"

#include <aga/rhi/DrawValidation.hpp>

using aga::rhi::isValidVertexCount;
using aga::rhi::kVertexFloats;
using aga::rhi::PrimitiveTopology;
using aga::samples::lineGeometry;
using aga::samples::pointGeometry;
using aga::samples::triangleGeometry;

TEST_CASE("Point geometry matches PointList rules", "[samples][geometry]") {
    auto g = pointGeometry();
    REQUIRE(g.data != nullptr);
    REQUIRE(g.vertexCount == 1);
    REQUIRE(isValidVertexCount(PrimitiveTopology::PointList, g.vertexCount));
}

TEST_CASE("Line geometry matches LineList rules", "[samples][geometry]") {
    auto g = lineGeometry();
    REQUIRE(g.data != nullptr);
    REQUIRE(g.vertexCount == 2);
    REQUIRE(isValidVertexCount(PrimitiveTopology::LineList, g.vertexCount));
}

TEST_CASE("Triangle geometry matches TriangleList rules", "[samples][geometry]") {
    auto g = triangleGeometry();
    REQUIRE(g.data != nullptr);
    REQUIRE(g.vertexCount == 3);
    REQUIRE(isValidVertexCount(PrimitiveTopology::TriangleList, g.vertexCount));
}

TEST_CASE("Sample vertices store seven floats per vertex", "[samples][geometry]") {
    auto tri = triangleGeometry();
    // First vertex of triangle: top, red-ish
    REQUIRE(tri.data[0] == Catch::Approx(0.0f));
    REQUIRE(tri.data[1] == Catch::Approx(0.5f));
    REQUIRE(tri.data[3] == Catch::Approx(1.0f)); // r
    REQUIRE(kVertexFloats * tri.vertexCount == 21);
}
