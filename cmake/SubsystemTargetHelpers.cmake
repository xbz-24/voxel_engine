set(VE_SOURCE_ROOT "${PROJECT_SOURCE_DIR}/src")
set(VE_PUBLIC_INCLUDE_ROOT "${PROJECT_SOURCE_DIR}/include")

function(ve_collect_subsystem directory prefix)
    file(GLOB subsystem_sources CONFIGURE_DEPENDS "${directory}/*.cpp")
    file(GLOB subsystem_headers CONFIGURE_DEPENDS "${directory}/*.h")
    list(SORT subsystem_sources)
    list(SORT subsystem_headers)
    set(${prefix}_SOURCES ${subsystem_sources} PARENT_SCOPE)
    set(${prefix}_HEADERS ${subsystem_headers} PARENT_SCOPE)
endfunction()

ve_collect_subsystem("${VE_SOURCE_ROOT}/core" VE_CORE)
ve_collect_subsystem("${VE_SOURCE_ROOT}/render" VE_RENDER)
file(GLOB VE_RENDER_BACKENDS_HEADERS CONFIGURE_DEPENDS
    "${VE_SOURCE_ROOT}/render/backends/*.h"
)
list(SORT VE_RENDER_BACKENDS_HEADERS)
ve_collect_subsystem("${VE_SOURCE_ROOT}/world" VE_WORLD)
ve_collect_subsystem("${VE_SOURCE_ROOT}/network" VE_NETWORK)
ve_collect_subsystem("${VE_SOURCE_ROOT}/runtime" VE_RUNTIME)
ve_collect_subsystem("${VE_SOURCE_ROOT}/app" VE_VOXEL_SANDBOX)

function(ve_add_subsystem target folder directory)
    cmake_parse_arguments(SUBSYSTEM "" "" "SOURCES;HEADERS" ${ARGN})
    add_library(${target} STATIC ${SUBSYSTEM_SOURCES} ${SUBSYSTEM_HEADERS})
    target_include_directories(${target} PUBLIC ${directory})
    target_link_libraries(${target} PUBLIC ve_dependencies PRIVATE ve_project_options)
    ve_enable_common_pch(${target})
    set_target_properties(${target} PROPERTIES FOLDER "Engine/${folder}")
    source_group(TREE ${directory} FILES ${SUBSYSTEM_SOURCES} ${SUBSYSTEM_HEADERS})
endfunction()

function(ve_add_backend_component target folder)
    cmake_parse_arguments(COMPONENT "" "" "SOURCES;HEADERS;PUBLIC_LINKS;PRIVATE_LINKS" ${ARGN})
    add_library(${target} STATIC ${COMPONENT_SOURCES} ${COMPONENT_HEADERS})
    target_include_directories(${target} PUBLIC "${VE_SOURCE_ROOT}/render/backends")
    target_link_libraries(${target}
        PUBLIC
            ve_dependencies
            ${COMPONENT_PUBLIC_LINKS}
        PRIVATE
            ve_project_options
            ${COMPONENT_PRIVATE_LINKS}
    )
    ve_enable_common_pch(${target})
    set_target_properties(${target} PROPERTIES FOLDER "Engine/Render/${folder}")
    source_group(TREE "${VE_SOURCE_ROOT}/render/backends" FILES ${COMPONENT_SOURCES} ${COMPONENT_HEADERS})
endfunction()

function(ve_copy_msvc_asan_runtime target)
    if (NOT (VE_ENABLE_ASAN AND MSVC))
        return()
    endif()
    get_filename_component(VE_MSVC_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    file(GLOB VE_MSVC_ASAN_RUNTIME_DLLS CONFIGURE_DEPENDS
        "${VE_MSVC_BIN_DIR}/clang_rt.asan*_dynamic-x86_64.dll"
    )
    if (NOT VE_MSVC_ASAN_RUNTIME_DLLS)
        message(WARNING "MSVC AddressSanitizer runtime DLLs were not found next to ${CMAKE_CXX_COMPILER}.")
        return()
    endif()
    foreach(asan_runtime_dll IN LISTS VE_MSVC_ASAN_RUNTIME_DLLS)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${asan_runtime_dll}"
                "$<TARGET_FILE_DIR:${target}>"
            VERBATIM
        )
    endforeach()
endfunction()

ve_add_subsystem(ve_core Core "${VE_SOURCE_ROOT}/core" SOURCES ${VE_CORE_SOURCES} HEADERS ${VE_CORE_HEADERS})
ve_add_subsystem(ve_render Render "${VE_SOURCE_ROOT}/render" SOURCES ${VE_RENDER_SOURCES} HEADERS ${VE_RENDER_HEADERS})
ve_add_subsystem(ve_world World "${VE_SOURCE_ROOT}/world" SOURCES ${VE_WORLD_SOURCES} HEADERS ${VE_WORLD_HEADERS})
ve_add_subsystem(ve_network Network "${VE_SOURCE_ROOT}/network" SOURCES ${VE_NETWORK_SOURCES} HEADERS ${VE_NETWORK_HEADERS})
ve_add_subsystem(ve_runtime Runtime "${VE_SOURCE_ROOT}/runtime" SOURCES ${VE_RUNTIME_SOURCES} HEADERS ${VE_RUNTIME_HEADERS})
ve_add_subsystem(ve_voxel_sandbox VoxelSandbox "${VE_SOURCE_ROOT}/app"
    SOURCES ${VE_VOXEL_SANDBOX_SOURCES}
    HEADERS ${VE_VOXEL_SANDBOX_HEADERS}
)
add_library(ve_app ALIAS ve_voxel_sandbox)
get_target_property(VE_APP_ALIAS_TARGET ve_app ALIASED_TARGET)
if (NOT VE_APP_ALIAS_TARGET STREQUAL "ve_voxel_sandbox")
    message(FATAL_ERROR "ve_app compatibility alias must resolve to ve_voxel_sandbox")
endif()
