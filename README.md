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
- vcpkg in manifest mode. Set `VCPKG_ROOT` to the vcpkg checkout when using
  the checked-in CMake presets.
- Vulkan SDK tools `glslc` and `spirv-val` on `PATH`.

## Configure and build

```powershell
cmake -S . -B Builds -A x64 `
  -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build Builds --config Debug --target ALL_BUILD -- /m:1
```

The Windows preset path is also available without a user-specific toolchain
location:

```powershell
$env:VCPKG_ROOT = "C:\path\to\vcpkg"
cmake --preset windows-visual-studio
cmake --build --preset windows-visual-studio-strict
```

Run the single demo with Vulkan explicitly:

```powershell
Builds\Debug\voxel_demo.exe --graphics-api vulkan
```

Use `--smoke-frames 3` for a bounded runtime check, or select the compatibility
backend with `--graphics-api opengl`. To exercise an exact `RuntimeLayout`, pass
`--asset-directory <path>` and `--vulkan-shader-directory <path>`; Vulkan
requires both paths, while OpenGL can use an asset directory without a shader
directory. When runtime smoke tests are enabled, CTest exercises both graphics
selections and the explicit Vulkan layout with the same bounded frame count.

## Validation

```powershell
Builds\Debug\engine_tests.exe
ctest --test-dir Builds -C Debug --output-on-failure
cmake --build Builds --config Debug --target verify_source_policy
```

The source policy checks authored C++ in `include/voxel`, `src`, `Tests`,
`apps`, `examples`, and `packaging`: every `.h/.cpp` family file must stay below 100
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
- `VoxelEngine::Authoring` and the runtime-backed `VoxelEngine::SDK` have
  relocatable CMake compile/link packages, each verified through an installed
  consumer after moving the install prefix. Runtime assets and compiled shaders
  are not part of that package contract. See the
  [Architecture Roadmap](docs/ArchitectureRoadmap.md).
- Vulkan remains the default; Vulkan and OpenGL both have bounded runtime smoke
  coverage.

## Installed CMake packages

Install the runtime-independent authoring component into a chosen prefix:

```powershell
cmake --build Builds --config Release --target voxel_engine_authoring
cmake --install Builds --config Release `
  --prefix C:/path/to/voxel-engine `
  --component Authoring
```

A consuming CMake project can then use:

```cmake
find_package(VoxelEngine 0.5.0 CONFIG REQUIRED COMPONENTS Authoring)
target_link_libraries(my_tool PRIVATE VoxelEngine::Authoring)
```

This component intentionally omits `Engine.h`, `EngineRun.h`, and the `Voxel.h`
runtime umbrella.

The runtime-backed SDK is an additive component. Build it and install both the
authoring base and SDK payload into the same prefix:

```powershell
cmake --build Builds --config Release --target voxel_engine_sdk
cmake --install Builds --config Release `
  --prefix C:/path/to/voxel-engine `
  --component Authoring
cmake --install Builds --config Release `
  --prefix C:/path/to/voxel-engine `
  --component SDK
```

A consumer must make the SDK's external dependencies discoverable, for example
by configuring with the same vcpkg toolchain, and can then link the public
target:

```cmake
find_package(VoxelEngine 0.5.0 CONFIG REQUIRED COMPONENTS SDK)
target_link_libraries(my_app PRIVATE VoxelEngine::SDK)
```

This SDK package is relocatable for configuration, compilation, and linking.
It deliberately installs neither the repository `assets/` tree nor compiled
Vulkan `.spv` files. Running an installed application therefore still requires
an application-owned runtime payload. Applications can provide exact resource
directories and disable development autodiscovery before startup:

```cpp
auto config = voxel::EngineConfig::Default().WithRuntimeLayout(
    voxel::RuntimeLayout{}
        .AssetsAt("C:/my-app/content/assets")
        .VulkanShadersAt("C:/my-app/content/shaders/vulkan"));
```

`AssetsAt` names the directory equivalent to `assets/`; `VulkanShadersAt`
names the directory containing the five compiled `.spv` files. Explicit paths
are validated before a window is created. Relative paths are captured against
the process working directory when the `Engine` is constructed. Without
`RuntimeLayout`, development
builds discover assets and compiled shaders from ancestors of the executable.
The installed consumer smoke still does not claim payload deployment or grant
content rights. In particular, do not copy or redistribute
Minecraft/Mojang-derived repository assets as part of an SDK installation.

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
