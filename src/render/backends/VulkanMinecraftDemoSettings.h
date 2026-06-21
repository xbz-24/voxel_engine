#pragma once

#include <cstdint>

namespace ve::rendering
{
	enum class VulkanMinecraftDemoPreset : std::uint8_t
	{
		LakesideVillage,
		QuarryOutpost,
		ForestFarm,
		StressTown
	};

	struct VulkanMinecraftDemoSceneConfig
	{
		VulkanMinecraftDemoPreset preset = VulkanMinecraftDemoPreset::LakesideVillage;
		int seed = 1337;
		int ground_y = 48;
		int terrain_radius = 52;
		int hill_height = 12;
		int house_count = 5;
		int tree_count = 28;
		int farm_rows = 7;
		int water_radius = 11;
		int tower_height = 14;
		int lantern_count = 12;
		int ore_richness = 14;
		int market_stall_count = 4;
		int floating_island_count = 3;
		int ruin_count = 3;
		int bridge_count = 2;
		int vista_marker_count = 8;
		bool village = true;
		bool farms = true;
		bool water = true;
		bool cave = true;
		bool trees = true;
		bool lights = true;
		bool market = true;
		bool floating_islands = true;
		bool ruins = true;
		bool beacon = true;
		bool stress_blocks = false;

		[[nodiscard]] bool operator==(const VulkanMinecraftDemoSceneConfig&) const = default;
	};

	struct VulkanMinecraftDemoSettings
	{
		VulkanMinecraftDemoSceneConfig scene{};
		bool show_controls = true;
		bool show_metrics = true;
		bool show_imgui_demo_window = false;
		bool request_scene_rebuild = false;
	};

	struct VulkanMinecraftDemoStats
	{
		int displayed_fps = 0;
		double delta_seconds = 0.0;
		double gpu_frame_ms = 0.0;
		double present_cpu_ms = 0.0;
		std::uint32_t index_count = 0;
		std::uint64_t world_revision = 0;
		bool gpu_timing_valid = false;
		bool gpu_renderer_active = false;
	};

	[[nodiscard]] const char* VulkanMinecraftDemoPresetName(VulkanMinecraftDemoPreset preset) noexcept;
}
