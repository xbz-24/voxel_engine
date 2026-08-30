function(ve_verify_public_authoring_target target)
    ve_collect_target_closure(${target} authoring_target_closure)
    set(forbidden_targets
        ve_app
        ve_core
        ve_network
        ve_render
        ve_render_backends
        ve_render_backend_factory
        ve_render_backend_opengl
        ve_render_backend_vulkan
        ve_render_backend_window
        ve_runtime
        ve_voxel_sandbox
        ve_world
        voxel_engine_sdk
    )
    foreach(forbidden_target IN LISTS forbidden_targets)
        if (forbidden_target IN_LIST authoring_target_closure)
            message(FATAL_ERROR
                "${target} crossed its authoring boundary through ${forbidden_target}")
        endif()
    endforeach()

    if (NOT ve_project_options IN_LIST authoring_target_closure)
        message(FATAL_ERROR "${target} lost the common project compile options")
    endif()

    set(graph_file "${CMAKE_CURRENT_BINARY_DIR}/public_authoring_target_graph.txt")
    string(REPLACE ";" "\n" graph_text "${authoring_target_closure}")
    file(WRITE "${graph_file}" "${target}\n${graph_text}\n")
endfunction()

cmake_language(DEFER CALL ve_verify_public_authoring_target voxel_engine_authoring)
