#pragma once

// =============================================================================
// AnimForge math configuration
// Forces GLM into the exact conventions required by this project and both
// OpenGL and Vulkan (column-major, right-handed, Y-up).
// =============================================================================

// GLM configuration – must appear before any GLM header
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE   // Vulkan-style depth; OpenGL path will adjust if needed
#define GLM_FORCE_LEFT_HANDED        // We override this – see note below
// Actually we want right-handed. GLM default is right-handed when LEFT_HANDED is not defined.
#undef GLM_FORCE_LEFT_HANDED

#define GLM_FORCE_XYZW_ONLY
#define GLM_ENABLE_EXPERIMENTAL      // for matrix_transform, quaternion extras if needed

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

namespace aga::math {

// Type aliases – single source of truth for the whole engine
using Vec2  = glm::vec2;
using Vec3  = glm::vec3;
using Vec4  = glm::vec4;
using Mat3  = glm::mat3;
using Mat4  = glm::mat4;
using Quat  = glm::quat;

// Convenience constants
inline constexpr Vec3 UNIT_X{1.0f, 0.0f, 0.0f};
inline constexpr Vec3 UNIT_Y{0.0f, 1.0f, 0.0f};
inline constexpr Vec3 UNIT_Z{0.0f, 0.0f, 1.0f};
inline constexpr Quat IDENTITY_QUAT{1.0f, 0.0f, 0.0f, 0.0f}; // w, x, y, z in GLM

// ---------------------------------------------------------------------------
// Transform – TRS decomposed form used throughout the animation system
// ---------------------------------------------------------------------------
struct Transform {
    Vec3 translation{0.0f};
    Quat rotation{1.0f, 0.0f, 0.0f, 0.0f}; // identity (w,x,y,z)
    Vec3 scale{1.0f};

    Mat4 to_mat4() const {
        // GLM: T * R * S  (column-major, right-handed)
        Mat4 t = glm::translate(Mat4{1.0f}, translation);
        Mat4 r = glm::mat4_cast(rotation);
        Mat4 s = glm::scale(Mat4{1.0f}, scale);
        return t * r * s;
    }

    static Transform identity() {
        return Transform{};
    }
};

// Free functions that match our coding style
inline Mat4 translate(const Vec3& v) {
    return glm::translate(Mat4{1.0f}, v);
}

inline Mat4 scale(const Vec3& v) {
    return glm::scale(Mat4{1.0f}, v);
}

inline Mat4 rotate(float radians, const Vec3& axis) {
    return glm::rotate(Mat4{1.0f}, radians, axis);
}

inline Quat angle_axis(float radians, const Vec3& axis) {
    return glm::angleAxis(radians, axis);
}

inline Vec3 lerp(const Vec3& a, const Vec3& b, float t) {
    return glm::mix(a, b, t);
}

inline Quat slerp(const Quat& a, const Quat& b, float t) {
    return glm::slerp(a, b, t);
}

inline Transform lerp(const Transform& a, const Transform& b, float t) {
    Transform result;
    result.translation = lerp(a.translation, b.translation, t);
    result.rotation    = slerp(a.rotation, b.rotation, t);
    result.scale       = lerp(a.scale, b.scale, t);
    return result;
}

} // namespace aga::math