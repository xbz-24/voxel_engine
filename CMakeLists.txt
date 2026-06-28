cmake_minimum_required(VERSION 3.25)

project(VoxelEngine LANGUAGES C CXX)

# TODO: Add install/export package targets once voxel_engine_sdk no longer exports private engine target dependencies.
# TODO: Add a standalone SDK consumer smoke project that builds against an installed tree instead of this source checkout.
# TODO: Split editor/demo-only dependencies from the public SDK so minimal users do not inherit the full application stack.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(VE_BUILD_API_DOCS "Add the optional Doxygen API documentation target" OFF)
option(VE_BUILD_EXAMPLES "Build public API example applications" ON)
option(VE_BUILD_TESTS "Build engine tests" ON)
option(VE_BUILD_RUNTIME_SMOKE_TESTS "Add short voxel_engine launch tests that exit after a few frames" OFF)
option(VE_ENABLE_STRICT_WARNINGS "Enable the project's extended compiler warning set" ON)
option(VE_ENABLE_ASAN "Build supported targets with AddressSanitizer instrumentation" OFF)
option(VE_ENABLE_UBSAN "Build supported targets with UndefinedBehaviorSanitizer instrumentation" OFF)
option(VE_WARNINGS_AS_ERRORS "Treat project warnings as compilation errors" ON)
option(VE_USE_LATEST_CXX "Use the newest C++ language mode exposed by the compiler" ON)
set(VE_IMGUI_VULKAN_BACKEND_SOURCE "" CACHE FILEPATH "Path to imgui_impl_vulkan.cpp")
set(VE_RUNTIME_SMOKE_FRAMES "3" CACHE STRING "Frame count used by optional runtime smoke tests")

add_library(ve_project_options INTERFACE)
target_compile_features(ve_project_options INTERFACE cxx_std_23)
# TODO: Add compiler matrix CI presets for MSVC, ClangCL, and one non-Windows toolchain before treating the SDK as portable.

if (MSVC)
    target_compile_options(ve_project_options INTERFACE
        /MP
        /permissive-
        /Zc:__cplusplus
        /Zc:preprocessor
        /Zc:throwingNew
        /Zc:inline
        /EHsc
        /utf-8
        /diagnostics:caret
        /external:anglebrackets
        /external:W0
        /sdl
    )
    if (VE_USE_LATEST_CXX)
        target_compile_options(ve_project_options INTERFACE /std:c++latest)
    endif()
    if (VE_ENABLE_STRICT_WARNINGS)
        target_compile_options(ve_project_options INTERFACE
            /Wall
            /wd4514
            /wd4625
            /wd4626
            /wd4710
            /wd4711
            /wd4820
            /wd5026
            /wd5027
            /wd5045
            /w14242
            /w14254
            /w14263
            /w14265
            /w14287
            /we4289
            /w14296
            /w14311
            /w14545
            /w14546
            /w14547
            /w14549
            /w14555
            /w14619
            /w14640
            /w14826
            /w14905
            /w14906
            /w14928
        )
    endif()
    if (VE_WARNINGS_AS_ERRORS)
        target_compile_options(ve_project_options INTERFACE /WX)
    endif()
else()
    if (VE_USE_LATEST_CXX)
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 18)
            target_compile_options(ve_project_options INTERFACE -std=c++2c)
        elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14)
            target_compile_options(ve_project_options INTERFACE -std=c++2c)
        endif()
    endif()
    if (VE_ENABLE_STRICT_WARNINGS)
        target_compile_options(ve_project_options INTERFACE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Woverloaded-virtual
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
        )
    endif()
    if (VE_WARNINGS_AS_ERRORS)
        target_compile_options(ve_project_options INTERFACE -Werror)
    endif()
endif()

if (VE_ENABLE_ASAN)
    target_compile_definitions(ve_project_options INTERFACE VE_ADDRESS_SANITIZER=1)
    if (MSVC)
        target_compile_options(ve_project_options INTERFACE /fsanitize=address /Zi /Oy-)
        target_link_options(ve_project_options INTERFACE /INCREMENTAL:NO)
    else()
        target_compile_options(ve_project_options INTERFACE -fsanitize=address -fno-omit-frame-pointer)
        target_link_options(ve_project_options INTERFACE -fsanitize=address)
    endif()
endif()

if (VE_ENABLE_UBSAN)
    if (MSVC)
        message(WARNING "VE_ENABLE_UBSAN is not supported by MSVC and will be ignored.")
    else()
        target_compile_definitions(ve_project_options INTERFACE VE_UNDEFINED_BEHAVIOR_SANITIZER=1)
        target_compile_options(ve_project_options INTERFACE -fsanitize=undefined -fno-omit-frame-pointer)
        target_link_options(ve_project_options INTERFACE -fsanitize=undefined)
    endif()
endif()

find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS container)
find_package(doctest CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(xsimd CONFIG REQUIRED)
find_program(VE_GLSLC_EXECUTABLE glslc
    HINTS
        "$ENV{VULKAN_SDK}/Bin"
        "C:/VulkanSDK/1.4.350.0/Bin"
)
if (NOT VE_GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc was not found; install the Vulkan SDK or add glslc to PATH.")
endif()
find_package(Stb QUIET)
if (NOT Stb_FOUND)
    find_path(Stb_INCLUDE_DIR stb_image.h PATHS ${CMAKE_SOURCE_DIR} REQUIRED)
endif()

# Third-party usage requirements live in one place so subsystem targets stay small.
add_library(ve_dependencies INTERFACE)
target_compile_definitions(ve_dependencies INTERFACE
    _WIN32_WINNT=0x0A00
    NOMINMAX
    ROOT_DIR="${CMAKE_SOURCE_DIR}"
)

set(VE_SOURCE_ROOT "${CMAKE_SOURCE_DIR}/src")
set(VE_PUBLIC_INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include")

function(ve_collect_subsystem directory prefix)
    file(GLOB subsystem_sources CONFIGURE_DEPENDS "${directory}/*.cpp")
    file(GLOB subsystem_headers CONFIGURE_DEPENDS "${directory}/*.h" "${directory}/*.inl")
    list(SORT subsystem_sources)
    list(SORT subsystem_headers)
    set(${prefix}_SOURCES ${subsystem_sources} PARENT_SCOPE)
    set(${prefix}_HEADERS ${subsystem_headers} PARENT_SCOPE)
endfunction()

ve_collect_subsystem("${VE_SOURCE_ROOT}/core" VE_CORE)
ve_collect_subsystem("${VE_SOURCE_ROOT}/render" VE_RENDER)
ve_collect_subsystem("${VE_SOURCE_ROOT}/render/backends" VE_RENDER_BACKENDS)
ve_collect_subsystem("${VE_SOURCE_ROOT}/world" VE_WORLD)
ve_collect_subsystem("${VE_SOURCE_ROOT}/network" VE_NETWORK)
ve_collect_subsystem("${VE_SOURCE_ROOT}/app" VE_APP)

function(ve_add_subsystem target folder directory)
    cmake_parse_arguments(SUBSYSTEM "" "" "SOURCES;HEADERS" ${ARGN})
    add_library(${target} STATIC ${SUBSYSTEM_SOURCES} ${SUBSYSTEM_HEADERS})
    target_include_directories(${target} PUBLIC ${directory})
    target_link_libraries(${target} PUBLIC ve_dependencies PRIVATE ve_project_options)
    set_target_properties(${target} PROPERTIES FOLDER "Engine/${folder}")
    source_group(TREE ${directory} FILES ${SUBSYSTEM_SOURCES} ${SUBSYSTEM_HEADERS})
endfunction()

function(ve_copy_msvc_asan_runtime target)
    if (NOT (VE_ENABLE_ASAN AND MSVC))
        return()
    endif()
    get_filename_component(VE_MSVC_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    file(GLOB VE_MSVC_ASAN_RUNTIME_DLLS CONFIGURE_DEPENDS
        "${VE_MSVC_BIN_DIR}/clang_rt.asan*_dynamic-x86_64.dll"
    )
    if (NOT VE_MSVC_ASAN_RUNTIME_DLLS)
        message(WARNING "MSVC AddressSanitizer runtime DLLs were not found next to ${CMAKE_CXX_COMPILER}.")
        return()
    endif()
    foreach(asan_runtime_dll IN LISTS VE_MSVC_ASAN_RUNTIME_DLLS)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${asan_runtime_dll}"
                "$<TARGET_FILE_DIR:${target}>"
            VERBATIM
        )
    endforeach()
endfunction()

ve_add_subsystem(ve_core Core "${VE_SOURCE_ROOT}/core" SOURCES ${VE_CORE_SOURCES} HEADERS ${VE_CORE_HEADERS})
ve_add_subsystem(ve_render Render "${VE_SOURCE_ROOT}/render" SOURCES ${VE_RENDER_SOURCES} HEADERS ${VE_RENDER_HEADERS})
ve_add_subsystem(ve_render_backends Render "${VE_SOURCE_ROOT}/render/backends" SOURCES ${VE_RENDER_BACKENDS_SOURCES} HEADERS ${VE_RENDER_BACKENDS_HEADERS})
ve_add_subsystem(ve_world World "${VE_SOURCE_ROOT}/world" SOURCES ${VE_WORLD_SOURCES} HEADERS ${VE_WORLD_HEADERS})
ve_add_subsystem(ve_network Network "${VE_SOURCE_ROOT}/network" SOURCES ${VE_NETWORK_SOURCES} HEADERS ${VE_NETWORK_HEADERS})
ve_add_subsystem(ve_app App "${VE_SOURCE_ROOT}/app" SOURCES ${VE_APP_SOURCES} HEADERS ${VE_APP_HEADERS})

add_library(voxel_engine_sdk STATIC
    "${VE_SOURCE_ROOT}/api/EngineConfigTranslator.cpp"
    "${VE_SOURCE_ROOT}/api/EngineConfigValidation.cpp"
    "${VE_SOURCE_ROOT}/api/Engine.cpp"
    "${VE_SOURCE_ROOT}/api/EngineRuntimeBridge.cpp"
    "${VE_SOURCE_ROOT}/api/EngineRuntimeBridge.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Assets.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Blocks.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Camera.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Engine.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Materials.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Runtime.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Scene.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Voxel.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/World.h"
)
target_include_directories(voxel_engine_sdk PUBLIC "${VE_PUBLIC_INCLUDE_ROOT}")
target_link_libraries(voxel_engine_sdk PRIVATE ve_project_options ve_app)
set_target_properties(voxel_engine_sdk PROPERTIES FOLDER "Engine/API")
add_library(VoxelEngine::SDK ALIAS voxel_engine_sdk)

set(VE_VULKAN_SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/generated/shaders/vulkan")
set(VE_VULKAN_SHADER_SOURCES
    "${CMAKE_SOURCE_DIR}/shaders/vulkan/voxel_chunk.vert"
    "${CMAKE_SOURCE_DIR}/shaders/vulkan/voxel_chunk.frag"
)
set(VE_VULKAN_SHADER_OUTPUTS)
foreach(shader_source IN LISTS VE_VULKAN_SHADER_SOURCES)
    get_filename_component(shader_name "${shader_source}" NAME)
    set(shader_output "${VE_VULKAN_SHADER_OUTPUT_DIR}/${shader_name}.spv")
    add_custom_command(
        OUTPUT "${shader_output}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${VE_VULKAN_SHADER_OUTPUT_DIR}"
        COMMAND "${VE_GLSLC_EXECUTABLE}" "${shader_source}" -o "${shader_output}"
        DEPENDS "${shader_source}"
        COMMENT "Compiling Vulkan shader ${shader_name}"
        VERBATIM
    )
    list(APPEND VE_VULKAN_SHADER_OUTPUTS "${shader_output}")
endforeach()
add_custom_target(ve_vulkan_shaders DEPENDS ${VE_VULKAN_SHADER_OUTPUTS})
set_target_properties(ve_vulkan_shaders PROPERTIES FOLDER "Engine/Render")
add_dependencies(ve_render_backends ve_vulkan_shaders)
target_compile_definitions(ve_render_backends PRIVATE VE_VULKAN_SHADER_DIR="${VE_VULKAN_SHADER_OUTPUT_DIR}")

target_link_libraries(ve_render PUBLIC ve_core)
target_link_libraries(ve_render_backends PUBLIC ve_core ve_render ve_world)
target_link_libraries(ve_world PUBLIC ve_core ve_render)
target_link_libraries(ve_network PUBLIC ve_core ve_world)
target_link_libraries(ve_app PUBLIC ve_core ve_render ve_render_backends ve_world ve_network)

target_link_libraries(ve_core PUBLIC
    glm::glm
    glfw
    spdlog::spdlog
    xsimd
)

target_include_directories(ve_render SYSTEM PUBLIC ${Stb_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})
target_link_libraries(ve_render PUBLIC
    GLEW::GLEW
    glm::glm
    assimp::assimp
    volk::volk_headers
    Vulkan::Headers
)

target_link_libraries(ve_render_backends PUBLIC
    Boost::container
    GLEW::GLEW
    glfw
    imgui::imgui
    volk::volk
    volk::volk_headers
    Vulkan::Headers
    GPUOpen::VulkanMemoryAllocator
)

if (CMAKE_TOOLCHAIN_FILE)
    if (NOT VE_IMGUI_VULKAN_BACKEND_SOURCE)
        get_filename_component(VE_VCPKG_BUILDSYSTEM_DIR "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
        get_filename_component(VE_VCPKG_SCRIPTS_DIR "${VE_VCPKG_BUILDSYSTEM_DIR}" DIRECTORY)
        get_filename_component(VE_VCPKG_ROOT "${VE_VCPKG_SCRIPTS_DIR}" DIRECTORY)
        file(GLOB VE_IMGUI_BACKEND_DIRS CONFIGURE_DEPENDS "${VE_VCPKG_ROOT}/buildtrees/imgui/src/*/backends")
        find_file(VE_IMGUI_VULKAN_BACKEND_SOURCE imgui_impl_vulkan.cpp PATHS ${VE_IMGUI_BACKEND_DIRS} NO_DEFAULT_PATH)
    endif()
    if (VE_IMGUI_VULKAN_BACKEND_SOURCE)
        get_filename_component(VE_IMGUI_VULKAN_BACKEND_DIR "${VE_IMGUI_VULKAN_BACKEND_SOURCE}" DIRECTORY)
        target_sources(ve_render_backends PRIVATE "${VE_IMGUI_VULKAN_BACKEND_SOURCE}")
        target_include_directories(ve_render_backends SYSTEM PRIVATE "${VE_IMGUI_VULKAN_BACKEND_DIR}")
        target_compile_definitions(ve_render_backends PRIVATE IMGUI_IMPL_VULKAN_USE_VOLK)
        set_source_files_properties("${VE_IMGUI_VULKAN_BACKEND_SOURCE}" PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
        if (MSVC)
            set_source_files_properties("${VE_IMGUI_VULKAN_BACKEND_SOURCE}" PROPERTIES COMPILE_OPTIONS "/W0")
        endif()
    else()
        message(FATAL_ERROR "imgui_impl_vulkan.cpp was not found in vcpkg buildtrees; run CMake after vcpkg restores imgui.")
    endif()
endif()

target_link_libraries(ve_world PUBLIC
    glm::glm
)

target_link_libraries(ve_network PUBLIC
    asio::asio
    ws2_32
)

target_link_libraries(ve_app PUBLIC
    GLEW::GLEW
    glfw
    glm::glm
    imgui::imgui
)

set(VE_LAUNCHER_SOURCES
    "${VE_SOURCE_ROOT}/api/CommandLineLauncher.cpp"
    "${VE_SOURCE_ROOT}/api/CommandLineLauncher.h"
)

add_executable(voxel_engine main.cpp ${VE_LAUNCHER_SOURCES} "${VE_SOURCE_ROOT}/app/main.rc")
target_include_directories(voxel_engine PRIVATE "${VE_SOURCE_ROOT}/api")
target_link_libraries(voxel_engine PRIVATE
    ve_project_options
    voxel_engine_sdk
)
set_target_properties(voxel_engine PROPERTIES FOLDER "Applications")
ve_copy_msvc_asan_runtime(voxel_engine)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT voxel_engine)

if (VE_BUILD_EXAMPLES)
    function(ve_add_public_example target source)
        add_executable(${target} "${source}")
        target_link_libraries(${target} PRIVATE
            ve_project_options
            voxel_engine_sdk
        )
        set_target_properties(${target} PROPERTIES FOLDER "Examples")
        ve_copy_msvc_asan_runtime(${target})
    endfunction()

    ve_add_public_example(voxel_example_minimal "${CMAKE_SOURCE_DIR}/examples/minimal.cpp")
    ve_add_public_example(voxel_example_custom_scene "${CMAKE_SOURCE_DIR}/examples/custom_scene.cpp")
    ve_add_public_example(voxel_example_model_demos "${CMAKE_SOURCE_DIR}/examples/model_demos.cpp")
    ve_add_public_example(voxel_example_world_edit "${CMAKE_SOURCE_DIR}/examples/world_edit.cpp")
    ve_add_public_example(voxel_example_showcase "${CMAKE_SOURCE_DIR}/examples/showcase.cpp")
endif()

if (VE_BUILD_RUNTIME_SMOKE_TESTS)
    enable_testing()
    add_test(NAME voxel_engine_runtime_smoke_desert
        COMMAND $<TARGET_FILE:voxel_engine> --demo desert --smoke-frames ${VE_RUNTIME_SMOKE_FRAMES}
    )
endif()

if (VE_BUILD_API_DOCS)
    find_package(Doxygen QUIET)
    if (DOXYGEN_FOUND)
        configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
        add_custom_target(engine_api_docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Generating VoxelEngine API documentation with Doxygen"
            VERBATIM
        )
        set_target_properties(engine_api_docs PROPERTIES FOLDER "Documentation")
    else()
        message(STATUS "Doxygen was not found; API documentation target will be skipped.")
    endif()
endif()

if (VE_BUILD_TESTS)
    enable_testing()
    add_executable(engine_tests
        ${VE_LAUNCHER_SOURCES}
        Tests/EngineSmokeTests.cpp
        Tests/GraphicsCommandTests.cpp
        Tests/GraphicsGeometryTests.cpp
        Tests/NetworkProtocolTests.cpp
        Tests/PublicApiCompileTests.cpp
        Tests/RenderBackendTests.cpp
        Tests/WorldCoordinateTests.cpp
    )
    target_include_directories(engine_tests PRIVATE "${VE_SOURCE_ROOT}/api")
    target_link_libraries(engine_tests PRIVATE
        ve_project_options
        doctest::doctest
        voxel_engine_sdk
        ve_app
        ve_core
        ve_network
        ve_render
        ve_render_backends
        ve_world
    )
    if (MSVC)
        target_compile_options(engine_tests PRIVATE /wd4866)
    endif()
    set_target_properties(engine_tests PROPERTIES FOLDER "Tests")
    ve_copy_msvc_asan_runtime(engine_tests)
    add_test(NAME engine_tests COMMAND engine_tests)
endif()
