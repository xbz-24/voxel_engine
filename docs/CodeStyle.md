# Voxel Engine Code Style

This project is moving toward Google-style C++ with small engine-specific rules.

## Naming

- Types: `PascalCase`, for example `GameModel`.
- Functions: `PascalCase` for public API, matching Google C++ style.
- Local variables and parameters: `lower_snake_case`.
- Class data members: `lower_snake_case_`.
- Constants: `kPascalCase` for non-member constants.
- Namespaces: short lowercase namespaces, for example `ve::engine`.

## Function Shape

- Prefer one visible responsibility per function.
- Prefer request/config structs when a function would need many parameters.
- Document public functions with `@param` and `@return`.
- Keep every authored C++ source and header below 100 lines. Reach that limit through cohesive components with normal `.h`/`.cpp` boundaries; never use textual include fragments or formatting tricks to satisfy the metric.

## Ownership

- Prefer value ownership for model data.
- Prefer `std::unique_ptr` for OpenGL-backed resources that need delayed construction.
- Prefer move-only outputs for heavy mesh/network payloads.
- Avoid raw owning pointers.
- Use raw pointers only for nullable, non-owning views. Prefer references when null is invalid, `std::span` for contiguous borrowed ranges, and smart pointers only when ownership is transferred or shared.
- Prefer `std::ranges`, constrained templates, and small generic helpers when they remove real duplication without hiding domain names.

## Enforcement Rule

Every authored C++ file in the policy roots must satisfy this guide. Refactors
must preserve the limit instead of moving implementation into textual include
fragments.

## Automated Source Policy

The `source_policy` CTest scans authored C++ under `include/voxel`, `src`,
`Tests`, `apps`, and `examples`. It counts `.cc`, `.cpp`, `.cppm`, `.cxx`,
`.h`, `.hh`, `.hpp`, `.hxx`, `.ipp`, `.ixx`, and `.tpp` files. Each must
contain at most 99 physical lines, and `.inl` files are forbidden.

Run the same check without building C++ through either interface:

```sh
cmake --build <build-directory> --target verify_source_policy
ctest --test-dir <build-directory> -R "^source_policy$" --output-on-failure
```

For a build-tree-independent check, run:

```sh
cmake -DVE_SOURCE_DIR=<repository-root> -P cmake/VerifySourcePolicy.cmake
```
