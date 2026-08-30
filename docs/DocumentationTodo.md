# Documentation TODOs

The public SDK reference is organized into Doxygen groups for engine startup,
world editing, runtime callbacks, assets, materials, and scene graph authoring.

Focused public examples now cover `AssetCatalog`, `SceneGraph`,
`RuntimeCommands`, and `EngineConfig` callback wiring under `examples/`, with
matching `voxel_example_*` CMake targets.

The internal network framing, packet flow, version/capability behavior, and
snapshot/live-mutation ordering are documented in
[Network Protocol and Replication](NetworkProtocol.md). The document keeps
unimplemented negotiation, snapshots, security, and runtime integration listed
as explicit gaps.

The optional `engine_api_docs` CMake target generates public SDK documentation
when configured with `VE_BUILD_API_DOCS=ON` and Doxygen is available.
