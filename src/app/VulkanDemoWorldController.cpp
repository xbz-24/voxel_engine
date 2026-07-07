#include "VulkanDemoWorldController.h"

#include "GameModel.h"
#include "VulkanDemoSceneBuilder.h"

namespace ve::engine
{
	void VulkanDemoWorldController::Update(GameModel& model,
		const ve::rendering::VulkanMinecraftDemoSceneConfig& scene_config,
		bool force_rebuild)
	{
		if (model.PendingWorldGenerationCount() != 0) return;
		if (!force_rebuild && scene_built_) return;
		VulkanDemoSceneBuilder::Build(model.MutableWorld(), scene_config);
		scene_built_ = true;
	}

	void VulkanDemoWorldController::Invalidate() noexcept
	{
		scene_built_ = false;
	}
}
