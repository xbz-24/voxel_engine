#include "VulkanSoftwareVoxelRasterizer.h"

#include "Camera.h"
#include "VulkanSoftwareVoxelRasterizerRaycast.h"
#include "World.h"

#include <algorithm>
#include <chrono>

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] std::uint32_t SampleStepFor(const VulkanDemoSettings& settings) noexcept
		{
			return std::clamp(settings.pixel_block_size, 1u, 8u);
		}

		[[nodiscard]] std::uint32_t EffectiveSampleStepFor(const VulkanDemoSettings& settings, const VulkanFrameTiming& previous_timing) noexcept
		{
			const std::uint32_t requested = SampleStepFor(settings);
			if (!settings.adaptive_quality || previous_timing.render_extent.width == 0u || previous_timing.render_extent.height == 0u)
			{
				return requested;
			}

			const double previous_cpu_ms =
				previous_timing.raster_cpu_ms +
				previous_timing.upscale_cpu_ms +
				previous_timing.upload_cpu_ms +
				previous_timing.present_cpu_ms;
			if (previous_cpu_ms > 80.0) return std::max(requested, 8u);
			if (previous_cpu_ms > 45.0) return std::max(requested, 6u);
			if (previous_cpu_ms > 28.0) return std::max(requested, 4u);
			if (previous_cpu_ms > 18.0) return std::max(requested, 3u);
			return requested;
		}
	}

	void VulkanSoftwareVoxelRasterizer::Render(const VulkanSoftwareVoxelRasterizerFrame& frame)
	{
		if (frame.input.toggle_debug_overlay) frame.settings.show_debug_overlay = !frame.settings.show_debug_overlay;
		if (frame.input.toggle_tuning_panel) frame.settings.show_tuning_panel = !frame.settings.show_tuning_panel;
		if (!Resize(frame.extent, frame.settings)) return;

		const std::uint64_t world_revision = frame.world.Revision();
		const auto snapshot_start = std::chrono::steady_clock::now();
		if (world_snapshot_.blocks.empty() || world_snapshot_.world_revision != world_revision)
		{
			CaptureWorldSnapshot(frame.world);
		}
		const auto snapshot_end = std::chrono::steady_clock::now();
		const glm::vec3 origin = FindAirCameraOrigin(world_snapshot_, frame.camera.GetPosition());
		const std::uint32_t sample_step = EffectiveSampleStepFor(frame.settings, frame.previous_timing);
		EnsureRayCache(frame.camera, sample_step);

		const RasterWork work{ &world_snapshot_, origin, frame.format, frame.settings.max_ray_distance, frame.settings.fog_strength };
		const auto raster_start = std::chrono::steady_clock::now();
		RenderSamplesMultithreaded(work);
		const auto raster_end = std::chrono::steady_clock::now();
		ApplyVoxelOutlines(frame.format, frame.settings.outline_strength);
		if (frame.settings.show_debug_overlay) DrawDemoOverlay(frame);
		if (frame.settings.show_tuning_panel) DrawTuningPanel(frame);
		DrawCrosshair(frame.format);
		const auto overlay_end = std::chrono::steady_clock::now();

		last_timing_.snapshot_cpu_ms = std::chrono::duration<double, std::milli>(snapshot_end - snapshot_start).count();
		last_timing_.raster_cpu_ms = std::chrono::duration<double, std::milli>(raster_end - raster_start).count();
		last_timing_.upscale_cpu_ms = std::chrono::duration<double, std::milli>(overlay_end - raster_end).count();
		last_timing_.worker_count = static_cast<std::uint32_t>(workers_.size() + 1u);
		last_timing_.sample_step = sample_step;
		last_timing_.render_extent = render_extent_;
	}
}
