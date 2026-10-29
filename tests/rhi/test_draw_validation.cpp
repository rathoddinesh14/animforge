
#include <catch2/catch_test_macros.hpp>

#include <aga/rhi/DrawValidation.hpp>

TEST_CASE("Vertex layout uses position3 and color4", "[rhi][draw]") {
    REQUIRE(aga::rhi::kVertexFloats == 7);
}
