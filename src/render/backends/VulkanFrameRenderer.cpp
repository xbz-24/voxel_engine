#include "VulkanFrameRenderer.h"

#include "Camera.h"
#include "Logger.h"
#include "VulkanBackend.h"
#include "World.h"

#include <chrono>
#include <span>
#include <string>

namespace ve::rendering
{
	namespace
	{
	}

	VulkanFrameRenderer::~VulkanFrameRenderer() { Release(); }

	bool VulkanFrameRenderer::Initialize(VulkanBackend& backend, const std::filesystem::path& block_texture_directory)
	{
		Release();
		backend_ = &backend;
		device_ = backend.Device().Handle();
		if (device_ == VK_NULL_HANDLE) return false;
		rasterizer_.LoadBlockTextures(block_texture_directory);
		if (!CreateCommandResources() || !CreateSynchronization() || !CreateTimestampQueries())
		{
			Release();
			return false;
		}
		image_layouts_.assign(backend.Swapchain().Images().size(), VK_IMAGE_LAYOUT_UNDEFINED);
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Vulkan voxel frame renderer initialized");
		return true;
	}

	bool VulkanFrameRenderer::CreateTimestampQueries()
	{
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(backend_->PhysicalDevice().Handle(), &properties);
		timestamp_period_ns_ = properties.limits.timestampPeriod;
		if (timestamp_period_ns_ <= 0.0f) return true;
		std::uint32_t family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(backend_->PhysicalDevice().Handle(), &family_count, nullptr);
		ve::core::DynamicArray<VkQueueFamilyProperties> families(family_count);
		if (family_count > 0) vkGetPhysicalDeviceQueueFamilyProperties(backend_->PhysicalDevice().Handle(), &family_count, families.data());
		const std::uint32_t graphics_family = backend_->PhysicalDevice().QueueFamilies().graphics_family;
		if (graphics_family >= families.size() || families[graphics_family].timestampValidBits == 0u)
		{
			timestamp_period_ns_ = 0.0f;
			return true;
		}

		VkQueryPoolCreateInfo query_info{ VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO };
		query_info.queryType = VK_QUERY_TYPE_TIMESTAMP;
		query_info.queryCount = static_cast<std::uint32_t>(kFramesInFlight * 2u);
		if (vkCreateQueryPool(device_, &query_info, nullptr, &timestamp_query_pool_) != VK_SUCCESS)
		{
			timestamp_query_pool_ = VK_NULL_HANDLE;
			timestamp_period_ns_ = 0.0f;
		}
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
		const std::size_t swapchain_image_count = backend_->Swapchain().Images().size();
		if (swapchain_image_count == 0) return false;
		render_finished_semaphores_.assign(swapchain_image_count, VK_NULL_HANDLE);
		for (std::size_t index = 0; index < kFramesInFlight; ++index)
		{
			if (vkCreateSemaphore(device_, &semaphore_info, nullptr, &image_available_semaphores_[index]) != VK_SUCCESS ||
				vkCreateFence(device_, &fence_info, nullptr, &in_flight_fences_[index]) != VK_SUCCESS)
			{
				return false;
			}
		}
		for (VkSemaphore& semaphore : render_finished_semaphores_)
		{
			if (vkCreateSemaphore(device_, &semaphore_info, nullptr, &semaphore) != VK_SUCCESS) return false;
		}
		return true;
	}

	bool VulkanFrameRenderer::EnsureFrameBuffer(VkExtent2D extent)
	{
		if (!rasterizer_.Resize(extent, demo_settings_)) return false;
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

	bool VulkanFrameRenderer::UploadFramePixels(VulkanFrameTiming& timing)
	{
		const std::span<const std::uint32_t> pixels = rasterizer_.Pixels();
		const auto start = std::chrono::steady_clock::now();
		const bool copied = upload_buffer_.CopyFrom(device_, pixels.data(), pixels.size_bytes());
		const auto end = std::chrono::steady_clock::now();
		timing.upload_cpu_ms = std::chrono::duration<double, std::milli>(end - start).count();
		return copied;
	}

	bool VulkanFrameRenderer::RecordCommandBuffer(VkCommandBuffer command_buffer, std::uint32_t image_index, std::size_t frame_index)
	{
		VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) return false;

		const std::uint32_t first_query = static_cast<std::uint32_t>(frame_index * 2u);
		if (timestamp_query_pool_ != VK_NULL_HANDLE)
		{
			vkCmdResetQueryPool(command_buffer, timestamp_query_pool_, first_query, 2u);
			vkCmdWriteTimestamp(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, timestamp_query_pool_, first_query);
		}

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

		if (timestamp_query_pool_ != VK_NULL_HANDLE)
		{
			vkCmdWriteTimestamp(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, timestamp_query_pool_, first_query + 1u);
		}

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

	void VulkanFrameRenderer::CaptureCompletedGpuTiming(std::size_t frame_index, VulkanFrameTiming& timing) const
	{
		if (timestamp_query_pool_ == VK_NULL_HANDLE || timestamp_period_ns_ <= 0.0f) return;
		if (!timestamp_query_valid_[frame_index]) return;
		std::array<std::uint64_t, 2> timestamps{};
		const VkResult result = vkGetQueryPoolResults(device_,
			timestamp_query_pool_,
			static_cast<std::uint32_t>(frame_index * 2u),
			static_cast<std::uint32_t>(timestamps.size()),
			sizeof(std::uint64_t) * timestamps.size(),
			timestamps.data(),
			sizeof(std::uint64_t),
			VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);
		if (result != VK_SUCCESS || timestamps[1] <= timestamps[0]) return;
		const double elapsed_ns = static_cast<double>(timestamps[1] - timestamps[0]) * static_cast<double>(timestamp_period_ns_);
		timing.gpu_copy_ms = elapsed_ns / 1'000'000.0;
		timing.has_gpu_copy_timing = true;
	}

	bool VulkanFrameRenderer::DrawFrame(const ve::world::World& world,
		const Camera& camera,
		int displayed_fps,
		double delta_seconds,
		const VulkanDemoInput& input)
	{
		if (backend_ == nullptr || device_ == VK_NULL_HANDLE) return false;
		const VkExtent2D extent = backend_->Swapchain().Extent();
		if (!EnsureFrameBuffer(extent)) return false;
		rasterizer_.Render(VulkanSoftwareVoxelRasterizerFrame{ world, camera, extent, backend_->Swapchain().ImageFormat(), displayed_fps, delta_seconds, previous_frame_timing_, input, demo_settings_ });
		VulkanFrameTiming current_timing = rasterizer_.LastTiming();
		if (!UploadFramePixels(current_timing)) return false;

		const VkFence fence = in_flight_fences_[current_frame_];
		if (vkWaitForFences(device_, 1, &fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) return false;
		CaptureCompletedGpuTiming(current_frame_, current_timing);

		std::uint32_t image_index = 0;
		const auto present_start = std::chrono::steady_clock::now();
		const VkResult acquire_result = vkAcquireNextImageKHR(device_, backend_->Swapchain().Handle(), UINT64_MAX,
			image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &image_index);
		if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR) return false;
		if (image_index >= image_layouts_.size()) return false;
		if (image_index >= render_finished_semaphores_.size()) return false;

		if (vkResetFences(device_, 1, &fence) != VK_SUCCESS) return false;
		VkCommandBuffer command_buffer = command_buffers_[current_frame_];
		if (vkResetCommandBuffer(command_buffer, 0) != VK_SUCCESS || !RecordCommandBuffer(command_buffer, image_index, current_frame_))
		{
			return false;
		}

		const VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		const VkSemaphore image_available = image_available_semaphores_[current_frame_];
		const VkSemaphore render_finished = render_finished_semaphores_[image_index];
		VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &image_available;
		submit_info.pWaitDstStageMask = &wait_stage;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &render_finished;
		if (vkQueueSubmit(backend_->Device().GraphicsQueue(), 1, &submit_info, fence) != VK_SUCCESS) return false;

		VkPresentInfoKHR present_info{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &render_finished;
		const VkSwapchainKHR swapchain = backend_->Swapchain().Handle();
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &swapchain;
		present_info.pImageIndices = &image_index;
		const VkResult present_result = vkQueuePresentKHR(backend_->Device().PresentQueue(), &present_info);
		const auto present_end = std::chrono::steady_clock::now();
		current_timing.present_cpu_ms = std::chrono::duration<double, std::milli>(present_end - present_start).count();
		if (timestamp_query_pool_ != VK_NULL_HANDLE) timestamp_query_valid_[current_frame_] = true;
		previous_frame_timing_ = current_timing;
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
		if (timestamp_query_pool_ != VK_NULL_HANDLE) vkDestroyQueryPool(device_, timestamp_query_pool_, nullptr);
		timestamp_query_pool_ = VK_NULL_HANDLE;
		timestamp_period_ns_ = 0.0f;
		timestamp_query_valid_.fill(false);
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
		render_finished_semaphores_.clear();
		for (VkSemaphore& semaphore : image_available_semaphores_)
		{
			if (semaphore != VK_NULL_HANDLE) vkDestroySemaphore(device_, semaphore, nullptr);
			semaphore = VK_NULL_HANDLE;
		}
		if (command_pool_ != VK_NULL_HANDLE) vkDestroyCommandPool(device_, command_pool_, nullptr);
		command_pool_ = VK_NULL_HANDLE;
		command_buffers_.fill(VK_NULL_HANDLE);
		image_layouts_.clear();
		rasterizer_.Release();
		previous_frame_timing_ = {};
		demo_settings_ = {};
		device_ = VK_NULL_HANDLE;
		backend_ = nullptr;
		current_frame_ = 0;
		logged_first_frame_ = false;
	}
}
