set(VE_SOURCE_ROOT "${CMAKE_SOURCE_DIR}/src")
set(VE_PUBLIC_INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include")

function(ve_collect_subsystem directory prefix)
    file(GLOB subsystem_sources CONFIGURE_DEPENDS "${directory}/*.cpp")
    file(GLOB subsystem_headers CONFIGURE_DEPENDS "${directory}/*.h" "${directory}/*.inl")
    list(SORT subsystem_sources)
    list(SORT subsystem_headers)
    set(${prefix}_SOURCES ${subsystem_sources} PARENT_SCOPE)
    set(${prefix}_HEADERS ${subsystem_headers} PARENT_SCOPE)
endfunction()

ve_collect_subsystem("${VE_SOURCE_ROOT}/core" VE_CORE)
ve_collect_subsystem("${VE_SOURCE_ROOT}/render" VE_RENDER)
ve_collect_subsystem("${VE_SOURCE_ROOT}/render/backends" VE_RENDER_BACKENDS)
ve_collect_subsystem("${VE_SOURCE_ROOT}/world" VE_WORLD)
ve_collect_subsystem("${VE_SOURCE_ROOT}/network" VE_NETWORK)
ve_collect_subsystem("${VE_SOURCE_ROOT}/app" VE_APP)

function(ve_add_subsystem target folder directory)
    cmake_parse_arguments(SUBSYSTEM "" "" "SOURCES;HEADERS" ${ARGN})
    add_library(${target} STATIC ${SUBSYSTEM_SOURCES} ${SUBSYSTEM_HEADERS})
    target_include_directories(${target} PUBLIC ${directory})
    target_link_libraries(${target} PUBLIC ve_dependencies PRIVATE ve_project_options)
    set_target_properties(${target} PROPERTIES FOLDER "Engine/${folder}")
    source_group(TREE ${directory} FILES ${SUBSYSTEM_SOURCES} ${SUBSYSTEM_HEADERS})
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
ve_add_subsystem(ve_render_backends Render "${VE_SOURCE_ROOT}/render/backends" SOURCES ${VE_RENDER_BACKENDS_SOURCES} HEADERS ${VE_RENDER_BACKENDS_HEADERS})
ve_add_subsystem(ve_world World "${VE_SOURCE_ROOT}/world" SOURCES ${VE_WORLD_SOURCES} HEADERS ${VE_WORLD_HEADERS})
ve_add_subsystem(ve_network Network "${VE_SOURCE_ROOT}/network" SOURCES ${VE_NETWORK_SOURCES} HEADERS ${VE_NETWORK_HEADERS})
ve_add_subsystem(ve_app App "${VE_SOURCE_ROOT}/app" SOURCES ${VE_APP_SOURCES} HEADERS ${VE_APP_HEADERS})
