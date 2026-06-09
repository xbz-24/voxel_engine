# Architecture Roadmap

## Direction

The engine should move from one flat `Builds/` folder into feature folders:

- `engine/app`: engine runtime, screens, main loop.
- `engine/mvc`: `GameModel`, `GameView`, `GameController`.
- `engine/render`: render state, 2D primitives, 3D primitives, GPU resources.
- `engine/world`: world, chunks, terrain, block storage.
- `engine/network`: sockets, protocol, sessions, replication.
- `engine/log`: logger, categories, formatting, sinks.
- `gameplay`: input mapping, hotbar, block interaction, player movement.

## Patterns To Apply Now

- MVC for game state, rendering, and input orchestration.
- Facade for `NetworkSession`, hiding sockets from gameplay.
- Command/request objects for rendering, meshing, networking, and world edits.
- Screen stack for menu, loading, gameplay, and editor screens.
- Adapter for the current GLFW window so an ImGui tools layer can be added cleanly.

## Immediate Refactors

- Move remaining frame gameplay code from `Engine` into `GameController`.
- Move 3D/HUD draw orchestration from `Engine` into `GameView`.
- Rename old member fields gradually to Google-style trailing underscore.
- Introduce `WindowBackend` before considering any non-GLFW window implementation.
- Add ImGui through GLFW/OpenGL first; do not mix WinForms into the game render loop.
