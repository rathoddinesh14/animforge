# Public API Overview (Initial Draft)

Namespace: `aga`

## Math

```cpp
namespace aga::math {
    struct Vec3 { float x, y, z; };
    struct Quat { float x, y, z, w; };
    struct Mat4 { float m[16]; };          // column-major
    struct Transform {
        Vec3 translation;
        Quat rotation;
        Vec3 scale;
    };

    Transform lerp(const Transform& a, const Transform& b, float t);
    Transform slerp(const Transform& a, const Transform& b, float t); // rotation via slerp
    Mat4 to_mat4(const Transform& t);
    // ... hierarchical multiply, inverse, etc.
}
```

## Animation Core

```cpp
namespace aga::animation {

struct Joint {
    std::string name;
    int parent = -1;                // -1 = root
    math::Transform bind_local;     // local bind pose
    math::Mat4 inverse_bind;        // model-space inverse bind
};

class Skeleton {
public:
    std::span<const Joint> joints() const;
    int find_joint(std::string_view name) const;
    // ...
};

struct Keyframe {
    float time;
    math::Transform value;
};

class AnimationClip {
public:
    float duration() const;
    // sample local pose at time t (with looping option)
    void sample(float time, bool looping, std::span<math::Transform> out_local) const;
};

class Pose {
public:
    void resize(size_t joint_count);
    std::span<math::Transform> local();
    std::span<math::Mat4> model();          // computed by evaluate
    void evaluate(const Skeleton& skeleton); // local → model hierarchical
};

class Animator {
public:
    void set_skeleton(const Skeleton* skel);
    void play(const AnimationClip* clip, float speed = 1.0f, bool loop = true);
    void cross_fade(const AnimationClip* clip, float duration);
    void update(float dt);
    const Pose& current_pose() const;
    // root motion extraction interface
};

} // namespace aga::animation
```

## Skinning

```cpp
namespace aga::rendering {

struct SkinningMatrices {
    std::span<const math::Mat4> matrices; // model * inverse_bind
};

class ISkinningBackend {
public:
    virtual ~ISkinningBackend() = default;
    virtual void upload(const SkinningMatrices& mats) = 0;
    virtual void bind() = 0;
};

// Concrete implementations in opengl/ and vulkan/

} // namespace aga::rendering
```

## Debug

```cpp
namespace aga::debug {
    void draw_skeleton(const Pose& pose, const Skeleton& skel, const math::Mat4& world);
    void draw_bone_weights(...);
    // etc.
}
```

This API is deliberately minimal and will evolve with each phase. Breaking changes are expected until Phase 2 is complete. All public headers live under `include/aga/`.