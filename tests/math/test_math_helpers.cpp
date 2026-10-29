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
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
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
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
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

TEST_CASE("Vec3 lerp interpolates components", "[math][interpolation]") {
    const Vec3 a{0.0f, 2.0f, 4.0f};
    const Vec3 b{10.0f, 12.0f, 14.0f};

    const Vec3 midpoint = lerp(a, b, 0.5f);

    REQUIRE(midpoint.x == Approx(5.0f));
    REQUIRE(midpoint.y == Approx(7.0f));
    REQUIRE(midpoint.z == Approx(9.0f));
}

TEST_CASE("Transform lerp interpolates translation and scale", "[math][interpolation]") {
    Transform a;
    a.translation = Vec3{0.0f, 2.0f, 4.0f};
    a.scale = Vec3{1.0f, 2.0f, 3.0f};

    Transform b;
    b.translation = Vec3{10.0f, 12.0f, 14.0f};
    b.scale = Vec3{3.0f, 4.0f, 5.0f};

    const Transform start = lerp(a, b, 0.0f);
    const Transform midpoint = lerp(a, b, 0.5f);
    const Transform finish = lerp(a, b, 1.0f);

    REQUIRE(start.translation.x == Approx(a.translation.x));
    REQUIRE(start.translation.y == Approx(a.translation.y));
    REQUIRE(start.translation.z == Approx(a.translation.z));

    REQUIRE(midpoint.translation.x == Approx(5.0f));
    REQUIRE(midpoint.translation.y == Approx(7.0f));
    REQUIRE(midpoint.translation.z == Approx(9.0f));

    REQUIRE(midpoint.scale.x == Approx(2.0f));
    REQUIRE(midpoint.scale.y == Approx(3.0f));
    REQUIRE(midpoint.scale.z == Approx(4.0f));

    REQUIRE(finish.translation.x == Approx(b.translation.x));
    REQUIRE(finish.translation.y == Approx(b.translation.y));
    REQUIRE(finish.translation.z == Approx(b.translation.z));

    REQUIRE(finish.scale.x == Approx(b.scale.x));
    REQUIRE(finish.scale.y == Approx(b.scale.y));
    REQUIRE(finish.scale.z == Approx(b.scale.z));
}

TEST_CASE("Quaternion slerp preserves endpoints and unit length", "[math][interpolation][quat]") {
    const Quat identity = IDENTITY_QUAT;
    const Quat half_turn = angle_axis(glm::pi<float>(), UNIT_Y);

    const Quat start = slerp(identity, half_turn, 0.0f);
    const Quat finish = slerp(identity, half_turn, 1.0f);
    const Quat midpoint = slerp(identity, half_turn, 0.5f);

    REQUIRE(glm::abs(glm::dot(start, identity)) == Approx(1.0f).margin(0.0001f));
    REQUIRE(glm::abs(glm::dot(finish, half_turn)) == Approx(1.0f).margin(0.0001f));
    REQUIRE(glm::length(midpoint) == Approx(1.0f).margin(0.0001f));
}

TEST_CASE("Quaternion slerp interpolates Z-axis rotations at t=0.5",
          "[math][interpolation][quat]") {
    const Quat q0 = angle_axis(glm::radians(0.0f), UNIT_Z);
    const Quat q90 = angle_axis(glm::radians(90.0f), UNIT_Z);
    const Quat q180 = angle_axis(glm::radians(180.0f), UNIT_Z);

    SECTION("0 degrees to 90 degrees produces 45 degrees") {
        const Quat result = slerp(q0, q90, 0.5f);
        const Vec3 rotated = result * UNIT_X;
        const float expected = glm::radians(45.0f);

        REQUIRE(rotated.x == Approx(glm::cos(expected)).margin(0.0001f));
        REQUIRE(rotated.y == Approx(glm::sin(expected)).margin(0.0001f));
        REQUIRE(rotated.z == Approx(0.0f).margin(0.0001f));
        REQUIRE(glm::length(result) == Approx(1.0f).margin(0.0001f));
    }

    SECTION("90 degrees to 180 degrees produces 135 degrees") {
        const Quat result = slerp(q90, q180, 0.5f);
        const Vec3 rotated = result * UNIT_X;
        const float expected = glm::radians(135.0f);

        REQUIRE(rotated.x == Approx(glm::cos(expected)).margin(0.0001f));
        REQUIRE(rotated.y == Approx(glm::sin(expected)).margin(0.0001f));
        REQUIRE(rotated.z == Approx(0.0f).margin(0.0001f));
        REQUIRE(glm::length(result) == Approx(1.0f).margin(0.0001f));
    }

    SECTION("0 degrees to 180 degrees produces a 90-degree midpoint") {
        const Quat result = slerp(q0, q180, 0.5f);
        const Vec3 rotated = result * UNIT_X;

        // q and -q represent the same endpoint, so either midpoint sign is valid.
        REQUIRE(rotated.x == Approx(0.0f).margin(0.0001f));
        REQUIRE(glm::abs(rotated.y) == Approx(1.0f).margin(0.0001f));
        REQUIRE(rotated.z == Approx(0.0f).margin(0.0001f));
        REQUIRE(glm::length(result) == Approx(1.0f).margin(0.0001f));
    }
}
