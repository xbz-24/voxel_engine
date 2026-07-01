#pragma once

#include "CoreTypes.h"
#include "VulkanGpuChunkRenderer.h"
#include "VulkanImGuiOverlay.h"
#include "VulkanMinecraftDemoSettings.h"
#include "VulkanSoftwareVoxelRasterizer.h"
#include "VulkanUploadBuffer.h"

#include <volk.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

class Camera;
namespace ve::engine { class Window; }

namespace ve::world
{
	class World;
}

namespace ve::rendering
{
	class VulkanBackend;

	/** Presents Vulkan frames by copying a rasterized frame image into the swapchain. */
	class VulkanFrameRenderer
	{
	public:
		// TODO: Rename this from demo renderer to frame orchestrator once software and GPU paths become pluggable passes.
		~VulkanFrameRenderer();

		/** @param backend Initialized Vulkan backend. @return True when frame resources are ready. */
		[[nodiscard]] bool Initialize(VulkanBackend& backend,
			ve::engine::Window& window,
			const std::filesystem::path& block_texture_directory,
			bool enable_imgui_overlay = true);

		/** @return True when the voxel world was copied to the swapchain and presented. */
		[[nodiscard]] bool DrawFrame(const ve::world::World& world,
			const Camera& camera,
			int displayed_fps,
			double delta_seconds,
			const VulkanDemoInput& input,
			VulkanMinecraftDemoSettings& minecraft_demo_settings);

		/** Waits for the device and releases frame resources. */
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
		[[nodiscard]] bool RecordSoftwareCommandBuffer(VkCommandBuffer command_buffer, std::uint32_t image_index, std::size_t frame_index);
		[[nodiscard]] bool RecordGpuCommandBuffer(VkCommandBuffer command_buffer, std::uint32_t image_index, std::size_t frame_index, const Camera& camera);
		[[nodiscard]] bool DrawGpuFrame(const ve::world::World& world,
			const Camera& camera,
			int displayed_fps,
			double delta_seconds,
			VulkanMinecraftDemoSettings& minecraft_demo_settings);
		[[nodiscard]] bool DrawSoftwareFrame(const ve::world::World& world,
			const Camera& camera,
			int displayed_fps,
			double delta_seconds,
			const VulkanDemoInput& input);
		[[nodiscard]] bool WaitForAllInFlightFrames() const;
		void CaptureCompletedGpuTiming(std::size_t frame_index, VulkanFrameTiming& timing) const;
		void ReleaseIntermediateImages();
		[[nodiscard]] static std::uint32_t FindMemoryType(VkPhysicalDevice physical_device, std::uint32_t type_filter, VkMemoryPropertyFlags properties);

		static constexpr std::size_t kFramesInFlight = 2;
		struct FrameResources
		{
			VulkanUploadBuffer upload_buffer;
			VkImage intermediate_image = VK_NULL_HANDLE;
			VkDeviceMemory intermediate_image_memory = VK_NULL_HANDLE;
			VkImageLayout intermediate_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
			VkCommandBuffer command_buffer = VK_NULL_HANDLE;
			VkSemaphore image_available = VK_NULL_HANDLE;
			VkSemaphore render_finished = VK_NULL_HANDLE;
			VkFence in_flight = VK_NULL_HANDLE;
			bool timestamp_query_valid = false;
		};

		VulkanBackend* backend_ = nullptr;
		VkDevice device_ = VK_NULL_HANDLE;
		VkCommandPool command_pool_ = VK_NULL_HANDLE;
		VkQueryPool timestamp_query_pool_ = VK_NULL_HANDLE;
		float timestamp_period_ns_ = 0.0f;
		std::array<FrameResources, kFramesInFlight> frames_{};
		VulkanGpuChunkRenderer gpu_chunk_renderer_;
		VulkanImGuiOverlay imgui_overlay_;
		VulkanSoftwareVoxelRasterizer rasterizer_;
		VulkanFrameTiming previous_frame_timing_{};
		VulkanDemoSettings demo_settings_{};
		VkExtent2D intermediate_extent_{};
		VkFormat intermediate_format_ = VK_FORMAT_UNDEFINED;
		VkFilter upscale_filter_ = VK_FILTER_NEAREST;
		ve::core::DynamicArray<VkImageLayout> image_layouts_;
		ve::core::DynamicArray<VkFence> images_in_flight_;
		std::size_t current_frame_ = 0;
		bool logged_first_frame_ = false;
	};
}
