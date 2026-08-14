cmake_minimum_required(VERSION 3.25)

foreach(required_variable IN ITEMS
        VE_CMAKE_COMMAND VE_CTEST_COMMAND VE_ENGINE_BINARY_DIR
        VE_ENGINE_SOURCE_DIR VE_CONSUMER_SOURCE_DIR VE_SMOKE_ROOT VE_GENERATOR)
    if (NOT DEFINED ${required_variable} OR "${${required_variable}}" STREQUAL "")
        message(FATAL_ERROR "${required_variable} is required")
    endif()
endforeach()
if (NOT DEFINED VE_TEST_CONFIG OR VE_TEST_CONFIG STREQUAL "")
    set(VE_TEST_CONFIG Debug)
endif()

function(ve_run_checked description)
    execute_process(COMMAND ${ARGN} RESULT_VARIABLE command_result
        OUTPUT_VARIABLE command_output ERROR_VARIABLE command_error)
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
foreach(component IN ITEMS Authoring SDK)
    ve_run_checked("${component} component install"
        "${VE_CMAKE_COMMAND}" --install "${VE_ENGINE_BINARY_DIR}"
        --config "${VE_TEST_CONFIG}" --prefix "${staging_prefix}"
        --component "${component}")
endforeach()
file(RENAME "${staging_prefix}" "${relocated_prefix}" RESULT rename_result)
if (rename_result)
    message(FATAL_ERROR "Installed package relocation failed: ${rename_result}")
endif()

foreach(required_file IN ITEMS Engine.h EngineRun.h RuntimeLayout.h Voxel.h)
    if (NOT EXISTS "${relocated_prefix}/include/voxel/${required_file}")
        message(FATAL_ERROR "SDK header was not installed: ${required_file}")
    endif()
endforeach()
file(GLOB_RECURSE installed_files RELATIVE "${relocated_prefix}"
    "${relocated_prefix}/*")
foreach(installed_file IN LISTS installed_files)
    string(TOLOWER "${installed_file}" normalized_installed_file)
    if (normalized_installed_file MATCHES "(^|/)assets(/|$)" OR
            normalized_installed_file MATCHES "[.]spv$")
        message(FATAL_ERROR
            "SDK compile/link package contains runtime payload: ${installed_file}")
    endif()
endforeach()
file(GLOB_RECURSE package_metadata "${relocated_prefix}/*.cmake")
foreach(metadata_file IN LISTS package_metadata)
    file(READ "${metadata_file}" metadata_content)
    string(REPLACE "\\" "/" metadata_content "${metadata_content}")
    foreach(leak_root IN ITEMS VE_ENGINE_SOURCE_DIR VE_ENGINE_BINARY_DIR VE_VCPKG_INSTALLED_DIR)
        if (DEFINED ${leak_root} AND NOT "${${leak_root}}" STREQUAL "")
            file(TO_CMAKE_PATH "${${leak_root}}" normalized_leak_root)
            string(FIND "${metadata_content}" "${normalized_leak_root}" leak_index)
            if (NOT leak_index EQUAL -1)
                message(FATAL_ERROR "${metadata_file} leaks ${leak_root}")
            endif()
        endif()
    endforeach()
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
else()
    list(APPEND generator_arguments
        "-DCMAKE_CONFIGURATION_TYPES=${VE_TEST_CONFIG}")
endif()
set(dependency_arguments)
if (DEFINED VE_CMAKE_TOOLCHAIN_FILE AND EXISTS "${VE_CMAKE_TOOLCHAIN_FILE}")
    list(APPEND dependency_arguments "-DCMAKE_TOOLCHAIN_FILE=${VE_CMAKE_TOOLCHAIN_FILE}")
endif()
if (DEFINED VE_VCPKG_INSTALLED_DIR AND NOT VE_VCPKG_INSTALLED_DIR STREQUAL "")
    list(APPEND dependency_arguments "-DVCPKG_INSTALLED_DIR=${VE_VCPKG_INSTALLED_DIR}")
endif()
if (DEFINED VE_VCPKG_TARGET_TRIPLET AND NOT VE_VCPKG_TARGET_TRIPLET STREQUAL "")
    list(APPEND dependency_arguments "-DVCPKG_TARGET_TRIPLET=${VE_VCPKG_TARGET_TRIPLET}")
endif()

ve_run_checked("Relocated SDK consumer configure" "${VE_CMAKE_COMMAND}"
    -S "${VE_CONSUMER_SOURCE_DIR}" -B "${consumer_build}" ${generator_arguments}
    ${dependency_arguments} "-DCMAKE_PREFIX_PATH=${relocated_prefix}")
ve_run_checked("Relocated SDK consumer build" "${VE_CMAKE_COMMAND}"
    --build "${consumer_build}" --config "${VE_TEST_CONFIG}")
ve_run_checked("Relocated SDK consumer test" "${VE_CTEST_COMMAND}"
    --test-dir "${consumer_build}" -C "${VE_TEST_CONFIG}" --output-on-failure)
