# Documentation TODOs

TODO: Add Doxygen groups for the public SDK modules: engine startup, world editing, runtime callbacks, assets, materials, and scene graph.

TODO: Add short public examples for `AssetCatalog`, `SceneGraph`, `RuntimeCommands`, and `EngineConfig` callback wiring so the docstrings point to concrete usage.

TODO: Add a network protocol note with packet flow diagrams, version negotiation expectations, and snapshot/live mutation ordering.

The optional `engine_api_docs` CMake target generates public SDK documentation
when configured with `VE_BUILD_API_DOCS=ON` and Doxygen is available.
