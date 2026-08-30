cmake_minimum_required(VERSION 3.25)

foreach(required_variable IN ITEMS
        VE_CMAKE_COMMAND
        VE_CTEST_COMMAND
        VE_ENGINE_BINARY_DIR
        VE_CONSUMER_SOURCE_DIR
        VE_SMOKE_ROOT
        VE_GENERATOR)
    if (NOT DEFINED ${required_variable} OR "${${required_variable}}" STREQUAL "")
        message(FATAL_ERROR "${required_variable} is required")
    endif()
endforeach()

if (NOT DEFINED VE_TEST_CONFIG OR VE_TEST_CONFIG STREQUAL "")
    set(VE_TEST_CONFIG Debug)
endif()

function(ve_run_checked description)
    execute_process(
        COMMAND ${ARGN}
        RESULT_VARIABLE command_result
        OUTPUT_VARIABLE command_output
        ERROR_VARIABLE command_error
    )
    if (NOT command_result EQUAL 0)
        message(FATAL_ERROR
            "${description} failed (${command_result})\n${command_output}\n${command_error}")
    endif()
    message(STATUS "${description} passed")
endfunction()

set(staging_prefix "${VE_SMOKE_ROOT}/staging")
set(relocated_prefix "${VE_SMOKE_ROOT}/relocated")
set(consumer_build "${VE_SMOKE_ROOT}/consumer-build")
file(REMOVE_RECURSE "${VE_SMOKE_ROOT}")
file(MAKE_DIRECTORY "${VE_SMOKE_ROOT}")

ve_run_checked("Authoring component install"
    "${VE_CMAKE_COMMAND}" --install "${VE_ENGINE_BINARY_DIR}"
    --config "${VE_TEST_CONFIG}"
    --prefix "${staging_prefix}"
    --component Authoring
)
file(RENAME "${staging_prefix}" "${relocated_prefix}" RESULT rename_result)
if (rename_result)
    message(FATAL_ERROR "Installed package relocation failed: ${rename_result}")
endif()
foreach(runtime_header IN ITEMS Engine.h EngineRun.h Voxel.h)
    if (EXISTS "${relocated_prefix}/include/voxel/${runtime_header}")
        message(FATAL_ERROR "Runtime-only header was installed: ${runtime_header}")
    endif()
endforeach()

set(generator_arguments -G "${VE_GENERATOR}")
if (DEFINED VE_GENERATOR_PLATFORM AND NOT VE_GENERATOR_PLATFORM STREQUAL "")
    list(APPEND generator_arguments -A "${VE_GENERATOR_PLATFORM}")
endif()
if (DEFINED VE_GENERATOR_TOOLSET AND NOT VE_GENERATOR_TOOLSET STREQUAL "")
    list(APPEND generator_arguments -T "${VE_GENERATOR_TOOLSET}")
endif()
if (NOT VE_MULTI_CONFIG)
    list(APPEND generator_arguments "-DCMAKE_BUILD_TYPE=${VE_TEST_CONFIG}")
endif()

ve_run_checked("Relocated consumer configure"
    "${VE_CMAKE_COMMAND}"
    -S "${VE_CONSUMER_SOURCE_DIR}"
    -B "${consumer_build}"
    ${generator_arguments}
    "-DCMAKE_PREFIX_PATH=${relocated_prefix}"
)
ve_run_checked("Relocated consumer build"
    "${VE_CMAKE_COMMAND}" --build "${consumer_build}" --config "${VE_TEST_CONFIG}"
)
ve_run_checked("Relocated consumer test"
    "${VE_CTEST_COMMAND}" --test-dir "${consumer_build}"
    -C "${VE_TEST_CONFIG}" --output-on-failure
)
