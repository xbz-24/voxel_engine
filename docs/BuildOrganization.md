# Build Organization

The CMake build is split into subsystem libraries so Visual Studio can compile
and display the engine as multiple projects instead of one large executable.

## Targets

- `ve_core`: reusable utilities, logging, ECS, physics, simulation, and memory.
- `ve_render`: API-independent rendering, geometry, import, and render passes.
- `ve_render_backends`: OpenGL compatibility, Vulkan, and DirectX 12 backends.
- `ve_world`: blocks, chunks, terrain generation, meshing, and world state.
- `ve_network`: sockets, sessions, protocol, multiplayer, and replication.
- `ve_runtime`: generic window, assets, timing, logging, and content-module host.
- `ve_voxel_sandbox`: voxel gameplay, editor, input, HUD, and render drivers.
- `ve_app`: compatibility link alias for `ve_voxel_sandbox`; build the real
  target with `--target ve_voxel_sandbox`.
- `voxel_engine_authoring` / `VoxelEngine::Authoring`: public configuration,
  validation, assets, materials, scene graph, and world serialization without
  private runtime dependencies.
- `voxel_engine_sdk` / `VoxelEngine::SDK`: complete public surface plus the
  runtime adapter and voxel sandbox.
- `voxel_demo`: the single authored application that consumes `VoxelEngine::SDK`.
- `engine_tests`: tests linked against the same libraries used by the engine.

Changing one implementation file now recompiles its owning library and relinks
the affected executable. It does not compile every engine implementation again.

Source files live under `src/core`, `src/render`, `src/render/backends`,
`src/world`, `src/network`, `src/runtime`, and `src/app`. Those physical
directories are also the CMake and Visual Studio project boundaries. The
`runtime_target_graph_policy` gate keeps `ve_runtime` independent from world,
network, and concrete render-backend aggregation.

All project targets compile in the compiler's latest C++ mode. The default
strict profile enables extended warnings, conforming MSVC behavior, SDL checks,
parallel compilation, and treats every project warning as an error.

Vulkan is the default runtime backend. It renders the procedural sky and indexed
voxel world mesh, owns per-frame shader resources, and records the optional
overlay before presentation. See [Vulkan Renderer and Shader System](VulkanRenderer.md)
for the frame flow, shader ABI, and extension checklist. The legacy renderer
continues to live behind the explicit OpenGL compatibility backend.

The demo selects its backend with `--graphics-api vulkan|opengl`; omitting the
option keeps Vulkan as the default. Automated runtime coverage currently
registers only the green Vulkan smoke. OpenGL remains available for manual
diagnosis while its known runtime crash is tracked in the architecture roadmap.

## Visual Studio

Generate the Visual Studio solution with:

```powershell
cmake --preset windows-visual-studio
```

Then open `out/build/windows-visual-studio/VoxelEngine.slnx`.
