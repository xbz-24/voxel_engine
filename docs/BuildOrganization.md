# Build Organization

The CMake build is split into subsystem libraries so Visual Studio can compile
and display the engine as multiple projects instead of one large executable.

## Targets

- `ve_core`: reusable utilities, logging, ECS, physics, simulation, and memory.
- `ve_render`: API-independent rendering, geometry, import, and render passes.
- `ve_render_backends`: OpenGL compatibility, Vulkan, and DirectX 12 backends.
- `ve_world`: blocks, chunks, terrain generation, meshing, and world state.
- `ve_network`: sockets, sessions, protocol, multiplayer, and replication.
- `ve_app`: engine lifecycle, editor, input, HUD, settings, and window setup.
- `voxel_engine`: the small executable that links the subsystem libraries.
- `engine_tests`: tests linked against the same libraries used by the engine.

Changing one implementation file now recompiles its owning library and relinks
the affected executable. It does not compile every engine implementation again.

Source files live under `src/core`, `src/render`, `src/render/backends`,
`src/world`, `src/network`, and `src/app`. Those physical directories are also
the CMake and Visual Studio project boundaries.

All project targets compile in the compiler's latest C++ mode. The default
strict profile enables extended warnings, conforming MSVC behavior, SDL checks,
parallel compilation, and treats every project warning as an error.

Vulkan is the default runtime backend. The current Vulkan path initializes the
window, instance, device, swapchain, and a minimal clear/present frame while the
legacy voxel world renderer continues to live behind the explicit OpenGL
compatibility backend.

## Visual Studio

Generate the Visual Studio solution with:

```powershell
cmake --preset windows-visual-studio
```

Then open `out/build/windows-visual-studio/VoxelEngine.slnx`.
