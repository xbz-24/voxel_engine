#include "Engine.h"

#include <utility>

EngineApplication::EngineApplication()
	: EngineApplication(ve::engine::EngineCreateInfo{})
{
}

EngineApplication::EngineApplication(ve::engine::EngineCreateInfo create_info)
	: create_info_(std::move(create_info))
{
	_runtimeSettings.isVSyncEnabled = create_info_.vsync;
	_runtimeSettings.showDebugOverlay = create_info_.show_debug_overlay;
	_runtimeSettings.renderDistanceChunks = ve::gameplay::ClampRenderDistanceChunks(create_info_.render_distance_chunks);
	_runtimeSettings.renderBackendConfiguration = create_info_.render_backend;
}

EngineApplication::~EngineApplication() = default;
