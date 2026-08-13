if (NOT VE_RUNTIME_TARGET_GRAPH_FILE OR
    NOT EXISTS "${VE_RUNTIME_TARGET_GRAPH_FILE}")
    message(FATAL_ERROR "ve_runtime dependency manifest was not generated")
endif()

file(STRINGS "${VE_RUNTIME_TARGET_GRAPH_FILE}" runtime_targets)
set(allowed_internal_targets
    ve_runtime
    ve_core
    ve_dependencies
    ve_project_options
    ve_render
    ve_render_backend_window
)
foreach(runtime_target IN LISTS runtime_targets)
    if (runtime_target MATCHES "^ve_" AND
        NOT runtime_target IN_LIST allowed_internal_targets)
        message(FATAL_ERROR
            "ve_runtime dependency manifest contains forbidden target ${runtime_target}")
    endif()
endforeach()

foreach(forbidden_target IN ITEMS ve_world ve_network ve_render_backends)
    if (forbidden_target IN_LIST runtime_targets)
        message(FATAL_ERROR "ve_runtime depends on ${forbidden_target}")
    endif()
endforeach()

message(STATUS "ve_runtime target graph respects the generic host boundary")

if (NOT VE_VOXEL_SANDBOX_TARGET_GRAPH_FILE OR
    NOT EXISTS "${VE_VOXEL_SANDBOX_TARGET_GRAPH_FILE}")
    message(FATAL_ERROR "ve_voxel_sandbox dependency manifest was not generated")
endif()
file(STRINGS "${VE_VOXEL_SANDBOX_TARGET_GRAPH_FILE}" sandbox_targets)
if (ve_network IN_LIST sandbox_targets)
    message(FATAL_ERROR "ve_voxel_sandbox depends on ve_network")
endif()
message(STATUS "ve_voxel_sandbox has no dead ve_network dependency")
