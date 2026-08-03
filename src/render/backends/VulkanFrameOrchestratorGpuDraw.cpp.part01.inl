#include "VulkanFrameOrchestrator.h"

#include "Camera.h"
#include "CoreTypes.h"
#include "Logger.h"
#include "VulkanBackend.h"
#include "VulkanFrameOrchestratorPresentation.h"
#include "World.h"

#include <chrono>
#include <cmath>
#include <string>

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] int VulkanResultCode(VkResult result) noexcept
		{
			return ve::core::ToInt(result);
		}
	}

	bool VulkanFrameOrchestrator::DrawGpuFrame(const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		const Camera& camera,
		int displayed_fps,
		double delta_seconds,
		VulkanMinecraftDemoSettings& minecraft_demo_settings,
		const VulkanGpuFrameControls& controls)
	{
		if (backend_ == nullptr || device_ == VK_NULL_HANDLE) return false;
		if (std::isfinite(delta_seconds) && delta_seconds > 0.0)
		{
			shader_elapsed_seconds_ = std::fmod(shader_elapsed_seconds_ + delta_seconds, 4096.0);
		}
		if (!controls.overlay_enabled)
		{
			minecraft_demo_settings.show_controls = false;
			minecraft_demo_settings.show_imgui_demo_window = false;
		}
		else if (controls.toggle_controls)
		{
			minecraft_demo_settings.show_controls = !minecraft_demo_settings.show_controls;
		}
		const VkFence fence = frames_[current_frame_].in_flight;
		if (vkWaitForFences(device_, 1, &fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) return false;
		VulkanFrameTiming completed_frame_timing = previous_frame_timing_;
		CaptureCompletedGpuTiming(current_frame_, completed_frame_timing);
		if (gpu_chunk_renderer_.NeedsWorldMeshUpdate(world) && !WaitForAllInFlightFrames()) return false;
		if (!gpu_chunk_renderer_.EnsureWorldMesh(world, block_registry)) return false;
		const VulkanGpuChunkMeshStats& mesh_stats = gpu_chunk_renderer_.MeshStats();
		if (controls.overlay_enabled)
		{
			imgui_overlay_.BeginFrame(minecraft_demo_settings, VulkanMinecraftDemoStats{
				displayed_fps,
				delta_seconds,
				completed_frame_timing.gpu_copy_ms,
				completed_frame_timing.present_cpu_ms,
				mesh_stats.last_rebuild_cpu_ms,
				mesh_stats.last_upload_cpu_ms,
				gpu_chunk_renderer_.IndexCount(),
				mesh_stats.last_shadow_index_count,
				mesh_stats.last_rebuilt_chunk_count,
				mesh_stats.cached_chunk_count,
				world.Revision(),
				completed_frame_timing.has_gpu_copy_timing,
				true
			});
		}
		std::uint32_t image_index = 0;
		const auto present_start = std::chrono::steady_clock::now();
		const VkResult acquire_result = AcquireSwapchainImage(device_, *backend_, frames_[current_frame_].image_available, image_index);
		if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR)
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Failed to acquire Vulkan swapchain image: " + std::to_string(VulkanResultCode(acquire_result)));
			return false;
		}
		if (!WaitForSwapchainImage(device_, images_in_flight_, image_index))
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Failed to wait for Vulkan swapchain image fence");
			return false;
		}
		if (vkResetFences(device_, 1, &fence) != VK_SUCCESS)
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Failed to reset Vulkan frame fence");
			return false;
		}
