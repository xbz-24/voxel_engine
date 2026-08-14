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
endif()
