# Documentation TODOs

TODO: Add Doxygen groups for the public SDK modules: engine startup, world editing, runtime callbacks, assets, materials, and scene graph.

TODO: Add short public examples for `AssetCatalog`, `SceneGraph`, `RuntimeCommands`, and `EngineConfig` callback wiring so the docstrings point to concrete usage.

TODO: Add a network protocol note with packet flow diagrams, version negotiation expectations, and snapshot/live mutation ordering.

TODO: Add a renderer backend note describing how `VulkanFrameOrchestrator`, `VulkanGpuChunkRenderer`, the overlay, and shader compilation outputs fit together.

TODO: Audit implementation-heavy `.inl` files under `src/api` if any of them become public extension points later; today the public contract is documented in `include/voxel`.

TODO: Add a generated-docs target once the CMake layout has a stable Doxygen dependency story.
