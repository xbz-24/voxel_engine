#pragma once

#include "CoreTypes.h"
#include "VulkanSoftwareVoxelRasterizer.h"
#include "VulkanUploadBuffer.h"

#include <volk.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

class Camera;

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
		~VulkanFrameRenderer();

		/** @param backend Initialized Vulkan backend. @return True when frame resources are ready. */
		[[nodiscard]] bool Initialize(VulkanBackend& backend, const std::filesystem::path& block_texture_directory);

		/** @return True when the voxel world was copied to the swapchain and presented. */
		[[nodiscard]] bool DrawFrame(const ve::world::World& world,
			const Camera& camera,
			int displayed_fps,
			double delta_seconds,
			const VulkanDemoInput& input);

		/** Waits for the device and releases frame resources. */
		void Release();

	private:
		[[nodiscard]] bool CreateCommandResources();
		[[nodiscard]] bool CreateSynchronization();
		[[nodiscard]] bool CreateTimestampQueries();
		[[nodiscard]] bool EnsureFrameBuffer(VkExtent2D extent);
		[[nodiscard]] bool UploadFramePixels(VulkanFrameTiming& timing);
		[[nodiscard]] bool RecordCommandBuffer(VkCommandBuffer command_buffer, std::uint32_t image_index, std::size_t frame_index);
		void CaptureCompletedGpuTiming(std::size_t frame_index, VulkanFrameTiming& timing) const;

		static constexpr std::size_t kFramesInFlight = 2;
		VulkanBackend* backend_ = nullptr;
		VkDevice device_ = VK_NULL_HANDLE;
		VkCommandPool command_pool_ = VK_NULL_HANDLE;
		VkQueryPool timestamp_query_pool_ = VK_NULL_HANDLE;
		float timestamp_period_ns_ = 0.0f;
		std::array<bool, kFramesInFlight> timestamp_query_valid_{};
		VulkanUploadBuffer upload_buffer_;
		VulkanSoftwareVoxelRasterizer rasterizer_;
		VulkanFrameTiming previous_frame_timing_{};
		VulkanDemoSettings demo_settings_{};
		ve::core::DynamicArray<VkImageLayout> image_layouts_;
		std::array<VkCommandBuffer, kFramesInFlight> command_buffers_{};
		std::array<VkSemaphore, kFramesInFlight> image_available_semaphores_{};
		ve::core::DynamicArray<VkSemaphore> render_finished_semaphores_;
		std::array<VkFence, kFramesInFlight> in_flight_fences_{};
		std::size_t current_frame_ = 0;
		bool logged_first_frame_ = false;
	};
}
