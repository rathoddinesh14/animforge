# Math Conventions

These conventions are mandatory across the entire codebase. Any deviation must be documented and justified.

## Coordinate System
- **Right-handed**
- **Y-up**
- **Z-forward** (into the screen is –Z in view space for typical OpenGL/Vulkan setups; world space follows right-handed Y-up)

## Matrix Layout
- **Column-major** storage (matches OpenGL and GLM default).
- Multiplication order: `M * v` (matrix on the left).
- Transform composition: `parent * local` (parent applied after local).

## Quaternion
- Storage: `(x, y, z, w)` with `w` as the real part.
- Identity: `(0, 0, 0, 1)`
- Normalization is the caller’s responsibility before use in critical paths; library functions will assert or normalize defensively in debug builds.

## Transform
- Stored as separate Translation (Vec3) + Rotation (Quat) + Scale (Vec3).
- Conversion to Mat4 is explicit via `to_mat4()`.
- Hierarchical evaluation multiplies parent model matrix by local matrix.

## Skinning Matrices
Final skinning matrix for a joint:
```
skin = model_pose * inverse_bind_pose
```
Vertex transformation:
```
skinned = Σ (w_i * skin_i * vertex)
```

## Units
- Time in seconds.
- Angles in radians for all math APIs.
- Distances in meters (convention only; no physics engine yet).

## Precision
- `float` for all runtime animation and rendering math.
- Double used only for offline tools or high-precision offline evaluation if needed.