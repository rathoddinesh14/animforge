
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "common/Geometry.hpp"

#include <aga/rhi/DrawValidation.hpp>

using aga::rhi::kVertexFloats;
using aga::samples::lineGeometry;
using aga::samples::pointGeometry;
using aga::samples::triangleGeometry;

TEST_CASE("Point geometry contains one vertex", "[samples][geometry]") {
    const auto geometry = pointGeometry();

    REQUIRE(geometry.data != nullptr);
    REQUIRE(geometry.vertexCount == 1);
}

TEST_CASE("Line geometry contains two vertices", "[samples][geometry]") {
    const auto geometry = lineGeometry();

    REQUIRE(geometry.data != nullptr);
    REQUIRE(geometry.vertexCount == 2);
}

TEST_CASE("Triangle geometry contains three vertices", "[samples][geometry]") {
    const auto geometry = triangleGeometry();

    REQUIRE(geometry.data != nullptr);
    REQUIRE(geometry.vertexCount == 3);
}

TEST_CASE("Sample geometry uses seven floats per vertex", "[samples][geometry]") {
    const auto geometry = triangleGeometry();

    REQUIRE(geometry.data[0] == Catch::Approx(0.0f));
    REQUIRE(geometry.data[1] == Catch::Approx(0.5f));
    REQUIRE(geometry.data[3] == Catch::Approx(1.0f));
    REQUIRE(kVertexFloats * geometry.vertexCount == 21);
}
