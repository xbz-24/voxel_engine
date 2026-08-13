# Architecture Roadmap

## Current Baseline

- `ve_runtime` owns the generic window, asset paths, timing, logging, and the
  content-module lifecycle. A target-graph gate keeps it independent from
  world, network, and concrete renderer aggregation.
- `ve_voxel_sandbox` owns voxel gameplay, input, editor UI, HUD, world edits,
  and backend-specific runtime drivers. `ve_app` remains only as a compatibility
  alias.
- `VoxelEngine::Authoring` is independent from the private runtime;
  `VoxelEngine::SDK` adds the runtime adapter and voxel sandbox.
- `voxel_demo` is the only authored application. Examples consume the public
  SDK instead of defining alternate launchers.
- Vulkan is the default runtime path. OpenGL remains an explicit compatibility
  choice, and both backends have bounded runtime smoke coverage.

The codebase already has `GameModel`/`GameController`, a screen stack,
`NetworkSession`, GLFW-backed window ownership, and both Vulkan and OpenGL
ImGui integrations. Those are established components, not pending patterns.

## Next Release Boundary

- Install and export the runtime-backed `VoxelEngine::SDK` only after its
  runtime assets and private target closure have a relocatable contract. Keep
  the installed `VoxelEngine::Authoring` package as the smaller supported
  boundary in the meantime.
- Maintain the canonical project/SDK/package version. Configure-time gates now
  keep the vcpkg manifest, CMake package metadata, and public
  `voxel::Version()` synchronized; graphics API metadata should continue to
  follow the same release version.
- Pin the vcpkg dependency baseline once the repository has a release branch
  and an intentional dependency-update procedure.
- Select and add a repository license before describing source distribution as
  an open-source release.

## Runtime and Rendering Work

- Feed `AssetCatalog`, `MaterialLibrary`, and `SceneGraph` into runtime systems.
  Until then, configured authored assets and entities must continue to produce
  explicit validation issues instead of silently being ignored.
- Keep the Vulkan and OpenGL bounded runtime smokes green as backend ownership
  and shutdown ordering evolve.
- Continue moving backend-owned resources and draw orchestration out of the
  compatibility renderer; retain migration-status metadata until direct tests
  prove each backend contract.
- Add stable public entity identifiers to frame callbacks only after entity
  ownership and event lifetime are defined as runtime contracts.

## World and Network Work

- Separate chunk storage, generation, meshing, and event publication far enough
  that tools can use world data without a graphics runtime.
- Replace grow-only chunk assumptions with explicit horizontal streaming bounds
  and vertical world bounds.
- Document packet flow, protocol-version negotiation, snapshot ordering, and
  live world-mutation ordering before exposing multiplayer through the public
  SDK.
