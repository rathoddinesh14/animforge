# Performance Budgets (Initial Targets)

These are starting targets for a mid-range desktop GPU (e.g. RTX 3060 / RX 6700 class) at 1080p 60 FPS. They will be refined with real measurements.

## Per-Character (Humanoid ~60–80 bones)

| Stage                        | Budget (ms) | Notes                                      |
|-----------------------------|-------------|--------------------------------------------|
| Pose evaluation (CPU)       | ≤ 0.15      | Hierarchical + blending                    |
| IK (if active)              | ≤ 0.10      | FABRIK / two-bone                          |
| Skinning matrix generation  | ≤ 0.05      | Model * inverse bind                       |
| GPU upload (if CPU path)    | ≤ 0.05      | Mapping / memcpy                           |
| Total animation cost        | ≤ 0.40      | Before rendering the mesh                  |

## Scene Level (50 characters)

| Item                     | Target                          |
|--------------------------|---------------------------------|
| Total animation CPU      | ≤ 8–10 ms                       |
| Draw calls (skinned)     | Prefer instancing later         |
| Memory (animation data)  | Aim for streaming / compression |

## Measurement Rules
- Always measure both Debug and Release.
- Use query timers (CPU) and GPU timestamp queries.
- Never optimize without a measured baseline.
- CPU reference path must remain available for correctness validation even after GPU path is primary.

Update this document after each major phase with real numbers.