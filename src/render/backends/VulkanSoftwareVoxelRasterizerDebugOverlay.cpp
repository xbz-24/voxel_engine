#include "VulkanSoftwareVoxelRasterizer.h"

#include "VulkanSoftwareRasterizerColor.h"

#include <algorithm>
#include <array>
#include <cstdio>

namespace ve::rendering
{
	void VulkanSoftwareVoxelRasterizer::DrawDemoOverlay(const VulkanSoftwareVoxelRasterizerFrame& frame)
	{
		const std::uint32_t accent = PackColor({ 87, 220, 180 }, frame.format);
		const std::uint32_t shadow = PackColor({ 18, 32, 44 }, frame.format);
		const std::uint32_t text = PackColor({ 232, 248, 244 }, frame.format);
		const std::uint32_t desired_bar_width = render_extent_.width > 480u ? std::min<std::uint32_t>(render_extent_.width / 3u, 430u) : render_extent_.width;
		const std::uint32_t bar_y = render_extent_.height > 32u ? 14u : 0u;
		const std::uint32_t bar_x = std::min<std::uint32_t>(20u, render_extent_.width);
		const std::uint32_t bar_width = std::min(desired_bar_width, render_extent_.width - bar_x);
		const std::uint32_t bar_end_x = std::min(bar_x + bar_width, render_extent_.width);
		for (std::uint32_t y = bar_y; y < bar_y + 96u && y < render_extent_.height; ++y)
		{
			std::uint32_t* row = render_pixels_.data() + (static_cast<std::size_t>(y) * render_extent_.width);
			std::fill(row + bar_x, row + bar_end_x, shadow);
		}
		for (std::uint32_t y = bar_y; y < bar_y + 4u && y < render_extent_.height; ++y)
		{
			std::uint32_t* row = render_pixels_.data() + (static_cast<std::size_t>(y) * render_extent_.width);
			std::fill(row + bar_x, row + bar_end_x, accent);
		}

		std::array<char, 64> fps_line{};
		std::array<char, 64> cpu_line{};
		std::array<char, 64> vk_line{};
		std::array<char, 64> resolution_line{};
		std::array<char, 64> sampling_line{};
		const double milliseconds = std::max(frame.delta_seconds, 0.0) * 1000.0;
		if (frame.displayed_fps > 0) std::snprintf(fps_line.data(), fps_line.size(), "FPS %d   %.1f MS", frame.displayed_fps, milliseconds);
		else std::snprintf(fps_line.data(), fps_line.size(), "FPS --   %.1f MS", milliseconds);

		std::snprintf(cpu_line.data(), cpu_line.size(), "SNAP %.1f RAST %.1f UI %.1f", frame.previous_timing.snapshot_cpu_ms, frame.previous_timing.raster_cpu_ms, frame.previous_timing.upscale_cpu_ms);
		std::snprintf(vk_line.data(), vk_line.size(), "COPY %.1f VK %.1f", frame.previous_timing.upload_cpu_ms, frame.previous_timing.present_cpu_ms);
		std::snprintf(resolution_line.data(), resolution_line.size(), "OUT %uX%u IN %uX%u", extent_.width, extent_.height, frame.previous_timing.render_extent.width, frame.previous_timing.render_extent.height);
		std::snprintf(sampling_line.data(), sampling_line.size(), "STEP %u TH %u", frame.previous_timing.sample_step, frame.previous_timing.worker_count);

		DrawText("VULKAN VOXEL DEMO", bar_x + 8u, bar_y + 9u, 1u, text);
		DrawText(fps_line.data(), bar_x + 8u, bar_y + 23u, 1u, text);
		DrawText(cpu_line.data(), bar_x + 8u, bar_y + 37u, 1u, text);
		DrawText(vk_line.data(), bar_x + 8u, bar_y + 51u, 1u, text);
		if (frame.previous_timing.has_gpu_copy_timing)
		{
			std::array<char, 64> gpu_line{};
			std::snprintf(gpu_line.data(), gpu_line.size(), "GPU COPY %.2f", frame.previous_timing.gpu_copy_ms);
			DrawText(gpu_line.data(), bar_x + 8u, bar_y + 65u, 1u, text);
		}
		else DrawText("GPU COPY --", bar_x + 8u, bar_y + 65u, 1u, text);

		DrawText(resolution_line.data(), bar_x + 8u, bar_y + 79u, 1u, text);
		DrawText(sampling_line.data(), bar_x + 8u, bar_y + 93u, 1u, text);
	}
}
