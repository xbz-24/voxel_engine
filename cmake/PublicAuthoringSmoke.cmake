if (VE_BUILD_TESTS)
    add_executable(public_authoring_link_smoke
        "${PROJECT_SOURCE_DIR}/Tests/PublicAuthoringLinkSmoke.cpp"
    )
    target_link_libraries(public_authoring_link_smoke PRIVATE
        ve_project_options
        voxel_engine_authoring
    )
    ve_enable_common_pch(public_authoring_link_smoke)
    set_target_properties(public_authoring_link_smoke PROPERTIES FOLDER "Tests")
    ve_copy_msvc_asan_runtime(public_authoring_link_smoke)
    add_test(NAME public_authoring_link_smoke COMMAND public_authoring_link_smoke)
    set_tests_properties(public_authoring_link_smoke PROPERTIES LABELS "api;authoring")
endif()
