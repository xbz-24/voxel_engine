set(VE_DEMO_ROOT "${PROJECT_SOURCE_DIR}/apps/voxel_demo")
set(VE_DEMO_SUPPORT_SOURCES
    "${VE_DEMO_ROOT}/DemoCommandLine.cpp"
    "${VE_DEMO_ROOT}/DemoCommandLine.h"
    "${VE_DEMO_ROOT}/DemoConfig.cpp"
    "${VE_DEMO_ROOT}/DemoConfig.h"
)
set(VE_DEMO_SOURCES
    "${VE_DEMO_ROOT}/main.cpp"
    "${VE_DEMO_ROOT}/ResizeSmokeController.cpp"
    "${VE_DEMO_ROOT}/ResizeSmokeController.h"
)

add_library(voxel_demo_support STATIC ${VE_DEMO_SUPPORT_SOURCES})
target_include_directories(voxel_demo_support PUBLIC "${VE_DEMO_ROOT}")
target_link_libraries(voxel_demo_support PUBLIC voxel_engine_sdk PRIVATE ve_project_options)
ve_enable_common_pch(voxel_demo_support)
set_target_properties(voxel_demo_support PROPERTIES FOLDER "Applications")

add_executable(voxel_demo ${VE_DEMO_SOURCES} "${VE_SOURCE_ROOT}/app/main.rc")
target_include_directories(voxel_demo PRIVATE "${VE_DEMO_ROOT}")
target_link_libraries(voxel_demo PRIVATE ve_project_options voxel_demo_support)
if (WIN32)
    target_link_libraries(voxel_demo PRIVATE User32)
endif()
ve_enable_common_pch(voxel_demo)
set_target_properties(voxel_demo PROPERTIES FOLDER "Applications")
source_group(TREE "${VE_DEMO_ROOT}" FILES ${VE_DEMO_SUPPORT_SOURCES} ${VE_DEMO_SOURCES})
ve_copy_msvc_asan_runtime(voxel_demo)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT voxel_demo)

if (VE_BUILD_RUNTIME_SMOKE_TESTS)
    add_executable(voxel_static_scene_smoke
        "${PROJECT_SOURCE_DIR}/apps/static_scene_smoke/main.cpp"
        "${PROJECT_SOURCE_DIR}/apps/static_scene_smoke/StaticSceneSmokeBasicScenarios.cpp"
        "${PROJECT_SOURCE_DIR}/apps/static_scene_smoke/StaticSceneSmokeReentrancy.cpp"
        "${PROJECT_SOURCE_DIR}/apps/static_scene_smoke/StaticSceneSmokeScenarios.h"
        "${PROJECT_SOURCE_DIR}/apps/static_scene_smoke/StaticSceneSmokeSupport.cpp"
        "${PROJECT_SOURCE_DIR}/apps/static_scene_smoke/StaticSceneSmokeSupport.h"
    )
    target_link_libraries(voxel_static_scene_smoke PRIVATE
        ve_project_options
        voxel_engine_sdk
    )
    ve_enable_common_pch(voxel_static_scene_smoke)
    set_target_properties(voxel_static_scene_smoke PROPERTIES FOLDER "Tests/Runtime")
    ve_copy_msvc_asan_runtime(voxel_static_scene_smoke)
    add_dependencies(voxel_demo voxel_static_scene_smoke)
endif()
