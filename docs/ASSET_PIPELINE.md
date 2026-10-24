# Asset Pipeline

## Primary Runtime Format
**glTF 2.0** (binary `.glb` preferred for samples).

Required features:
- Skinned meshes (`SKIN`, `JOINTS_0`, `WEIGHTS_0`)
- Skeletal animations (`channels` targeting node translation/rotation/scale)
- Node hierarchy that matches the skeleton

## Supported Import Paths

1. **Runtime (preferred for samples)**  
   - tinygltf or cgltf for minimal dependency footprint.

2. **Authoring / conversion**  
   - Assimp for FBX, Collada, etc. during development.
   - Convert to glTF offline; do not ship Assimp in the final runtime if avoidable.

## Coordinate System Conversion
- glTF is Y-up, right-handed — matches our conventions.  
- FBX often needs axis conversion; handle it in the importer, never in the core evaluation code.

## Compression (Future)
- Investigate ACL (Animation Compression Library) or custom quantization.
- Store compressed clips; decompress on load or stream.

## Sample Assets
Place small glTF files under `assets/models/`.  
Large files should be downloaded via a script or Git LFS (document the process).

## Naming Conventions
- Skeleton joints: use the names present in the asset; provide a mapping table if retargeting is required later.
- Animation clips: clear descriptive names (`Idle`, `Walk_Forward`, `Run_Stop`, etc.).