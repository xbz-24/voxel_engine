if (VE_BUILD_TESTS)
    enable_testing()
    add_executable(engine_tests
        ${VE_LAUNCHER_SOURCES}
        Tests/AssetPathTests.cpp
        Tests/EngineSmokeTests.cpp
        Tests/FrameGraphTests.cpp
        Tests/GraphicsCommandTests.cpp
        Tests/GraphicsGeometryTests.cpp
        Tests/NetworkProtocolTests.cpp
        Tests/PublicApiCompileTests.cpp
        Tests/RenderBackendTests.cpp
        Tests/WindowTests.cpp
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
    ve_enable_common_pch(engine_tests)
    set_target_properties(engine_tests PROPERTIES FOLDER "Tests")
    ve_copy_msvc_asan_runtime(engine_tests)
    add_test(NAME engine_tests COMMAND engine_tests)

    add_executable(public_api_header_smoke Tests/PublicApiHeaderSmoke.cpp)
    target_link_libraries(public_api_header_smoke PRIVATE
        ve_project_options
        voxel_engine_sdk
    )
    ve_enable_common_pch(public_api_header_smoke)
    set_target_properties(public_api_header_smoke PROPERTIES FOLDER "Tests")
    ve_copy_msvc_asan_runtime(public_api_header_smoke)
    add_test(NAME public_api_header_smoke COMMAND public_api_header_smoke)

    add_library(public_api_headers_only_smoke OBJECT Tests/PublicApiHeadersOnlySmoke.cpp)
    target_include_directories(public_api_headers_only_smoke PRIVATE "${VE_PUBLIC_INCLUDE_ROOT}")
    target_link_libraries(public_api_headers_only_smoke PRIVATE ve_project_options)
    set_target_properties(public_api_headers_only_smoke PROPERTIES FOLDER "Tests")

    set(VE_PUBLIC_HEADER_SELF_CONTAINED_SOURCES)
    foreach(public_header IN LISTS VE_PUBLIC_SDK_HEADERS)
        file(RELATIVE_PATH public_header_include_path "${VE_PUBLIC_INCLUDE_ROOT}" "${public_header}")
        string(REPLACE "\\" "/" public_header_include_path "${public_header_include_path}")
        string(MAKE_C_IDENTIFIER "${public_header_include_path}" public_header_anchor)
        set(public_header_smoke_source
            "${CMAKE_CURRENT_BINARY_DIR}/generated/public_header_self_contained/${public_header_anchor}.cpp")
        file(WRITE "${public_header_smoke_source}"
            "#include <${public_header_include_path}>\n"
            "int ${public_header_anchor}_self_contained_smoke() { return 0; }\n")
        list(APPEND VE_PUBLIC_HEADER_SELF_CONTAINED_SOURCES "${public_header_smoke_source}")
    endforeach()

    add_library(public_api_header_self_contained_smoke OBJECT ${VE_PUBLIC_HEADER_SELF_CONTAINED_SOURCES})
    target_include_directories(public_api_header_self_contained_smoke PRIVATE "${VE_PUBLIC_INCLUDE_ROOT}")
    target_link_libraries(public_api_header_self_contained_smoke PRIVATE ve_project_options)
    set_target_properties(public_api_header_self_contained_smoke PROPERTIES FOLDER "Tests")

    add_dependencies(public_api_header_smoke
        public_api_headers_only_smoke
        public_api_header_self_contained_smoke
    )
endif()
