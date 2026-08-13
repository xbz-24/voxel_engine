#include "EngineRuntime.h"

#include "Logger.h"
#include "RuntimeRenderDriverFactory.h"

namespace ve::engine
{
	EngineStartupResult EngineRuntime::CreateRenderDriver()
	{
		render_driver_ = RuntimeRenderDriverFactory::Create(window_.GraphicsApi());
		if (!render_driver_)
		{
			const EngineStartupResult missing_driver =
				RuntimeRenderDriverFactory::MissingDriverResult(window_.GraphicsApi());
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, missing_driver.message);
			return missing_driver;
		}
		const EngineStartupResult result = render_driver_->Initialize(
			window_, asset_paths_, engine_.CreateInfo());
		if (!result)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, result.message);
		}
		return result;
	}
}
