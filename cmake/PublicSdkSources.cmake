set(VE_PUBLIC_AUTHORING_SOURCE_NAMES
    EngineCameraConfig.cpp
    EngineCameraWindow.cpp
    EngineConfigAssetSearchValidation.cpp
    EngineConfigAssetSourceValidation.cpp
    EngineConfigAssetValidation.cpp
    EngineConfigBasicValidation.cpp
    EngineConfigFluent.cpp
    EngineConfigLogging.cpp
    EngineConfigMaterialValidation.cpp
    EngineConfigReferenceValidation.cpp
    EngineConfigRenderEffects.cpp
    EngineConfigRenderEffectValidation.cpp
    EngineConfigRenderStyle.cpp
    EngineConfigRenderStyleValidation.cpp
    EngineConfigRuntimeLayout.cpp
    EngineConfigRuntimeLayoutValidation.cpp
    EngineConfigRuntimeOptions.cpp
    EngineConfigStaticSceneSupport.cpp
    EngineConfigValidationApi.cpp
    EngineConfigValidationDefault.cpp
    EngineConfigValidationHelpers.cpp
    EngineConfigValidationLabels.cpp
    EngineConfigValidatorImpl.cpp
    EngineLogSettings.cpp
    EngineRuntimeApi.cpp
    EngineRuntimeCommands.cpp
    EngineRuntimeLayout.cpp
    EngineSceneAssets.cpp
    EngineSceneEntity.cpp
    EngineSceneGraph.cpp
    EngineSceneGraphLookup.cpp
    EngineSceneGraphValidation.cpp
    EngineSceneLighting.cpp
    EngineSceneMaterial.cpp
    EngineWorldBasics.cpp
    EngineWorldBlockSerialization.cpp
    EngineWorldDeserialization.cpp
    EngineWorldEdits.cpp
    EngineWorldPaths.cpp
    EngineWorldSerialization.cpp
    EngineWorldSerializationTerrain.cpp
    EngineWorldSerializationTerrainLoad.cpp
    EngineWorldStructures.cpp
    EngineWorldTerrain.cpp
)

set(VE_PUBLIC_RUNTIME_ADAPTER_SOURCE_NAMES
    EngineConfigBlockMapping.cpp
    EngineConfigCreateInfoMapping.cpp
    EngineConfigRuntimeCallbackMapping.cpp
    EngineConfigRuntimeLayoutMapping.cpp
    EngineConfigStaticSceneMapping.cpp
    EngineConfigTranslatorDefault.cpp
    EngineConfigTranslatorHelpers.cpp
    EngineConfigTranslatorImpl.cpp
    EngineConfigWorldTranslator.cpp
    EngineInstance.cpp
    EngineInstanceConstruction.cpp
    EngineInstanceRuntime.cpp
    EngineRunShortcuts.cpp
    EngineRuntimeBridge.cpp
)

set(VE_PUBLIC_AUTHORING_SOURCES)
foreach(source_name IN LISTS VE_PUBLIC_AUTHORING_SOURCE_NAMES)
    list(APPEND VE_PUBLIC_AUTHORING_SOURCES "${VE_SOURCE_ROOT}/api/${source_name}")
endforeach()

set(VE_PUBLIC_RUNTIME_ADAPTER_SOURCES)
foreach(source_name IN LISTS VE_PUBLIC_RUNTIME_ADAPTER_SOURCE_NAMES)
    list(APPEND VE_PUBLIC_RUNTIME_ADAPTER_SOURCES "${VE_SOURCE_ROOT}/api/${source_name}")
endforeach()

file(GLOB VE_PUBLIC_API_IMPLEMENTATIONS CONFIGURE_DEPENDS "${VE_SOURCE_ROOT}/api/*.cpp")
set(VE_CLASSIFIED_PUBLIC_API_SOURCES
    ${VE_PUBLIC_AUTHORING_SOURCES}
    ${VE_PUBLIC_RUNTIME_ADAPTER_SOURCES}
)
list(SORT VE_PUBLIC_API_IMPLEMENTATIONS)
list(SORT VE_CLASSIFIED_PUBLIC_API_SOURCES)
if (NOT VE_PUBLIC_API_IMPLEMENTATIONS STREQUAL VE_CLASSIFIED_PUBLIC_API_SOURCES)
    message(FATAL_ERROR
        "Every src/api implementation must be classified explicitly as authoring or runtime adapter")
endif()
