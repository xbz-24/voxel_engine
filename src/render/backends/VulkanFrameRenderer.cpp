#include "VulkanFrameRenderer.h"

#include "Logger.h"
#include "VulkanBackend.h"
#include "Window.h"

#include <filesystem>

namespace ve::rendering
{
	VulkanFrameRenderer::~VulkanFrameRenderer() { Release(); }
	bool VulkanFrameRenderer::Initialize(VulkanBackend& backend,
		ve::engine::Window& window,
		const std::filesystem::path& block_texture_directory,
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
		const std::filesystem::path shader_directory =
#if defined(VE_VULKAN_SHADER_DIR)
			VE_VULKAN_SHADER_DIR;
#else
			{};
#endif
		if (!gpu_chunk_renderer_.Initialize(backend, command_pool_, block_texture_directory, shader_directory))
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Vulkan GPU chunk renderer failed; falling back to CPU voxel rasterizer");
			rasterizer_.LoadBlockTextures(block_texture_directory);
		}
		else if (imgui_overlay_enabled_ && !imgui_overlay_.Initialize(backend, window, gpu_chunk_renderer_.RenderPass()))
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Vulkan ImGui overlay failed; continuing without live demo controls");
			imgui_overlay_enabled_ = false;
		}
		image_layouts_.assign(backend.Swapchain().Images().size(), VK_IMAGE_LAYOUT_UNDEFINED);
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Vulkan voxel frame renderer initialized");
		return true;
	}
	bool VulkanFrameRenderer::DrawFrame(const ve::world::World& world,
		const Camera& camera,
		int displayed_fps,
		double delta_seconds,
		const VulkanDemoInput& input,
		VulkanMinecraftDemoSettings& minecraft_demo_settings)
	{
		const VulkanGpuFrameControls gpu_controls{
			imgui_overlay_enabled_ && imgui_overlay_.IsInitialized(),
			input.toggle_tuning_panel
		};
		if (gpu_chunk_renderer_.IsInitialized()) return DrawGpuFrame(world, camera, displayed_fps, delta_seconds, minecraft_demo_settings, gpu_controls);
		return DrawSoftwareFrame(world, camera, displayed_fps, delta_seconds, input);
	}
	bool VulkanFrameRenderer::WantsMouseInput() const noexcept
	{
		return imgui_overlay_.WantsMouseInput();
	}
	bool VulkanFrameRenderer::WantsKeyboardInput() const noexcept
	{
		return imgui_overlay_.WantsKeyboardInput();
	}
}
