if (VE_BUILD_EXAMPLES)
    function(ve_add_public_example target source)
        add_executable(${target} "${source}")
        target_link_libraries(${target} PRIVATE
            ve_project_options
            voxel_engine_sdk
        )
        ve_enable_common_pch(${target})
        set_target_properties(${target} PROPERTIES FOLDER "Examples")
        ve_copy_msvc_asan_runtime(${target})
    endfunction()

    ve_add_public_example(voxel_example_minimal "${PROJECT_SOURCE_DIR}/examples/minimal.cpp")
    ve_add_public_example(voxel_example_custom_scene "${PROJECT_SOURCE_DIR}/examples/custom_scene.cpp")
    ve_add_public_example(voxel_example_world_edit "${PROJECT_SOURCE_DIR}/examples/world_edit.cpp")
    ve_add_public_example(voxel_example_asset_catalog "${PROJECT_SOURCE_DIR}/examples/asset_catalog.cpp")
    ve_add_public_example(voxel_example_scene_graph "${PROJECT_SOURCE_DIR}/examples/scene_graph.cpp")
    ve_add_public_example(voxel_example_runtime_commands "${PROJECT_SOURCE_DIR}/examples/runtime_commands.cpp")
    ve_add_public_example(voxel_example_runtime_callbacks "${PROJECT_SOURCE_DIR}/examples/runtime_callbacks.cpp")

    set(VE_PUBLIC_EXAMPLE_TARGETS
        voxel_example_minimal
        voxel_example_custom_scene
        voxel_example_world_edit
        voxel_example_asset_catalog
        voxel_example_scene_graph
        voxel_example_runtime_commands
        voxel_example_runtime_callbacks
    )
    add_custom_target(voxel_examples)
    add_dependencies(voxel_examples ${VE_PUBLIC_EXAMPLE_TARGETS})
    set_target_properties(voxel_examples PROPERTIES FOLDER "Examples")

    if (VE_BUILD_TESTS)
        enable_testing()
        foreach(headless_example IN ITEMS
                voxel_example_asset_catalog
                voxel_example_scene_graph
                voxel_example_runtime_commands)
            add_test(NAME ${headless_example} COMMAND $<TARGET_FILE:${headless_example}>)
            set_tests_properties(${headless_example} PROPERTIES
                LABELS "api;examples;headless"
                TIMEOUT 30
            )
        endforeach()
    endif()
endif()
