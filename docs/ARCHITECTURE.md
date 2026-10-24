# Architecture

## High-Level Overview

```
+-------------------+       +-------------------+
|   Asset Loading   | ----> |  Animation Data   |
| (glTF / Assimp)   |       | (Skeleton, Clips) |
+-------------------+       +-------------------+
                                      |
                                      v
+-------------------+       +-------------------+
|  Animation Graph  | ----> |   Pose Evaluator  |
| (State / Blend)   |       | (Local → Model)   |
+-------------------+       +-------------------+
                                      |
                    +-----------------+-----------------+
                    |                                   |
                    v                                   v
          +-------------------+               +-------------------+
          |   CPU Skinning    |               |   GPU Skinning    |
          | (reference path)  |               | (OpenGL / Vulkan) |
          +-------------------+               +-------------------+
                    |                                   |
                    +-----------------+-----------------+
                                      |
                                      v
                            +-------------------+
                            |  Render Backend   |
                            | (GL or Vulkan)    |
                            +-------------------+
```

## Core Modules

### 1. `aga::math`
- Vec3, Quat, Mat4, Transform (TRS).
- Interpolation helpers (lerp, slerp, cubic, Hermite).
- Coordinate system utilities.
- Strictly header-only where possible; no external math library dependency in the hot path (GLM optional for samples).

### 2. `aga::animation`
- `Skeleton` – joint hierarchy + inverse bind poses.
- `AnimationClip` – keyframes per joint (compressed later).
- `Pose` – local + model-space transforms.
- `Animator` – playback controller + blending layers.
- `IKSolver` family.
- `RootMotionExtractor`.

### 3. `aga::rendering`
- Abstract `SkinningBackend` interface.
- `OpenGLSkinning` and `VulkanSkinning` implementations.
- Debug draw utilities.

### 4. `aga::platform`
- Window, input, timing (Windows first via Win32 or GLFW).
- File I/O abstraction.

### 5. `aga::core`
- Logging, assertions, memory arenas (optional), timing.

## Data Flow (Typical Frame)

1. Animation graph / state machine produces a set of weighted clips + blend parameters.
2. Pose evaluator computes local transforms → hierarchical model-space pose.
3. Optional IK post-process modifies the pose.
4. Optional procedural layers (springs, look-at) applied.
5. Skinning matrices (model * inverseBind) generated.
6. Matrices uploaded (or computed on GPU) and mesh rendered.

## Key Design Principles

- **Correctness first**: CPU reference path always available.
- **Explicit ownership**: no hidden global state for animation data.
- **Separation of evaluation and rendering**: the same pose can be skinned by either backend.
- **Data-oriented hot paths**: contiguous arrays of transforms preferred over deep class hierarchies.
- **Platform abstraction only where necessary**: Windows implementation lives under `platform/windows`.

## Future Extensibility Points

- Animation compression (ACL or custom).
- GPU-driven animation (compute dispatch of entire pose evaluation).
- Motion matching database.
- Multi-threaded pose evaluation for crowds.