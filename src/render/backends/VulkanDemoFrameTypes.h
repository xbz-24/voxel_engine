#pragma once

#include <volk.h>

#include <cstdint>

class Camera;

namespace ve::world
{
	class World;
}

namespace ve::rendering
{
	struct VulkanFrameTiming
	{
		double snapshot_cpu_ms = 0.0;
		double raster_cpu_ms = 0.0;
		double upscale_cpu_ms = 0.0;
		double upload_cpu_ms = 0.0;
		double present_cpu_ms = 0.0;
		double gpu_copy_ms = 0.0;
		bool has_gpu_copy_timing = false;
		std::uint32_t worker_count = 1;
		std::uint32_t sample_step = 1;
		VkExtent2D render_extent{};
	};

	struct VulkanDemoSettings
	{
		std::uint32_t max_internal_width = 1600;
		std::uint32_t max_internal_height = 900;
		std::uint32_t pixel_block_size = 1;
		float max_ray_distance = 96.0f;
		float fog_strength = 0.38f;
		float outline_strength = 0.08f;
		bool adaptive_quality = true;
		bool show_tuning_panel = false;
		bool show_debug_overlay = true;
	};

	struct VulkanDemoInput
	{
		double mouse_x = 0.0;
		double mouse_y = 0.0;
		bool mouse_left_down = false;
		bool mouse_left_pressed = false;
		bool toggle_debug_overlay = false;
		bool toggle_tuning_panel = false;
	};

	struct VulkanSoftwareVoxelRasterizerFrame
	{
		VulkanSoftwareVoxelRasterizerFrame(const ve::world::World& frame_world,
			const Camera& frame_camera,
			VkExtent2D frame_extent,
			VkFormat frame_format,
			int frame_displayed_fps,
			double frame_delta_seconds,
			VulkanFrameTiming frame_timing,
			const VulkanDemoInput& frame_input,
			VulkanDemoSettings& frame_settings) noexcept;

		const ve::world::World& world;
		const Camera& camera;
		VkExtent2D extent{};
		VkFormat format = VK_FORMAT_UNDEFINED;
		int displayed_fps = 0;
		double delta_seconds = 0.0;
		VulkanFrameTiming previous_timing{};
		const VulkanDemoInput& input;
		VulkanDemoSettings& settings;
	};
}
