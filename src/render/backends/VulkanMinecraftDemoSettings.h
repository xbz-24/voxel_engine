#pragma once

#include <cstdint>

namespace ve::rendering
{
	enum class VulkanMinecraftDemoPreset : std::uint8_t
	{
		HyperrealDesert,
		AquaModel,
		SponzaAtrium
	};

	struct VulkanMinecraftDemoSceneConfig
	{
		VulkanMinecraftDemoPreset preset = VulkanMinecraftDemoPreset::HyperrealDesert;
		int seed = 6407;
		int ground_y = 44;
		int terrain_radius = 132;
		int hill_height = 20;
		int house_count = 52;
		int tree_count = 340;
		int farm_rows = 28;
		int water_radius = 12;
		int tower_height = 12;
		int lantern_count = 30;
		int ore_richness = 28;
		int market_stall_count = 130;
		int floating_island_count = 0;
		int ruin_count = 4;
		int bridge_count = 0;
		int vista_marker_count = 8;
		bool village = false;
		bool farms = false;
		bool water = true;
		bool cave = true;
		bool trees = true;
		bool lights = true;
		bool market = false;
		bool floating_islands = false;
		bool ruins = true;
		bool beacon = false;
		bool stress_blocks = false;

		[[nodiscard]] bool operator==(const VulkanMinecraftDemoSceneConfig&) const = default;
	};

	struct VulkanMinecraftDemoSettings
	{
		VulkanMinecraftDemoSceneConfig scene{};
		bool show_controls = false;
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
		double mesh_rebuild_cpu_ms = 0.0;
		double mesh_upload_cpu_ms = 0.0;
		std::uint32_t index_count = 0;
		std::uint64_t world_revision = 0;
		bool gpu_timing_valid = false;
		bool gpu_renderer_active = false;
	};

	[[nodiscard]] const char* VulkanMinecraftDemoPresetName(VulkanMinecraftDemoPreset preset) noexcept;
}
