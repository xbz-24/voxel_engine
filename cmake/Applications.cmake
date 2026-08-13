set(VE_DEMO_ROOT "${PROJECT_SOURCE_DIR}/apps/voxel_demo")
set(VE_DEMO_SUPPORT_SOURCES
    "${VE_DEMO_ROOT}/DemoCommandLine.cpp"
    "${VE_DEMO_ROOT}/DemoCommandLine.h"
    "${VE_DEMO_ROOT}/DemoConfig.cpp"
    "${VE_DEMO_ROOT}/DemoConfig.h"
)
set(VE_DEMO_SOURCES
    "${VE_DEMO_ROOT}/main.cpp"
)

add_library(voxel_demo_support STATIC ${VE_DEMO_SUPPORT_SOURCES})
target_include_directories(voxel_demo_support PUBLIC "${VE_DEMO_ROOT}")
target_link_libraries(voxel_demo_support PUBLIC voxel_engine_sdk PRIVATE ve_project_options)
ve_enable_common_pch(voxel_demo_support)
set_target_properties(voxel_demo_support PROPERTIES FOLDER "Applications")

add_executable(voxel_demo ${VE_DEMO_SOURCES} "${VE_SOURCE_ROOT}/app/main.rc")
target_include_directories(voxel_demo PRIVATE "${VE_DEMO_ROOT}")
target_link_libraries(voxel_demo PRIVATE ve_project_options voxel_demo_support)
ve_enable_common_pch(voxel_demo)
set_target_properties(voxel_demo PROPERTIES FOLDER "Applications")
source_group(TREE "${VE_DEMO_ROOT}" FILES ${VE_DEMO_SUPPORT_SOURCES} ${VE_DEMO_SOURCES})
ve_copy_msvc_asan_runtime(voxel_demo)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT voxel_demo)
