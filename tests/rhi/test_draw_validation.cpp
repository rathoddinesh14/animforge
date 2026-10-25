#include <catch2/catch_test_macros.hpp>

#include <aga/rhi/DrawValidation.hpp>

using aga::rhi::isValidVertexCount;
using aga::rhi::kVertexFloats;
using aga::rhi::PrimitiveTopology;

TEST_CASE("Vertex float stride is pos3 + color4", "[rhi][draw]") {
    REQUIRE(kVertexFloats == 7);
}

TEST_CASE("PointList accepts any positive count", "[rhi][draw]") {
    REQUIRE_FALSE(isValidVertexCount(PrimitiveTopology::PointList, 0));
    REQUIRE(isValidVertexCount(PrimitiveTopology::PointList, 1));
    REQUIRE(isValidVertexCount(PrimitiveTopology::PointList, 4));
}

TEST_CASE("LineList requires even vertex count", "[rhi][draw]") {
    REQUIRE_FALSE(isValidVertexCount(PrimitiveTopology::LineList, 0));
    REQUIRE_FALSE(isValidVertexCount(PrimitiveTopology::LineList, 1));
    REQUIRE(isValidVertexCount(PrimitiveTopology::LineList, 2));
    REQUIRE_FALSE(isValidVertexCount(PrimitiveTopology::LineList, 3));
    REQUIRE(isValidVertexCount(PrimitiveTopology::LineList, 4));
}

TEST_CASE("LineStrip requires at least two vertices", "[rhi][draw]") {
    REQUIRE_FALSE(isValidVertexCount(PrimitiveTopology::LineStrip, 1));
    REQUIRE(isValidVertexCount(PrimitiveTopology::LineStrip, 2));
    REQUIRE(isValidVertexCount(PrimitiveTopology::LineStrip, 5));
}

TEST_CASE("TriangleList requires multiple of three", "[rhi][draw]") {
    REQUIRE_FALSE(isValidVertexCount(PrimitiveTopology::TriangleList, 0));
    REQUIRE_FALSE(isValidVertexCount(PrimitiveTopology::TriangleList, 1));
    REQUIRE_FALSE(isValidVertexCount(PrimitiveTopology::TriangleList, 2));
    REQUIRE(isValidVertexCount(PrimitiveTopology::TriangleList, 3));
    REQUIRE_FALSE(isValidVertexCount(PrimitiveTopology::TriangleList, 4));
    REQUIRE(isValidVertexCount(PrimitiveTopology::TriangleList, 6));
}

TEST_CASE("TriangleStrip requires at least three vertices", "[rhi][draw]") {
    REQUIRE_FALSE(isValidVertexCount(PrimitiveTopology::TriangleStrip, 2));
    REQUIRE(isValidVertexCount(PrimitiveTopology::TriangleStrip, 3));
    REQUIRE(isValidVertexCount(PrimitiveTopology::TriangleStrip, 4));
}
