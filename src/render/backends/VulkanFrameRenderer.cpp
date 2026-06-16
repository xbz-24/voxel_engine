#include "VulkanFrameRenderer.h"

#include "Camera.h"
#include "Logger.h"
#include "VulkanBackend.h"
#include "World.h"

#include <span>
#include <string>

namespace ve::rendering
{
	namespace
	{
	}

	VulkanFrameRenderer::~VulkanFrameRenderer() { Release(); }

	bool VulkanFrameRenderer::Initialize(VulkanBackend& backend)
	{
		Release();
		backend_ = &backend;
		device_ = backend.Device().Handle();
		if (device_ == VK_NULL_HANDLE) return false;
		if (!CreateCommandResources() || !CreateSynchronization())
		{
			Release();
			return false;
		}
		image_layouts_.assign(backend.Swapchain().Images().size(), VK_IMAGE_LAYOUT_UNDEFINED);
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Vulkan voxel frame renderer initialized");
		return true;
	}

	bool VulkanFrameRenderer::CreateCommandResources()
	{
		VkCommandPoolCreateInfo pool_info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		pool_info.queueFamilyIndex = backend_->PhysicalDevice().QueueFamilies().graphics_family;
		if (vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool_) != VK_SUCCESS) return false;

		VkCommandBufferAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocate_info.commandPool = command_pool_;
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandBufferCount = static_cast<std::uint32_t>(command_buffers_.size());
		return vkAllocateCommandBuffers(device_, &allocate_info, command_buffers_.data()) == VK_SUCCESS;
	}

	bool VulkanFrameRenderer::CreateSynchronization()
	{
		const VkSemaphoreCreateInfo semaphore_info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VkFenceCreateInfo fence_info{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		for (std::size_t index = 0; index < kFramesInFlight; ++index)
		{
			if (vkCreateSemaphore(device_, &semaphore_info, nullptr, &image_available_semaphores_[index]) != VK_SUCCESS ||
				vkCreateSemaphore(device_, &semaphore_info, nullptr, &render_finished_semaphores_[index]) != VK_SUCCESS ||
				vkCreateFence(device_, &fence_info, nullptr, &in_flight_fences_[index]) != VK_SUCCESS)
			{
				return false;
			}
		}
		return true;
	}

	bool VulkanFrameRenderer::EnsureFrameBuffer(VkExtent2D extent)
	{
		if (!rasterizer_.Resize(extent)) return false;
		const VkDeviceSize byte_size = static_cast<VkDeviceSize>(extent.width) * static_cast<VkDeviceSize>(extent.height) * sizeof(std::uint32_t);
		const VkDeviceSize previous_size = upload_buffer_.Size();
		if (!upload_buffer_.Ensure(*backend_, byte_size)) return false;
		if (previous_size != byte_size)
		{
			VE_LOG_CATEGORY_INFO(ve::log::category::Render, std::string("Allocated Vulkan CPU upload framebuffer: ") +
				std::to_string(extent.width) + "x" + std::to_string(extent.height));
		}
		return true;
	}

	bool VulkanFrameRenderer::UploadFramePixels()
	{
		const std::span<const std::uint32_t> pixels = rasterizer_.Pixels();
		return upload_buffer_.CopyFrom(device_, pixels.data(), pixels.size_bytes());
	}

	bool VulkanFrameRenderer::RecordCommandBuffer(VkCommandBuffer command_buffer, std::uint32_t image_index)
	{
		VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) return false;

		VkImageMemoryBarrier to_transfer{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		to_transfer.oldLayout = image_layouts_[image_index];
		to_transfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		to_transfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		to_transfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		to_transfer.image = backend_->Swapchain().Images()[image_index];
		to_transfer.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		to_transfer.subresourceRange.levelCount = 1;
		to_transfer.subresourceRange.layerCount = 1;
		to_transfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &to_transfer);

		const VkExtent2D extent = rasterizer_.Extent();
		VkBufferImageCopy copy_region{};
		copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy_region.imageSubresource.layerCount = 1;
		copy_region.imageExtent = { extent.width, extent.height, 1 };
		vkCmdCopyBufferToImage(command_buffer, upload_buffer_.Buffer(), backend_->Swapchain().Images()[image_index],
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

		VkImageMemoryBarrier to_present{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		to_present.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		to_present.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		to_present.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		to_present.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		to_present.image = backend_->Swapchain().Images()[image_index];
		to_present.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		to_present.subresourceRange.levelCount = 1;
		to_present.subresourceRange.layerCount = 1;
		to_present.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0, 0, nullptr, 0, nullptr, 1, &to_present);
		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) return false;
		image_layouts_[image_index] = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		return true;
	}

	bool VulkanFrameRenderer::DrawFrame(const ve::world::World& world, const Camera& camera)
	{
		if (backend_ == nullptr || device_ == VK_NULL_HANDLE) return false;
		const VkExtent2D extent = backend_->Swapchain().Extent();
		if (!EnsureFrameBuffer(extent)) return false;
		rasterizer_.Render(VulkanSoftwareVoxelRasterizerFrame{ world, camera, extent, backend_->Swapchain().ImageFormat() });
		if (!UploadFramePixels()) return false;

		const VkFence fence = in_flight_fences_[current_frame_];
		if (vkWaitForFences(device_, 1, &fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) return false;

		std::uint32_t image_index = 0;
		const VkResult acquire_result = vkAcquireNextImageKHR(device_, backend_->Swapchain().Handle(), UINT64_MAX,
			image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &image_index);
		if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR) return false;
		if (image_index >= image_layouts_.size()) return false;

		if (vkResetFences(device_, 1, &fence) != VK_SUCCESS) return false;
		VkCommandBuffer command_buffer = command_buffers_[current_frame_];
		if (vkResetCommandBuffer(command_buffer, 0) != VK_SUCCESS || !RecordCommandBuffer(command_buffer, image_index))
		{
			return false;
		}

		const VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &image_available_semaphores_[current_frame_];
		submit_info.pWaitDstStageMask = &wait_stage;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &render_finished_semaphores_[current_frame_];
		if (vkQueueSubmit(backend_->Device().GraphicsQueue(), 1, &submit_info, fence) != VK_SUCCESS) return false;

		VkPresentInfoKHR present_info{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &render_finished_semaphores_[current_frame_];
		const VkSwapchainKHR swapchain = backend_->Swapchain().Handle();
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &swapchain;
		present_info.pImageIndices = &image_index;
		const VkResult present_result = vkQueuePresentKHR(backend_->Device().PresentQueue(), &present_info);
		current_frame_ = (current_frame_ + 1) % kFramesInFlight;
		if (!logged_first_frame_)
		{
			VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Presented first Vulkan voxel frame");
			logged_first_frame_ = true;
		}
		return present_result == VK_SUCCESS || present_result == VK_SUBOPTIMAL_KHR;
	}

	void VulkanFrameRenderer::Release()
	{
		if (device_ != VK_NULL_HANDLE) vkDeviceWaitIdle(device_);
		upload_buffer_.Release(device_);
		for (VkFence& fence : in_flight_fences_)
		{
			if (fence != VK_NULL_HANDLE) vkDestroyFence(device_, fence, nullptr);
			fence = VK_NULL_HANDLE;
		}
		for (VkSemaphore& semaphore : render_finished_semaphores_)
		{
			if (semaphore != VK_NULL_HANDLE) vkDestroySemaphore(device_, semaphore, nullptr);
			semaphore = VK_NULL_HANDLE;
		}
		for (VkSemaphore& semaphore : image_available_semaphores_)
		{
			if (semaphore != VK_NULL_HANDLE) vkDestroySemaphore(device_, semaphore, nullptr);
			semaphore = VK_NULL_HANDLE;
		}
		if (command_pool_ != VK_NULL_HANDLE) vkDestroyCommandPool(device_, command_pool_, nullptr);
		command_pool_ = VK_NULL_HANDLE;
		command_buffers_.fill(VK_NULL_HANDLE);
		image_layouts_.clear();
		rasterizer_ = {};
		device_ = VK_NULL_HANDLE;
		backend_ = nullptr;
		current_frame_ = 0;
		logged_first_frame_ = false;
	}
}
