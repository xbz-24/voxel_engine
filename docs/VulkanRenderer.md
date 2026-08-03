# Vulkan Renderer and Shader System

The Vulkan renderer owns the GPU path used by voxel applications. It keeps frame submission, world-mesh upload, shader data, and overlay recording behind four separate responsibilities:

- `VulkanFrameOrchestrator` acquires swapchain images, waits on frame fences, records command buffers, submits work, and presents.
- `VulkanGpuChunkRenderer` owns the render pass, voxel and sky pipelines, framebuffers, world-mesh buffers, and shader resources.
- `VulkanImGuiOverlay` records optional UI commands after the scene draw and before the render pass ends.
- `cmake/VulkanShaders.cmake` compiles GLSL with warnings as errors and validates every generated SPIR-V module.

## Frame Flow

Each frame-in-flight has its own `VulkanVoxelFrameUniforms` buffer and descriptor set. The orchestrator waits for that frame's fence before the renderer updates the buffer, so CPU writes cannot race an earlier GPU submission. A single descriptor layout is shared by the sky and voxel pipelines.

The renderer records the scene in this order:

1. Upload camera, light matrix, elapsed time, viewport, atmosphere, and surface settings to descriptor set 0, binding 0.
2. Render the indexed world mesh into the current frame's directional shadow depth image.
3. Transition that image for PCF sampling through the shadow render pass.
4. Begin the main render pass and bind the same frame descriptor set.
5. Draw a full-screen procedural sky and the indexed voxel world mesh.
6. Record the optional overlay and end the render pass.

## Shader ABI

The CPU and GLSL contracts deliberately use different update paths:

- A 64-byte matrix push constant changes between the sky and world draws.
- A 224-byte `std140` uniform block changes once per frame and includes the directional light view-projection matrix.
- The uniform block lives at set 0, binding 0; fragment stages consume the environment and the shadow vertex stage consumes the light matrix.

Binding 1 contains the current frame's 2048x2048 comparison depth image. Each frame in flight owns a separate shadow image, view, and framebuffer, so updating one frame never races a previous GPU submission. The main fragment shader uses a manually weighted 3x3 PCF kernel and receiver bias. The light projection is snapped to shadow-map texels to prevent subpixel swimming as the camera moves.

The CPU builds a second index buffer containing only voxel faces oriented toward the configured sun. The overlay reports visual and shadow index counts so the depth-pass amplification remains measurable on large worlds.

`VulkanGpuChunkRendererTypes.h` has size and offset assertions for the C++ side.
`voxel_transform.glsl` and `voxel_environment.glsl` define the GLSL side. Keep
both definitions synchronized whenever the ABI changes, then update
`Tests/RenderBackendVulkanTests.inl` with the expected offsets.

The public `voxel::VoxelRenderStyle` is backend-neutral. Its values are
validated before translation into the internal style. The Vulkan path currently
consumes all atmosphere and surface controls; compatibility backends may render
them differently while preserving the same startup contract.

## Shader Modules

The four compiled stages are `voxel_chunk.vert`, `voxel_chunk.frag`,
`voxel_sky.vert`, and `voxel_sky.frag`. Reusable `.glsl` modules divide the
fragment work into focused layers:

- Environment: frame uniforms, transform ABI, math, noise, and tone mapping.
- Materials: surface masks, material tint, emission, and procedural detail.
- Lighting: GGX BRDF, Oren-Nayar rough diffusion, procedural ambient occlusion,
  directional shadow mapping, subsurface response, contact shadowing, and
  reflections.
- Atmosphere: Beer-Lambert aerial perspective, sky scattering, animated clouds
  with self-shadow absorption, and water reflections.

Procedural normal and edge detail use screen-space derivatives plus camera
distance to fade frequencies that cannot be sampled reliably. Keep that
filtering in place when adding higher-frequency noise; unfiltered detail causes
shimmer and wastes fragment work in the distance.

Include guards make modules composable. New modules must also be listed in
`VE_VULKAN_SHADER_INCLUDE_SOURCES`; this gives CMake the complete dependency
graph and recompiles all affected stages when an include changes.

## Adding a Shader Control

Keep a new user-facing control end to end:

1. Add the value and fluent method to `voxel::VoxelRenderStyle`.
2. Mirror it in the internal `VoxelRenderStyle` and configuration translator.
3. Validate public and direct `EngineCreateInfo` inputs.
4. Pack it into an aligned frame-uniform slot and expose it through
   `VoxelEnvironment`.
5. Consume it in a focused shader module and add API, validation, and ABI tests.

Do not silently expand the push-constant block for frame-wide data. The 64-byte
matrix contract remains portable across devices with the Vulkan minimum push
constant limit, while the uniform buffer is the intended extension point.

## Verification

Run the shader compiler before the C++ suite so GLSL failures stay local:

```powershell
cmake --build Builds --config Debug --target ve_vulkan_shaders
cmake --build Builds --config Debug --target voxel_demo engine_tests
Builds\Debug\engine_tests.exe
ctest --test-dir Builds -C Debug --output-on-failure
$env:VE_VULKAN_VALIDATION='1'
Builds\Debug\voxel_demo.exe --smoke-frames 2
```

The final smoke is required for descriptor lifetime, pipeline-layout, and
synchronization changes because those contracts cannot be proven by compilation
alone.
