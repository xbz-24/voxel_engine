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

The source files remain in `Builds/` temporarily. Their CMake target is the
authoritative subsystem boundary while they are moved into feature directories.
