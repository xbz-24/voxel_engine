# Documentation TODOs

TODO: Add Doxygen groups for the public SDK modules: engine startup, world editing, runtime callbacks, assets, materials, and scene graph.

TODO: Add short public examples for `AssetCatalog`, `SceneGraph`, `RuntimeCommands`, and `EngineConfig` callback wiring so the docstrings point to concrete usage.

The internal network framing, packet flow, version/capability behavior, and
snapshot/live-mutation ordering are documented in
[Network Protocol and Replication](NetworkProtocol.md). The document keeps
unimplemented negotiation, snapshots, security, and runtime integration listed
as explicit gaps.

The optional `engine_api_docs` CMake target generates public SDK documentation
when configured with `VE_BUILD_API_DOCS=ON` and Doxygen is available.
