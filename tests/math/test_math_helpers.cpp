#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <aga/math/math.hpp>

#include <glm/gtc/constants.hpp>

using namespace aga::math;
using Catch::Approx;

TEST_CASE("Transform defaults to identity", "[math][transform]") {
    const Transform t{};

    REQUIRE(t.translation.x == Approx(0.0f));
    REQUIRE(t.translation.y == Approx(0.0f));
    REQUIRE(t.translation.z == Approx(0.0f));

    REQUIRE(t.rotation.w == Approx(1.0f));
    REQUIRE(t.rotation.x == Approx(0.0f));
    REQUIRE(t.rotation.y == Approx(0.0f));
    REQUIRE(t.rotation.z == Approx(0.0f));

    REQUIRE(t.scale.x == Approx(1.0f));
    REQUIRE(t.scale.y == Approx(1.0f));
    REQUIRE(t.scale.z == Approx(1.0f));

    const Mat4 matrix = t.to_mat4();
    for(int col = 0; col < 4; ++col) {
        for(int row = 0; row < 4; ++row) {
            REQUIRE(matrix[col][row] == Approx(col == row ? 1.0f : 0.0f));
        }
    }
}

TEST_CASE("Transform applies scale before translation", "[math][transform]") {
    Transform t{};
    t.translation = Vec3(3.0f, -2.0f, 5.0f);
    t.scale = Vec3(1.0f, 2.0f, 3.0f);

    const Vec4 p = t.to_mat4() * Vec4(1.0f);

    REQUIRE(p.x == Approx(4.0f));
    REQUIRE(p.y == Approx(0.0f));
    REQUIRE(p.z == Approx(8.0f));
    REQUIRE(p.w == Approx(1.0f));
}

TEST_CASE("transform identity factory returns identity", "[math][transform]") {
    const Transform t = Transform::identity();

    REQUIRE(t.translation.x == Approx(0.0f));
    REQUIRE(t.translation.y == Approx(0.0f));
    REQUIRE(t.translation.z == Approx(0.0f));

    REQUIRE(t.rotation.w == Approx(1.0f));
    REQUIRE(t.rotation.x == Approx(0.0f));
    REQUIRE(t.rotation.y == Approx(0.0f));
    REQUIRE(t.rotation.z == Approx(0.0f));

    REQUIRE(t.scale.x == Approx(1.0f));
    REQUIRE(t.scale.y == Approx(1.0f));
    REQUIRE(t.scale.z == Approx(1.0f));

    const Mat4 matrix = t.to_mat4();
    for(int col = 0; col < 4; ++col) {
        for(int row = 0; row < 4; ++row) {
            REQUIRE(matrix[col][row] == Approx(col == row ? 1.0f : 0.0f));
        }
    }
}

TEST_CASE("Translate helper moves a point", "[math][helpers]") {
    const Vec4 point{1.0f, 0.0f, 0.0f, 1.0f};
    const Vec4 result = translate(Vec3(2.0f, 3.0f, 4.0f)) * point;

    REQUIRE(result.x == Approx(3.0f));
    REQUIRE(result.y == Approx(3.0f));
    REQUIRE(result.z == Approx(4.0f));
    REQUIRE(result.w == Approx(1.0f));
}

TEST_CASE("Scale helper scales a point", "[math][helpers]") {
    const Vec4 point{1.0f, 2.0f, 3.0f, 1.0f};
    const Vec4 result = scale(Vec3(2.0f, 3.0f, 4.0f)) * point;

    REQUIRE(result.x == Approx(2.0f));
    REQUIRE(result.y == Approx(6.0f));
    REQUIRE(result.z == Approx(12.0f));
    REQUIRE(result.w == Approx(1.0f));
}

TEST_CASE("rotate helper rotates around Z axis", "[math][helpers]") {
    const Vec4 point{1.0f, 0.0f, 0.0f, 1.0f};
    const Vec4 result = rotate(glm::half_pi<float>(), UNIT_Z) * point;

    REQUIRE(result.x == Approx(0.0f).margin(0.0001f));
    REQUIRE(result.y == Approx(1.0f).margin(0.0001f));
    REQUIRE(result.z == Approx(0.0f).margin(0.0001f));
    REQUIRE(result.w == Approx(1.0f).margin(0.0001f));
}

TEST_CASE("angle_axis rotates aroaund the requested axis", "[math][quat]") {
    const Quat q = angle_axis(glm::half_pi<float>(), UNIT_Z);
    const Vec3 result = q * UNIT_X;

    REQUIRE(glm::length(q) == Approx(1.0f).margin(0.0001f));
    REQUIRE(result.x == Approx(0.0f).margin(0.0001f));
    REQUIRE(result.y == Approx(1.0f).margin(0.0001f));
    REQUIRE(result.z == Approx(0.0f).margin(0.0001f));
}
