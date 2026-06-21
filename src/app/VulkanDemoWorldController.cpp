#include "VulkanDemoWorldController.h"

#include "GameModel.h"
#include "Logger.h"
#include "VulkanDemoSceneBuilder.h"

namespace ve::engine
{
	void VulkanDemoWorldController::Update(GameModel& model,
		const ve::rendering::VulkanMinecraftDemoSceneConfig& scene_config,
		bool force_rebuild)
	{
		if (model.PendingWorldGenerationCount() != 0) return;
		if (!force_rebuild && active_config_ && *active_config_ == scene_config) return;
		VulkanDemoSceneBuilder::Build(model.MutableWorld(), scene_config);
		active_config_ = scene_config;
		VE_LOG_CATEGORY_INFO(ve::log::category::World, "Built editable Vulkan Minecraft demo scene");
	}

	void VulkanDemoWorldController::Invalidate() noexcept
	{
		active_config_.reset();
	}
}
