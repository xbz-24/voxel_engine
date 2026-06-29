add_library(voxel_engine_sdk STATIC
    "${VE_SOURCE_ROOT}/api/EngineConfigTranslator.cpp"
    "${VE_SOURCE_ROOT}/api/EngineConfigValidation.cpp"
    "${VE_SOURCE_ROOT}/api/EngineDemoPresets.cpp"
    "${VE_SOURCE_ROOT}/api/Engine.cpp"
    "${VE_SOURCE_ROOT}/api/EngineRuntimeApi.cpp"
    "${VE_SOURCE_ROOT}/api/EngineRuntimeBridge.cpp"
    "${VE_SOURCE_ROOT}/api/EngineRuntimeBridge.h"
    "${VE_SOURCE_ROOT}/api/EngineSceneApi.cpp"
    "${VE_SOURCE_ROOT}/api/EngineWorldApi.cpp"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Assets.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/BlockAliases.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Blocks.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/BlockTypes.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Camera.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Engine.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/EngineConfig.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/EngineConfigTypes.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/EngineRun.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Materials.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Runtime.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Scene.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/Voxel.h"
    "${VE_PUBLIC_INCLUDE_ROOT}/voxel/World.h"
)
target_include_directories(voxel_engine_sdk PUBLIC "${VE_PUBLIC_INCLUDE_ROOT}")
target_link_libraries(voxel_engine_sdk PRIVATE ve_project_options ve_app)
set_target_properties(voxel_engine_sdk PROPERTIES FOLDER "Engine/API")
add_library(VoxelEngine::SDK ALIAS voxel_engine_sdk)
