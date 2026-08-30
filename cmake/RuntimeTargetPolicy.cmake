function(ve_collect_target_closure root_target output_variable)
    set(pending_targets "${root_target}")
    set(visited_targets)
    while(pending_targets)
        list(POP_FRONT pending_targets current_target)
        if (NOT TARGET "${current_target}")
            continue()
        endif()
        get_target_property(aliased_target "${current_target}" ALIASED_TARGET)
        if (aliased_target)
            set(current_target "${aliased_target}")
        endif()
        if (current_target IN_LIST visited_targets)
            continue()
        endif()
        list(APPEND visited_targets "${current_target}")
        foreach(link_property IN ITEMS LINK_LIBRARIES INTERFACE_LINK_LIBRARIES)
            get_target_property(target_links "${current_target}" "${link_property}")
            if (NOT target_links)
                continue()
            endif()
            foreach(link_item IN LISTS target_links)
                set(link_target "${link_item}")
                if (link_target MATCHES "^\\$<LINK_ONLY:([^>]+)>$")
                    set(link_target "${CMAKE_MATCH_1}")
                endif()
                if (TARGET "${link_target}")
                    list(APPEND pending_targets "${link_target}")
                endif()
                string(REGEX MATCHALL "ve_[A-Za-z0-9_]+" internal_link_targets "${link_item}")
                foreach(internal_link_target IN LISTS internal_link_targets)
                    if (TARGET "${internal_link_target}")
                        list(APPEND pending_targets "${internal_link_target}")
                    endif()
                endforeach()
            endforeach()
        endforeach()
    endwhile()
    list(REMOVE_ITEM visited_targets "${root_target}")
    list(REMOVE_DUPLICATES visited_targets)
    list(SORT visited_targets)
    set(${output_variable} "${visited_targets}" PARENT_SCOPE)
endfunction()

ve_collect_target_closure(ve_runtime VE_RUNTIME_TARGET_CLOSURE)
set(VE_RUNTIME_ALLOWED_INTERNAL_TARGETS
    ve_core
    ve_dependencies
    ve_project_options
    ve_render
    ve_render_backend_window
)
foreach(runtime_dependency IN LISTS VE_RUNTIME_TARGET_CLOSURE)
    if (runtime_dependency MATCHES "^ve_" AND
        NOT runtime_dependency IN_LIST VE_RUNTIME_ALLOWED_INTERNAL_TARGETS)
        message(FATAL_ERROR
            "ve_runtime crossed its generic host boundary through ${runtime_dependency}")
    endif()
endforeach()

ve_collect_target_closure(ve_voxel_sandbox VE_VOXEL_SANDBOX_TARGET_CLOSURE)
if (ve_network IN_LIST VE_VOXEL_SANDBOX_TARGET_CLOSURE)
    message(FATAL_ERROR "ve_voxel_sandbox retains its dead ve_network dependency")
endif()

set(VE_RUNTIME_TARGET_GRAPH_FILE "${CMAKE_CURRENT_BINARY_DIR}/runtime_target_graph.txt")
string(REPLACE ";" "\n" VE_RUNTIME_TARGET_GRAPH_TEXT "${VE_RUNTIME_TARGET_CLOSURE}")
file(WRITE "${VE_RUNTIME_TARGET_GRAPH_FILE}"
    "ve_runtime\n${VE_RUNTIME_TARGET_GRAPH_TEXT}\n")
set(VE_VOXEL_SANDBOX_TARGET_GRAPH_FILE
    "${CMAKE_CURRENT_BINARY_DIR}/voxel_sandbox_target_graph.txt")
string(REPLACE ";" "\n" VE_VOXEL_SANDBOX_TARGET_GRAPH_TEXT
    "${VE_VOXEL_SANDBOX_TARGET_CLOSURE}")
file(WRITE "${VE_VOXEL_SANDBOX_TARGET_GRAPH_FILE}"
    "ve_voxel_sandbox\n${VE_VOXEL_SANDBOX_TARGET_GRAPH_TEXT}\n")

add_custom_target(verify_runtime_target_graph
    COMMAND "${CMAKE_COMMAND}"
        "-DVE_RUNTIME_TARGET_GRAPH_FILE=${VE_RUNTIME_TARGET_GRAPH_FILE}"
        "-DVE_VOXEL_SANDBOX_TARGET_GRAPH_FILE=${VE_VOXEL_SANDBOX_TARGET_GRAPH_FILE}"
        -P "${CMAKE_CURRENT_LIST_DIR}/VerifyRuntimeTargetGraph.cmake"
    VERBATIM
)
add_dependencies(verify_runtime_target_graph ve_runtime)
set_target_properties(verify_runtime_target_graph PROPERTIES FOLDER "Quality")

if (VE_BUILD_TESTS)
    add_test(NAME runtime_target_graph_policy
        COMMAND "${CMAKE_COMMAND}"
            "-DVE_RUNTIME_TARGET_GRAPH_FILE=${VE_RUNTIME_TARGET_GRAPH_FILE}"
            "-DVE_VOXEL_SANDBOX_TARGET_GRAPH_FILE=${VE_VOXEL_SANDBOX_TARGET_GRAPH_FILE}"
            -P "${CMAKE_CURRENT_LIST_DIR}/VerifyRuntimeTargetGraph.cmake"
    )
    set_tests_properties(runtime_target_graph_policy PROPERTIES LABELS "architecture;quality")
endif()
