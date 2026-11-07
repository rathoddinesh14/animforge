
#include <catch2/catch_test_macros.hpp>

#include <vishwakarma/graphics/DrawValidation.hpp>

TEST_CASE("Vertex layout uses position3 and color4", "[graphics][draw]") {
    REQUIRE(vishwakarma::graphics::kVertexFloats == 7);
}
