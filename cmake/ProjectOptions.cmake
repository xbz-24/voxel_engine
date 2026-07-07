option(VE_BUILD_API_DOCS "Add the optional Doxygen API documentation target" OFF)
option(VE_BUILD_EXAMPLES "Build public API example applications" ON)
option(VE_BUILD_TESTS "Build engine tests" ON)
option(VE_BUILD_RUNTIME_SMOKE_TESTS "Add short voxel_engine launch tests that exit after a few frames" OFF)
option(VE_ENABLE_STRICT_WARNINGS "Enable the project's extended compiler warning set" ON)
option(VE_ENABLE_PRECOMPILED_HEADERS "Use common project precompiled headers to shorten C++ rebuilds" ON)
option(VE_ENABLE_ASAN "Build supported targets with AddressSanitizer instrumentation" OFF)
option(VE_ENABLE_UBSAN "Build supported targets with UndefinedBehaviorSanitizer instrumentation" OFF)
option(VE_WARNINGS_AS_ERRORS "Treat project warnings as compilation errors" ON)
option(VE_USE_LATEST_CXX "Use the newest C++ language mode exposed by the compiler" ON)
set(VE_IMGUI_VULKAN_BACKEND_SOURCE "" CACHE FILEPATH "Path to imgui_impl_vulkan.cpp")
set(VE_RUNTIME_SMOKE_FRAMES "3" CACHE STRING "Frame count used by optional runtime smoke tests")

add_library(ve_project_options INTERFACE)
target_compile_features(ve_project_options INTERFACE cxx_std_23)
# Compiler matrix presets for MSVC, ClangCL, and Linux GCC are defined in CMakePresets.json.

function(ve_enable_common_pch target)
    if (NOT VE_ENABLE_PRECOMPILED_HEADERS)
        return()
    endif()
    target_precompile_headers(${target} PRIVATE
        <algorithm>
        <array>
        <cassert>
        <cmath>
        <cstddef>
        <cstdint>
        <filesystem>
        <memory>
        <optional>
        <span>
        <string>
        <string_view>
        <unordered_map>
        <utility>
        <vector>
        <glm/glm.hpp>
    )
endfunction()

include("${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake")
include("${PROJECT_SOURCE_DIR}/cmake/Sanitizers.cmake")
