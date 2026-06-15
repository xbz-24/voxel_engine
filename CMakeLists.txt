cmake_minimum_required(VERSION 3.25)

project(VoxelEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(VE_BUILD_API_DOCS "Add the optional Doxygen API documentation target" OFF)
option(VE_BUILD_TESTS "Build engine tests" ON)

find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(xsimd CONFIG REQUIRED)
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
target_include_directories(ve_dependencies INTERFACE
    ${CMAKE_SOURCE_DIR}/Builds
    ${Stb_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
)
target_link_libraries(ve_dependencies INTERFACE
    GLEW::GLEW
    glfw
    glm::glm
    assimp::assimp
    asio::asio
    imgui::imgui
    spdlog::spdlog
    volk::volk
    volk::volk_headers
    Vulkan::Headers
    GPUOpen::VulkanMemoryAllocator
    xsimd
    ws2_32
)

# CONFIGURE_DEPENDS keeps newly added implementation files visible after CMake
# regenerates. The classification below is the temporary boundary while files
# are moved from Builds/ into feature directories.
file(GLOB VE_ALL_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Builds/*.cpp")

file(GLOB VE_CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/Builds/BackgroundTask*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Ecs*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Entity*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Fabrik*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Fluid*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/FrameTimer.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/InverseKinematics*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/LinearMemory*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Log*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Memory*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/NavierStokes*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Physics*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/RigidBody*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Simd*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/WorkerPolicy.cpp"
)

file(GLOB VE_NETWORK_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/Builds/Multiplayer*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Network*.cpp"
)

file(GLOB VE_WORLD_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/Builds/AsyncChunk*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/AsyncWorld*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Block*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Chunk*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/ComputeTerrain*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/GreedyChunk*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/LevelSpawn.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/World*.cpp"
)

file(GLOB VE_RENDER_BACKEND_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/Builds/DirectX12Backend.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/OpenGLCompatibilityBackend.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Vulkan*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/WindowRuntime.cpp"
)

file(GLOB VE_RENDER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/Builds/AmbientOcclusion*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Assimp*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Camera.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/CascadedShadow*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/DeferredRenderer.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/FrameGraph*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Frustum.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/GeometryFactory.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Gpu*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Graphics*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/MonteCarlo*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/MortonCode.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/OpenGLRender*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/PathTracing*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/PbrMaterial.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Plane.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Render*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/SkyBox.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/TemporalAntiAliasing*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/TextureLoader.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/VolumetricRaymarcher.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/Voxel*.cpp"
    "${CMAKE_SOURCE_DIR}/Builds/dependencies.cpp"
)

set(VE_APP_SOURCES ${VE_ALL_SOURCES})
list(REMOVE_ITEM VE_APP_SOURCES
    ${VE_CORE_SOURCES}
    ${VE_NETWORK_SOURCES}
    ${VE_WORLD_SOURCES}
    ${VE_RENDER_BACKEND_SOURCES}
    ${VE_RENDER_SOURCES}
)

function(ve_add_subsystem target folder)
    add_library(${target} STATIC ${ARGN})
    target_link_libraries(${target} PUBLIC ve_dependencies)
    set_target_properties(${target} PROPERTIES FOLDER "Engine/${folder}")
    source_group(TREE "${CMAKE_SOURCE_DIR}/Builds" PREFIX "Source Files" FILES ${ARGN})
endfunction()

ve_add_subsystem(ve_core Core ${VE_CORE_SOURCES})
ve_add_subsystem(ve_render Render ${VE_RENDER_SOURCES})
ve_add_subsystem(ve_render_backends Render ${VE_RENDER_BACKEND_SOURCES})
ve_add_subsystem(ve_world World ${VE_WORLD_SOURCES})
ve_add_subsystem(ve_network Network ${VE_NETWORK_SOURCES})
ve_add_subsystem(ve_app App ${VE_APP_SOURCES})

add_executable(voxel_engine main.cpp)
target_link_libraries(voxel_engine PRIVATE
    ve_app
    ve_world
    ve_network
    ve_render
    ve_render_backends
    ve_core
)
set_target_properties(voxel_engine PROPERTIES FOLDER "Applications")

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
        Tests/EngineSmokeTests.cpp
        Tests/GraphicsCommandTests.cpp
        Tests/GraphicsGeometryTests.cpp
        Tests/RenderBackendTests.cpp
    )
    target_link_libraries(engine_tests PRIVATE
        doctest::doctest
        ve_core
        ve_render
        ve_render_backends
    )
    set_target_properties(engine_tests PROPERTIES FOLDER "Tests")
    add_test(NAME engine_tests COMMAND engine_tests)
endif()
