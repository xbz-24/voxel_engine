#pragma once

#include "VulkanFrameOrchestratorState.h"

#include <filesystem>

class Camera;
namespace ve::engine { class Window; }
namespace ve::world { class World; }
namespace ve::blocks { class BlockRegistry; }

namespace ve::rendering
{
	/** Presents rasterized Vulkan frames through the swapchain. */
	class VulkanFrameOrchestrator : private VulkanFrameOrchestratorState
	{
	public:
		~VulkanFrameOrchestrator();

		[[nodiscard]] bool Initialize(VulkanBackend& backend,
			ve::engine::Window& window,
			const std::filesystem::path& block_texture_directory,
			const VoxelRenderStyle& render_style,
			bool enable_imgui_overlay = true);

		[[nodiscard]] bool DrawFrame(const ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			const Camera& camera,
			int displayed_fps,
			double delta_seconds,
			const VulkanFrameInput& input,
			VulkanOverlaySettings& overlay_settings);

		void Release();
		[[nodiscard]] bool WantsMouseInput() const noexcept;
		[[nodiscard]] bool WantsKeyboardInput() const noexcept;

	private:
		[[nodiscard]] bool CreateCommandResources();
		[[nodiscard]] bool CreateSynchronization();
		[[nodiscard]] bool CreateTimestampQueries();
		[[nodiscard]] bool EnsureFrameBuffer(VkExtent2D extent);
		[[nodiscard]] bool EnsureIntermediateImages(VkExtent2D extent, VkFormat format);
		[[nodiscard]] bool UploadFramePixels(VulkanFrameTiming& timing, std::size_t frame_index);
		[[nodiscard]] bool RecordSoftwareCommandBuffer(
			VkCommandBuffer command_buffer,
			std::uint32_t image_index,
			std::size_t frame_index);
		[[nodiscard]] bool RecordGpuCommandBuffer(VkCommandBuffer command_buffer,
			std::uint32_t image_index,
			std::size_t frame_index,
			float elapsed_seconds,
			const Camera& camera);
		[[nodiscard]] bool DrawGpuFrame(const ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			const Camera& camera,
			int displayed_fps,
			double delta_seconds,
			VulkanOverlaySettings& overlay_settings,
			const VulkanGpuFrameControls& controls);
		[[nodiscard]] bool DrawSoftwareFrame(const ve::world::World& world,
			const Camera& camera,
			int displayed_fps,
			double delta_seconds,
			const VulkanFrameInput& input);
		void ApplyGpuFrameControls(double delta_seconds,
			VulkanOverlaySettings& overlay_settings,
			const VulkanGpuFrameControls& controls);
		[[nodiscard]] bool PrepareGpuScene(const ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			int displayed_fps,
			double delta_seconds,
			VulkanOverlaySettings& overlay_settings,
			const VulkanGpuFrameControls& controls,
			const VulkanFrameTiming& completed_timing);
		[[nodiscard]] bool CompleteGpuFrame(const VulkanFrameTiming& completed_timing,
			double present_cpu_ms,
			VkResult present_result);
		[[nodiscard]] bool WaitForAllInFlightFrames() const;
		void CaptureCompletedGpuTiming(std::size_t frame_index, VulkanFrameTiming& timing) const;
		void ReleaseIntermediateImages();
		[[nodiscard]] static std::uint32_t FindMemoryType(VkPhysicalDevice physical_device,
			std::uint32_t type_filter,
			VkMemoryPropertyFlags properties);
	};
}
