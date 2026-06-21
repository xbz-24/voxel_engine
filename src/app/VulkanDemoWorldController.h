#pragma once

#include "VulkanMinecraftDemoSettings.h"

#include <optional>

namespace ve::engine
{
	class GameModel;

	/** Owns temporary Vulkan demo world presets while the renderer migration is in progress. */
	class VulkanDemoWorldController
	{
	public:
		void Update(GameModel& model,
			const ve::rendering::VulkanMinecraftDemoSceneConfig& scene_config,
			bool force_rebuild);
		void Invalidate() noexcept;

	private:
		std::optional<ve::rendering::VulkanMinecraftDemoSceneConfig> active_config_;
	};
}
