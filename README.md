# Voxel Engine

Modern C++23 voxel-engine workbench with one authored application, a public
configuration SDK, Vulkan as the default runtime backend, and an explicit
OpenGL compatibility path.

The project currently targets Windows first. The supported application is
`voxel_demo`; the public examples under `examples/` exercise the same
`VoxelEngine::SDK` surface without introducing alternate launchers.

## Requirements

- Visual Studio 2022 or newer with the Desktop C++ workload.
- CMake 3.25 or newer.
- vcpkg in manifest mode.
- Vulkan SDK tools `glslc` and `spirv-val` on `PATH`.

## Configure and build

```powershell
cmake -S . -B Builds -A x64 `
  -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build Builds --config Debug --target ALL_BUILD -- /m:1
```

Run the single demo with Vulkan explicitly:

```powershell
Builds\Debug\voxel_demo.exe --graphics-api vulkan
```

Use `--smoke-frames 3` for a bounded runtime check. The CLI also accepts
`--graphics-api opengl`, but that compatibility path currently has a known
runtime crash and is not part of automated smoke coverage.

## Validation

```powershell
Builds\Debug\engine_tests.exe
ctest --test-dir Builds -C Debug --output-on-failure
cmake --build Builds --config Debug --target verify_source_policy
```

The source policy checks authored C++ in `include/voxel`, `src`, `Tests`,
`apps`, and `examples`: every `.h/.cpp` family file must stay below 100
physical lines, and `.inl` files are forbidden. Public headers are also
compiled individually to enforce self-containment.

## Current architecture status

- `voxel_demo` is the only authored application.
- `VoxelEngine::Authoring` provides builders, validation, assets, materials,
  scenes, and world serialization without linking the private runtime.
- `VoxelEngine::SDK` adds the runtime adapter and voxel sandbox.
- The public SDK supports world configuration/serialization, callbacks,
  assets/materials/scene authoring, and an embeddable frame loop.
- Runtime asset-catalog loading, material binding, and scene-graph rendering
  remain deliberately disabled and report validation errors.
- The SDK is build-tree consumable but not yet an installed, relocatable CMake
  package. See [Architecture Roadmap](docs/ArchitectureRoadmap.md).
- OpenGL is available for diagnosis but Vulkan is the tested runtime path.

## Documentation

- [Build organization](docs/BuildOrganization.md)
- [Architecture roadmap](docs/ArchitectureRoadmap.md)
- [Code style](docs/CodeStyle.md)
- [Vulkan renderer](docs/VulkanRenderer.md)

API documentation can be enabled with `-DVE_BUILD_API_DOCS=ON` when Doxygen is
installed, then built with the `engine_api_docs` target.

## Licensing

No license has been selected yet. Until the repository owner adds one, the
code should not be treated as granting permission to copy, redistribute, or
reuse it outside the rights provided by applicable law.
