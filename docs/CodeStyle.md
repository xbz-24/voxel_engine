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
- Keep engine source files under 100 lines when practical.

## Ownership

- Prefer value ownership for model data.
- Prefer `std::unique_ptr` for OpenGL-backed resources that need delayed construction.
- Prefer move-only outputs for heavy mesh/network payloads.
- Avoid raw owning pointers.
- Use raw pointers only for nullable, non-owning views. Prefer references when null is invalid, `std::span` for contiguous borrowed ranges, and smart pointers only when ownership is transferred or shared.
- Prefer `std::ranges`, constrained templates, and small generic helpers when they remove real duplication without hiding domain names.

## Migration Rule

Existing files can be migrated gradually. New files should follow this guide now.
