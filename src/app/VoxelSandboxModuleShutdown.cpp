#include "VoxelSandboxModule.h"

#include "Logger.h"
#include "RuntimeRenderDriver.h"

namespace ve::engine
{
	void VoxelSandboxModule::Shutdown() noexcept
	{
		editor_controller_.Shutdown();
		static_model_scene_.Shutdown();
		model_.reset();
		if (!render_driver_)
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Engine,
				"No runtime render driver to release");
			VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Engine runtime stopped");
			return;
		}
		render_driver_->Shutdown();
		render_driver_.reset();
		VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Engine runtime stopped");
	}
}
