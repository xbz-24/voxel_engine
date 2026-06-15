#pragma once

#include "CoreTypes.h"

#include <volk.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

class Camera;

namespace ve::world
{
	class World;
}

namespace ve::rendering
{
	class VulkanBackend;

	/** Presents the Vulkan path by copying a software voxel frame into the swapchain. */
	class VulkanFrameRenderer
	{
	public:
		~VulkanFrameRenderer();

		/** @param backend Initialized Vulkan backend. @return True when frame resources are ready. */
		[[nodiscard]] bool Initialize(VulkanBackend& backend);

		/** @return True when the voxel world was copied to the swapchain and presented. */
		[[nodiscard]] bool DrawFrame(const ve::world::World& world, const Camera& camera);

		/** Waits for the device and releases frame resources. */
		void Release();

	private:
		[[nodiscard]] bool CreateCommandResources();
		[[nodiscard]] bool CreateSynchronization();
		[[nodiscard]] bool EnsureFrameBuffer(VkExtent2D extent);
		[[nodiscard]] bool CreateUploadBuffer(VkDeviceSize byte_size);
		[[nodiscard]] bool UploadFramePixels();
		[[nodiscard]] bool RecordCommandBuffer(VkCommandBuffer command_buffer, std::uint32_t image_index);
		void DestroyUploadBuffer();
		void RenderSoftwareWorld(const ve::world::World& world, const Camera& camera);

		static constexpr std::size_t kFramesInFlight = 2;
		VulkanBackend* backend_ = nullptr;
		VkDevice device_ = VK_NULL_HANDLE;
		VkCommandPool command_pool_ = VK_NULL_HANDLE;
		VkBuffer upload_buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory upload_memory_ = VK_NULL_HANDLE;
		VkDeviceSize upload_buffer_size_ = 0;
		VkExtent2D pixel_extent_{};
		std::vector<std::uint32_t> pixels_;
		ve::core::DynamicArray<VkImageLayout> image_layouts_;
		std::array<VkCommandBuffer, kFramesInFlight> command_buffers_{};
		std::array<VkSemaphore, kFramesInFlight> image_available_semaphores_{};
		std::array<VkSemaphore, kFramesInFlight> render_finished_semaphores_{};
		std::array<VkFence, kFramesInFlight> in_flight_fences_{};
		std::size_t current_frame_ = 0;
		bool logged_first_frame_ = false;
	};
}
