#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <aryabhatta/math/math.hpp>

#include <type_traits>

namespace math = aryabhatta::math;
using Catch::Approx;

TEST_CASE("AnimForge consumes Aryabhatta public float API", "[math][aryabhatta]") {
    static_assert(std::is_same_v<math::Vec3, math::BasicVec3<float>>);
    static_assert(std::is_same_v<math::Transform, math::BasicTransform<float>>);

    const auto rotation = math::angle_axis(1.57079632679f, math::UNIT_Z);
    const math::Vec3 rotated = rotation * math::UNIT_X;

    REQUIRE(rotated.x == Approx(0.0f).margin(0.0001f));
    REQUIRE(rotated.y == Approx(1.0f).margin(0.0001f));
    REQUIRE(rotated.z == Approx(0.0f).margin(0.0001f));
}

TEST_CASE("Aryabhatta transform composes translation rotation and scale",
          "[math][aryabhatta][transform]") {
    math::Transform transform{};
    transform.translation = math::Vec3{10.0f, 20.0f, 30.0f};
    transform.rotation = math::angle_axis(1.57079632679f, math::UNIT_Y);
    transform.scale = math::Vec3{2.0f};

    const math::Vec4 world =
        transform.to_mat4() * math::Vec4{1.0f, 0.0f, 0.0f, 1.0f};

    REQUIRE(world.x == Approx(10.0f).margin(0.001f));
    REQUIRE(world.y == Approx(20.0f).margin(0.001f));
    REQUIRE(world.z == Approx(28.0f).margin(0.001f));
}

TEST_CASE("Aryabhatta provides double precision math types", "[math][aryabhatta][double]") {
    const auto rotation = math::angle_axis(1.5707963267948966, math::DUNIT_Z);
    const math::DVec3 rotated = rotation * math::DUNIT_X;

    REQUIRE(rotated.x == Approx(0.0).margin(1e-10));
    REQUIRE(rotated.y == Approx(1.0).margin(1e-10));
    REQUIRE(rotated.z == Approx(0.0).margin(1e-10));
    REQUIRE(math::DTransform{}.to_mat4()[0][0] == Approx(1.0));
}

TEST_CASE("Aryabhatta interpolation is available directly", "[math][aryabhatta][interpolation]") {
    const math::Vec3 midpoint = math::lerp(
        math::Vec3{0.0f, 2.0f, 4.0f}, math::Vec3{10.0f, 12.0f, 14.0f}, 0.5f);
    REQUIRE(midpoint.x == Approx(5.0f));
    REQUIRE(midpoint.y == Approx(7.0f));
    REQUIRE(midpoint.z == Approx(9.0f));

    const auto q0 = math::angle_axis(0.0f, math::UNIT_Z);
    const auto q90 = math::angle_axis(1.57079632679f, math::UNIT_Z);
    const auto halfway = math::slerp(q0, q90, 0.5f);
    const math::Vec3 direction = halfway * math::UNIT_X;
    REQUIRE(direction.x == Approx(0.70710678f).margin(0.0001f));
    REQUIRE(direction.y == Approx(0.70710678f).margin(0.0001f));
}
