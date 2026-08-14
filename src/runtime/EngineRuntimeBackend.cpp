#include "EngineRuntime.h"

#include "Logger.h"
#include "RenderBackendSelector.h"

#include <string>

namespace ve::engine
{
	EngineStartupResult EngineRuntime::InitializeWindow()
	{
		const ve::rendering::RenderBackendSelection selection =
			ve::rendering::RenderBackendSelector::Select(configuration_.render_backend);
		if (selection.headless)
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::UnsupportedRenderBackend,
				"Headless runtime hosting is not implemented");
		}
		const ve::rendering::GraphicsApi graphics_api = selection.api;
		VE_LOG_CATEGORY_INFO(ve::log::category::Engine,
			ve::rendering::RenderBackendSelector::Name(graphics_api));
		if (!window_.Initialize(graphics_api))
		{
			return EngineStartupResult::Failure(
				EngineStartupFailure::WindowInitializationFailed,
				"Window initialization failed for " +
				std::string{ ve::rendering::RenderBackendSelector::Name(graphics_api) });
		}
		window_.SetVSync(configuration_.vsync);
		return EngineStartupResult::Success();
	}
}
