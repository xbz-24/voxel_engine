#pragma once

#include "VulkanMinecraftDemoSettings.h"

#include <cstddef>
#include <string_view>

#include <glm/vec3.hpp>

namespace ve::rendering
{
	struct VulkanMinecraftDemoProfile
	{
		VulkanMinecraftDemoPreset preset = VulkanMinecraftDemoPreset::HyperrealDesert;
		std::string_view display_name;
		std::string_view window_title;
		int world_size_chunks = 20;
		int render_distance_chunks = 16;
		glm::vec3 camera_position{ 154.0f, 82.0f, 292.0f };
		glm::vec3 camera_target{ 160.0f, 52.0f, 86.0f };
		VulkanMinecraftDemoSceneConfig scene{};
		std::string_view model_asset_keyword;
		int model_stage_radius = 0;
		int model_target_extent = 0;
		std::size_t model_voxel_budget = 0;
	};

	[[nodiscard]] VulkanMinecraftDemoProfile VulkanMinecraftDemoProfileFor(VulkanMinecraftDemoPreset preset) noexcept;
	[[nodiscard]] VulkanMinecraftDemoSceneConfig VulkanMinecraftDemoDefaultSceneConfig(VulkanMinecraftDemoPreset preset) noexcept;
}
