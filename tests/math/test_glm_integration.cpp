#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <aga/math/math.hpp>

using namespace aga::math;
using Catch::Approx;

// =============================================================================
// GLM Integration Tests
// These tests verify that GLM is correctly configured for AnimForge conventions
// and that our thin wrapper (aga::math) behaves as expected.
// =============================================================================

TEST_CASE("GLM is configured with column-major matrices", "[math][glm]") {
    Mat4 m{1.0f}; // identity

    // Column-major: element (row, col) is accessed as m[col][row]
    // Setting the first column should affect memory layout accordingly.
    m[0] = Vec4{2.0f, 3.0f, 4.0f, 5.0f};

    REQUIRE(m[0][0] == Approx(2.0f));
    REQUIRE(m[0][1] == Approx(3.0f));
    REQUIRE(m[0][2] == Approx(4.0f));
    REQUIRE(m[0][3] == Approx(5.0f));
}

TEST_CASE("GLM quaternion storage is (w, x, y, z) and identity is correct", "[math][glm]") {
    Quat q = IDENTITY_QUAT;

    // GLM stores quaternions as w, x, y, z
    REQUIRE(q.w == Approx(1.0f));
    REQUIRE(q.x == Approx(0.0f));
    REQUIRE(q.y == Approx(0.0f));
    REQUIRE(q.z == Approx(0.0f));
}

TEST_CASE("Right-handed coordinate system - cross product of X and Y yields +Z", "[math][glm]") {
    Vec3 x = UNIT_X;
    Vec3 y = UNIT_Y;
    Vec3 z = glm::cross(x, y);

    REQUIRE(z.x == Approx(0.0f));
    REQUIRE(z.y == Approx(0.0f));
    REQUIRE(z.z == Approx(1.0f));
}

TEST_CASE("Transform::to_mat4 produces T * R * S (column-major)", "[math][transform]") {
    Transform t;
    t.translation = Vec3{10.0f, 20.0f, 30.0f};
    t.rotation = angle_axis(glm::radians(90.0f), UNIT_Y);
    t.scale = Vec3{2.0f, 2.0f, 2.0f};

    Mat4 m = t.to_mat4();

    // A point on the local +X axis after scale(2) → (2,0,0)
    // after 90° Y rotation → (0,0,-2)
    // after translation    → (10,20,28)
    Vec4 p_local{1.0f, 0.0f, 0.0f, 1.0f};
    Vec4 p_world = m * p_local;

    REQUIRE(p_world.x == Approx(10.0f).margin(0.001f));
    REQUIRE(p_world.y == Approx(20.0f).margin(0.001f));
    REQUIRE(p_world.z == Approx(28.0f).margin(0.001f));
}

TEST_CASE("slerp of identical quaternions returns the same quaternion", "[math][quat]") {
    Quat a = angle_axis(glm::radians(45.0f), UNIT_Y);
    Quat b = a;

    Quat result = slerp(a, b, 0.5f);

    REQUIRE(result.w == Approx(a.w).margin(0.0001f));
    REQUIRE(result.x == Approx(a.x).margin(0.0001f));
    REQUIRE(result.y == Approx(a.y).margin(0.0001f));
    REQUIRE(result.z == Approx(a.z).margin(0.0001f));
}

TEST_CASE("lerp of Vec3 is linear", "[math][vec3]") {
    Vec3 a{0.0f, 0.0f, 0.0f};
    Vec3 b{10.0f, 20.0f, 30.0f};

    Vec3 mid = lerp(a, b, 0.5f);

    REQUIRE(mid.x == Approx(5.0f));
    REQUIRE(mid.y == Approx(10.0f));
    REQUIRE(mid.z == Approx(15.0f));
}

TEST_CASE("Transform::identity produces identity matrix", "[math][transform]") {
    Transform id = Transform::identity();
    Mat4 m = id.to_mat4();

    REQUIRE(m[0][0] == Approx(1.0f));
    REQUIRE(m[1][1] == Approx(1.0f));
    REQUIRE(m[2][2] == Approx(1.0f));
    REQUIRE(m[3][3] == Approx(1.0f));

    // Off-diagonal should be zero
    REQUIRE(m[0][1] == Approx(0.0f));
    REQUIRE(m[0][2] == Approx(0.0f));
    REQUIRE(m[1][0] == Approx(0.0f));
}