cmake_minimum_required(VERSION 3.25)

project(VoxelEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(VE_BUILD_API_DOCS "Add the optional Doxygen API documentation target" OFF)
option(VE_BUILD_TESTS "Build engine tests" ON)
option(VE_ENABLE_STRICT_WARNINGS "Enable the project's extended compiler warning set" ON)
option(VE_WARNINGS_AS_ERRORS "Treat project warnings as compilation errors" ON)
option(VE_USE_LATEST_CXX "Use the newest C++ language mode exposed by the compiler" ON)

add_library(ve_project_options INTERFACE)
target_compile_features(ve_project_options INTERFACE cxx_std_23)

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

function(ve_collect_subsystem directory prefix)
    file(GLOB subsystem_sources CONFIGURE_DEPENDS "${directory}/*.cpp")
    file(GLOB subsystem_headers CONFIGURE_DEPENDS "${directory}/*.h" "${directory}/*.inl")
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

ve_add_subsystem(ve_core Core "${VE_SOURCE_ROOT}/core" SOURCES ${VE_CORE_SOURCES} HEADERS ${VE_CORE_HEADERS})
ve_add_subsystem(ve_render Render "${VE_SOURCE_ROOT}/render" SOURCES ${VE_RENDER_SOURCES} HEADERS ${VE_RENDER_HEADERS})
ve_add_subsystem(ve_render_backends Render "${VE_SOURCE_ROOT}/render/backends" SOURCES ${VE_RENDER_BACKENDS_SOURCES} HEADERS ${VE_RENDER_BACKENDS_HEADERS})
ve_add_subsystem(ve_world World "${VE_SOURCE_ROOT}/world" SOURCES ${VE_WORLD_SOURCES} HEADERS ${VE_WORLD_HEADERS})
ve_add_subsystem(ve_network Network "${VE_SOURCE_ROOT}/network" SOURCES ${VE_NETWORK_SOURCES} HEADERS ${VE_NETWORK_HEADERS})
ve_add_subsystem(ve_app App "${VE_SOURCE_ROOT}/app" SOURCES ${VE_APP_SOURCES} HEADERS ${VE_APP_HEADERS})

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
    volk::volk
    volk::volk_headers
    Vulkan::Headers
    GPUOpen::VulkanMemoryAllocator
)

target_link_libraries(ve_world PUBLIC
    GLEW::GLEW
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

add_executable(voxel_engine main.cpp "${VE_SOURCE_ROOT}/app/main.rc")
target_link_libraries(voxel_engine PRIVATE
    ve_project_options
    ve_app
    ve_world
    ve_network
    ve_render
    ve_render_backends
    ve_core
)
set_target_properties(voxel_engine PROPERTIES FOLDER "Applications")
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT voxel_engine)

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
        Tests/NetworkProtocolTests.cpp
        Tests/RenderBackendTests.cpp
        Tests/WorldCoordinateTests.cpp
    )
    target_link_libraries(engine_tests PRIVATE
        ve_project_options
        doctest::doctest
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
    add_test(NAME engine_tests COMMAND engine_tests)
endif()
