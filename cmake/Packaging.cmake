include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(VE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/VoxelEngine")

file(READ "${PROJECT_SOURCE_DIR}/vcpkg.json" VE_VCPKG_MANIFEST)
string(JSON VE_VCPKG_VERSION GET "${VE_VCPKG_MANIFEST}" version)
if (NOT VE_VCPKG_VERSION VERSION_EQUAL "${PROJECT_VERSION}")
    message(FATAL_ERROR
        "vcpkg.json version ${VE_VCPKG_VERSION} differs from project ${PROJECT_VERSION}")
endif()
string(JSON VE_VCPKG_BASELINE GET "${VE_VCPKG_MANIFEST}" builtin-baseline)
string(LENGTH "${VE_VCPKG_BASELINE}" VE_VCPKG_BASELINE_LENGTH)
if (NOT VE_VCPKG_BASELINE_LENGTH EQUAL 40 OR
        NOT VE_VCPKG_BASELINE MATCHES "^[0-9a-f]+$")
    message(FATAL_ERROR
        "vcpkg.json builtin-baseline must be a 40-character lowercase Git commit")
endif()

file(READ "${VE_PUBLIC_INCLUDE_ROOT}/voxel/SdkInfo.h" VE_SDK_INFO)
foreach(version_part IN ITEMS major minor patch)
    if (version_part STREQUAL "major")
        set(expected_version_part "${PROJECT_VERSION_MAJOR}")
    elseif(version_part STREQUAL "minor")
        set(expected_version_part "${PROJECT_VERSION_MINOR}")
    else()
        set(expected_version_part "${PROJECT_VERSION_PATCH}")
    endif()
    if (NOT VE_SDK_INFO MATCHES
            "int[ \t]+${version_part}[ \t]*=[ \t]*${expected_version_part}[ \t]*;")
        message(FATAL_ERROR
            "voxel::Version() ${version_part} differs from project ${PROJECT_VERSION}")
    endif()
endforeach()

install(TARGETS voxel_engine_authoring
    EXPORT VoxelEngineAuthoringTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Authoring
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Authoring
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT Authoring
)
install(FILES ${VE_PUBLIC_AUTHORING_HEADERS}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/voxel"
    COMPONENT Authoring
)
install(EXPORT VoxelEngineAuthoringTargets
    FILE VoxelEngineAuthoringTargets.cmake
    NAMESPACE VoxelEngine::
    DESTINATION "${VE_INSTALL_CMAKEDIR}"
    COMPONENT Authoring
)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/VoxelEngineConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/VoxelEngineConfig.cmake"
    INSTALL_DESTINATION "${VE_INSTALL_CMAKEDIR}"
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/VoxelEngineConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMinorVersion
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/VoxelEngineConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/VoxelEngineConfigVersion.cmake"
    DESTINATION "${VE_INSTALL_CMAKEDIR}"
    COMPONENT Authoring
)

if (VE_BUILD_TESTS)
    add_test(NAME installed_authoring_package
        COMMAND "${CMAKE_COMMAND}"
            "-DVE_CMAKE_COMMAND=${CMAKE_COMMAND}"
            "-DVE_CTEST_COMMAND=${CMAKE_CTEST_COMMAND}"
            "-DVE_ENGINE_BINARY_DIR=${CMAKE_BINARY_DIR}"
            "-DVE_CONSUMER_SOURCE_DIR=${PROJECT_SOURCE_DIR}/packaging/authoring-consumer"
            "-DVE_SMOKE_ROOT=${CMAKE_BINARY_DIR}/installed-authoring-smoke/$<CONFIG>"
            "-DVE_TEST_CONFIG=$<CONFIG>"
            "-DVE_GENERATOR=${CMAKE_GENERATOR}"
            "-DVE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DVE_GENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DVE_MULTI_CONFIG=$<BOOL:${CMAKE_CONFIGURATION_TYPES}>"
            -P "${PROJECT_SOURCE_DIR}/cmake/VerifyInstalledAuthoringPackage.cmake"
    )
    set_tests_properties(installed_authoring_package PROPERTIES
        LABELS "api;authoring;packaging"
        RUN_SERIAL TRUE
    )
endif()
