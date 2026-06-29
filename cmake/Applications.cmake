set(VE_LAUNCHER_SOURCES
    "${VE_SOURCE_ROOT}/api/CommandLineLauncher.cpp"
    "${VE_SOURCE_ROOT}/api/CommandLineLauncher.h"
    "${VE_SOURCE_ROOT}/api/CommandLineLauncherInternal.h"
    "${VE_SOURCE_ROOT}/api/CommandLineLauncherParsing.cpp"
)

add_executable(voxel_engine main.cpp ${VE_LAUNCHER_SOURCES} "${VE_SOURCE_ROOT}/app/main.rc")
target_include_directories(voxel_engine PRIVATE "${VE_SOURCE_ROOT}/api")
target_link_libraries(voxel_engine PRIVATE
    ve_project_options
    voxel_engine_sdk
)
set_target_properties(voxel_engine PROPERTIES FOLDER "Applications")
ve_copy_msvc_asan_runtime(voxel_engine)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT voxel_engine)
