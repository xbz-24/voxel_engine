#include "EngineRuntime.h"

#include "Logger.h"
#include "RenderBackendSelector.h"

#include <string>

namespace ve::engine
{
	EngineStartupResult EngineRuntime::InitializeWindow()
	{
		const ve::rendering::GraphicsApi graphics_api =
			ve::rendering::RenderBackendSelector::SelectApi(configuration_.render_backend);
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
