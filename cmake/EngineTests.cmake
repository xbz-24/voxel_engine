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
    set_target_properties(engine_tests PROPERTIES FOLDER "Tests")
    ve_copy_msvc_asan_runtime(engine_tests)
    add_test(NAME engine_tests COMMAND engine_tests)

    add_executable(public_api_header_smoke Tests/PublicApiHeaderSmoke.cpp)
    target_link_libraries(public_api_header_smoke PRIVATE
        ve_project_options
        voxel_engine_sdk
    )
    set_target_properties(public_api_header_smoke PROPERTIES FOLDER "Tests")
    ve_copy_msvc_asan_runtime(public_api_header_smoke)
    add_test(NAME public_api_header_smoke COMMAND public_api_header_smoke)

    add_library(public_api_headers_only_smoke OBJECT Tests/PublicApiHeadersOnlySmoke.cpp)
    target_include_directories(public_api_headers_only_smoke PRIVATE "${VE_PUBLIC_INCLUDE_ROOT}")
    target_link_libraries(public_api_headers_only_smoke PRIVATE ve_project_options)
    set_target_properties(public_api_headers_only_smoke PROPERTIES FOLDER "Tests")
endif()
