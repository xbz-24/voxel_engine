set(VE_PUBLIC_SDK_HEADERS
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Assets.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/BlockAliases.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Blocks.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/BlockTypes.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Camera.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Engine.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/EngineConfig.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/EngineConfigSlices.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/EngineConfigTypes.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/EngineRun.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Environment.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/GraphicsConfig.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Light.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Logging.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Material.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/MaterialLibrary.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Materials.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/MaterialTypes.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Runtime.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/RuntimeCallbacks.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/RuntimeCommands.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/RuntimeFrame.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/RuntimeInput.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Scene.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/SceneEntity.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/SceneGraph.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/SdkInfo.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/TerrainConfig.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Voxel.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/WindowConfig.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/World.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/WorldConfig.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/WorldConfigFactories.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/WorldConfigSerialization.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/WorldConfiguration.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/WorldCoordinates.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/WorldEdits.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/WorldTypes.h"
)

file(GLOB VE_PUBLIC_SDK_SOURCES CONFIGURE_DEPENDS "${VE_SOURCE_ROOT}/api/*.cpp")
list(SORT VE_PUBLIC_SDK_SOURCES)
add_library(voxel_engine_sdk STATIC ${VE_PUBLIC_SDK_SOURCES} ${VE_PUBLIC_SDK_HEADERS})
target_include_directories(voxel_engine_sdk PUBLIC "${VE_PUBLIC_INCLUDE_ROOT}")
target_link_libraries(voxel_engine_sdk PRIVATE ve_project_options ve_app)
ve_enable_common_pch(voxel_engine_sdk)
set_target_properties(voxel_engine_sdk PROPERTIES FOLDER "Engine/API")
add_library(VoxelEngine::SDK ALIAS voxel_engine_sdk)
