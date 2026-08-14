#include "VulkanFrameOrchestrator.h"

#include "Logger.h"
#include "VulkanBackend.h"
#include "Window.h"

#include <filesystem>

namespace ve::rendering
{
	VulkanFrameOrchestrator::~VulkanFrameOrchestrator() { Release(); }
	bool VulkanFrameOrchestrator::Initialize(VulkanBackend& backend,
		ve::engine::Window& window,
		const std::filesystem::path& block_texture_directory,
		const std::filesystem::path& shader_directory,
		const VoxelRenderStyle& render_style,
		bool enable_imgui_overlay)
	{
		Release();
		backend_ = &backend;
		device_ = backend.Device().Handle();
		imgui_overlay_enabled_ = enable_imgui_overlay;
		if (device_ == VK_NULL_HANDLE) return false;
		if (!CreateCommandResources() || !CreateSynchronization() || !CreateTimestampQueries())
		{
			Release();
			return false;
		}
		if (!gpu_chunk_renderer_.Initialize(
			backend,
			command_pool_,
			block_texture_directory,
			shader_directory,
			render_style,
			frames_.size()))
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Vulkan GPU chunk renderer failed; falling back to CPU voxel rasterizer");
			rasterizer_.LoadBlockTextures(block_texture_directory);
		}
		else if (imgui_overlay_enabled_ && !imgui_overlay_.Initialize(backend, window, gpu_chunk_renderer_.RenderPass()))
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Vulkan ImGui overlay failed; continuing without diagnostics");
			imgui_overlay_enabled_ = false;
		}
		image_layouts_.assign(backend.Swapchain().Images().size(), VK_IMAGE_LAYOUT_UNDEFINED);
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Vulkan voxel frame orchestrator initialized");
		return true;
	}
	bool VulkanFrameOrchestrator::DrawFrame(const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		const Camera& camera,
		int displayed_fps,
		double delta_seconds,
		const VulkanFrameInput& input,
		VulkanOverlaySettings& overlay_settings)
	{
		const VulkanGpuFrameControls gpu_controls{
			imgui_overlay_enabled_ && imgui_overlay_.IsInitialized(),
			input.toggle_debug_overlay
		};
		if (gpu_chunk_renderer_.IsInitialized()) return DrawGpuFrame(world, block_registry, camera, displayed_fps, delta_seconds, overlay_settings, gpu_controls);
		return DrawSoftwareFrame(world, camera, displayed_fps, delta_seconds, input);
	}
	bool VulkanFrameOrchestrator::WantsMouseInput() const noexcept
	{
		return imgui_overlay_.WantsMouseInput();
	}
	bool VulkanFrameOrchestrator::WantsKeyboardInput() const noexcept
	{
		return imgui_overlay_.WantsKeyboardInput();
	}
}
