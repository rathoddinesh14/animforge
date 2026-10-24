# Math Conventions

These conventions are mandatory across the entire codebase. Any deviation must be documented and justified.

## Primary Library
**GLM** (configured via `include/aga/math/math.hpp`).

All code must include `<aga/math/math.hpp>` rather than raw GLM headers so that the project-wide configuration is applied.

## Coordinate System
- **Right-handed**
- **Y-up**
- **Z-forward** (world space). In view space the camera looks down –Z (OpenGL style).

## Matrix Layout
- **Column-major** storage (matches OpenGL, Vulkan, and GLM default).
- Multiplication order: `M * v` (matrix on the left).
- Transform composition: `parent * local` (parent applied after local).

## Quaternion
- GLM storage order: `(w, x, y, z)` with `w` as the real part.
- Identity: `quat(1, 0, 0, 0)`
- We expose `aga::math::IDENTITY_QUAT` for clarity.

## Transform
- Stored as separate Translation (`Vec3`) + Rotation (`Quat`) + Scale (`Vec3`).
- Conversion to `Mat4` is performed by `Transform::to_mat4()` which builds `T * R * S`.

## Skinning Matrices
Final skinning matrix for a joint:
```
skin = model_pose * inverse_bind_pose
```
Vertex transformation:
```
skinned = Σ (w_i * skin_i * vertex)
```

## Depth & Projection Notes
| API     | NDC Y direction | Depth range |
|---------|-----------------|-------------|
| OpenGL  | Up (+Y)         | [-1, 1]     |
| Vulkan  | Down (–Y)       | [0, 1]      |

We enable `GLM_FORCE_DEPTH_ZERO_TO_ONE` so that GLM’s perspective helpers produce Vulkan-style depth.  
When targeting pure OpenGL we will adjust the projection matrix (or use a reverse-Z variant).

## Units
- Time in seconds.
- Angles in **radians** (enforced by `GLM_FORCE_RADIANS`).
- Distances in meters (convention only; no physics engine yet).

## Precision
- `float` for all runtime animation and rendering math.
- Double used only for offline tools or high-precision offline evaluation if needed.
