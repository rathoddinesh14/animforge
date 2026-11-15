# Math Conventions

These conventions are mandatory across the entire codebase. Any deviation must be documented and justified.

## Primary Library
**Aryabhatta::Math** (public API namespace `aryabhatta::math`).

AnimForge consumes the shared Aryabhatta math library directly instead of wrapping it in a local `aga::math` header. Keep direct GLM usage only for concrete APIs not exposed by Aryabhatta.

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
- We rely on Aryabhatta’s identity quaternion and transform helpers for clarity and consistency.

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

Projection construction must explicitly choose the clip-space convention for the active backend:
- OpenGL: `[-1, 1]` depth range with a camera-space convention matching OpenGL NDC.
- Vulkan: `[0, 1]` depth range with the Vulkan projection layout.

Do not assume a single global GLM depth macro is active across the project; backend-specific projection setup is explicit and reviewed per call site.

## Units
- Time in seconds.
- Angles in **radians** (enforced by `GLM_FORCE_RADIANS`).
- Distances in meters (convention only; no physics engine yet).

## Precision
- `float` for all runtime animation and rendering math.
- Double used only for offline tools or high-precision offline evaluation if needed.
