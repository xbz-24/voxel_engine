function(ve_verify_public_sdk_target target)
    ve_collect_target_closure(${target} sdk_target_closure)
    set(expected_internal_targets
        ve_core
        ve_dependencies
        ve_project_options
        ve_render
        ve_render_backend_factory
        ve_render_backend_opengl
        ve_render_backend_vulkan
        ve_render_backend_window
        ve_render_backends
        ve_runtime
        ve_voxel_sandbox
        ve_world
        voxel_engine_authoring
    )
    set(actual_internal_targets)
    foreach(dependency IN LISTS sdk_target_closure)
        if (dependency MATCHES "^ve_" OR
                dependency STREQUAL "voxel_engine_authoring")
            list(APPEND actual_internal_targets "${dependency}")
        endif()
    endforeach()
    list(REMOVE_DUPLICATES actual_internal_targets)
    list(SORT actual_internal_targets)
    list(SORT expected_internal_targets)
    if (NOT actual_internal_targets STREQUAL expected_internal_targets)
        message(FATAL_ERROR
            "VoxelEngine::SDK internal closure changed. Expected "
            "${expected_internal_targets}; found ${actual_internal_targets}")
    endif()
    if (ve_network IN_LIST sdk_target_closure)
        message(FATAL_ERROR "VoxelEngine::SDK must not regain ve_network")
    endif()
    set(graph_file "${CMAKE_CURRENT_BINARY_DIR}/public_sdk_target_graph.txt")
    string(REPLACE ";" "\n" graph_text "${sdk_target_closure}")
    file(WRITE "${graph_file}" "${target}\n${graph_text}\n")
endfunction()

cmake_language(DEFER CALL ve_verify_public_sdk_target voxel_engine_sdk)
