# Architecture Roadmap

## Current Baseline

- `ve_runtime` owns the generic window, asset paths, timing, logging, and the
  content-module lifecycle. A target-graph gate keeps it independent from
  world, network, and concrete renderer aggregation.
- `ve_voxel_sandbox` owns voxel gameplay, input, editor UI, HUD, world edits,
  and backend-specific runtime drivers. `ve_app` remains only as a compatibility
  alias.
- `VoxelEngine::Authoring` is independent from the private runtime;
  `VoxelEngine::SDK` adds the runtime adapter and voxel sandbox. Both are
  installable CMake compile/link packages; the SDK exports its static internal
  closure as implementation detail targets.
- `voxel_demo` is the only authored application. Examples consume the public
  SDK instead of defining alternate launchers.
- Vulkan is the default runtime path. OpenGL remains an explicit compatibility
  choice, and both backends have bounded runtime smoke coverage.

The codebase already has `GameModel`/`GameController`, a screen stack,
`NetworkSession`, GLFW-backed window ownership, and both Vulkan and OpenGL
ImGui integrations. Those are established components, not pending patterns.

## Next Release Boundary

- Keep the installed SDK's current boundary precise: its headers, libraries,
  dependency discovery, and CMake targets are relocatable and covered by a
  moved-prefix consumer smoke. Runtime assets, compiled Vulkan shaders, and an
  executable runtime-layout contract remain pending.
- Do not add the repository `assets/` tree or generated `.spv` files to the SDK
  install as a shortcut. Define the runtime payload layout and settle the
  redistribution rights of Minecraft/Mojang-derived assets before adding a
  runtime packaging component.
- Maintain the canonical project/SDK/package version. Configure-time gates now
  keep the vcpkg manifest, CMake package metadata, and public
  `voxel::Version()` synchronized; graphics API metadata should continue to
  follow the same release version.
- Keep the pinned vcpkg baseline on an intentional update cadence. Baseline
  changes must pass a clean manifest restore and the complete CI matrix before
  they are merged.
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
- Keep [Network Protocol and Replication](NetworkProtocol.md) synchronized with
  the internal wire contract. Before exposing multiplayer through the public
  SDK, implement the documented `ServerWelcome` negotiation, authoritative
  player snapshots, initial world snapshots, revision barriers, and runtime
  integration.
