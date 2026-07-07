#pragma once

#include <cstdint>
#include <cstddef>
#include <string_view>

#include <glm/vec3.hpp>

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
		std::uint32_t rebuilt_chunk_count = 0;
		std::uint32_t cached_chunk_count = 0;
		std::uint64_t world_revision = 0;
		bool gpu_timing_valid = false;
		bool gpu_renderer_active = false;
	};

	[[nodiscard]] const char* VulkanMinecraftDemoPresetName(VulkanMinecraftDemoPreset preset) noexcept;
	[[nodiscard]] VulkanMinecraftDemoProfile VulkanMinecraftDemoProfileFor(VulkanMinecraftDemoPreset preset) noexcept;
	[[nodiscard]] VulkanMinecraftDemoSceneConfig VulkanMinecraftDemoDefaultSceneConfig(VulkanMinecraftDemoPreset preset) noexcept;
}
