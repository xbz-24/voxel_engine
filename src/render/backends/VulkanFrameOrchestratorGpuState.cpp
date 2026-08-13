#include "VulkanFrameOrchestrator.h"

#include "Logger.h"
#include "VulkanBackend.h"
#include "World.h"

#include <cmath>

namespace ve::rendering
{
	void VulkanFrameOrchestrator::ApplyGpuFrameControls(
		double delta_seconds,
		VulkanOverlaySettings& overlay_settings,
		const VulkanGpuFrameControls& controls)
	{
		if (std::isfinite(delta_seconds) && delta_seconds > 0.0)
		{
			shader_elapsed_seconds_ = std::fmod(
				shader_elapsed_seconds_ + delta_seconds,
				4096.0);
		}
		if (!controls.overlay_enabled) overlay_settings.show_window = false;
		else if (controls.toggle_overlay)
		{
			overlay_settings.show_window = !overlay_settings.show_window;
		}
	}

	bool VulkanFrameOrchestrator::PrepareGpuScene(
		const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		int displayed_fps,
		double delta_seconds,
		VulkanOverlaySettings& overlay_settings,
		const VulkanGpuFrameControls& controls,
		const VulkanFrameTiming& completed_timing)
	{
		if (gpu_chunk_renderer_.NeedsWorldMeshUpdate(world) && !WaitForAllInFlightFrames())
		{
			return false;
		}
		if (!gpu_chunk_renderer_.EnsureWorldMesh(world, block_registry)) return false;
		if (!controls.overlay_enabled) return true;

		const VulkanGpuChunkMeshStats& mesh_stats = gpu_chunk_renderer_.MeshStats();
		imgui_overlay_.BeginFrame(overlay_settings, VulkanRendererStats{
			displayed_fps,
			delta_seconds,
			completed_timing.gpu_copy_ms,
			completed_timing.present_cpu_ms,
			mesh_stats.last_rebuild_cpu_ms,
			mesh_stats.last_upload_cpu_ms,
			gpu_chunk_renderer_.IndexCount(),
			mesh_stats.last_shadow_index_count,
			mesh_stats.last_rebuilt_chunk_count,
			mesh_stats.cached_chunk_count,
			world.Revision(),
			completed_timing.has_gpu_copy_timing,
			true
		});
		return true;
	}

	bool VulkanFrameOrchestrator::CompleteGpuFrame(
		const VulkanFrameTiming& completed_timing,
		double present_cpu_ms,
		VkResult present_result)
	{
		VulkanFrameTiming current_timing{};
		current_timing.gpu_copy_ms = completed_timing.gpu_copy_ms;
		current_timing.has_gpu_copy_timing = completed_timing.has_gpu_copy_timing;
		current_timing.present_cpu_ms = present_cpu_ms;
		current_timing.render_extent = backend_->Swapchain().Extent();
		current_timing.sample_step = 1u;
		current_timing.worker_count = 0u;
		if (timestamp_query_pool_ != VK_NULL_HANDLE)
		{
			frames_[current_frame_].timestamp_query_valid = true;
		}
		previous_frame_timing_ = current_timing;
		current_frame_ = (current_frame_ + 1u) % kFramesInFlight;
		if (!logged_first_frame_)
		{
			VE_LOG_CATEGORY_INFO(
				ve::log::category::Render,
				"Presented first Vulkan GPU chunk frame");
			logged_first_frame_ = true;
		}
		return present_result == VK_SUCCESS || present_result == VK_SUBOPTIMAL_KHR;
	}
}
