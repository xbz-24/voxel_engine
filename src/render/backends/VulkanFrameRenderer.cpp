#include "VulkanFrameRenderer.h"

#include "Camera.h"
#include "Logger.h"
#include "VulkanBackend.h"
#include "VulkanFrameRendererCommands.h"
#include "Window.h"
#include "World.h"

#include <chrono>
#include <span>
#include <string>
#include <vulkan/vulkan.hpp>

namespace ve::rendering
{
	VulkanFrameRenderer::~VulkanFrameRenderer() { Release(); }

	bool VulkanFrameRenderer::Initialize(VulkanBackend& backend, ve::engine::Window& window, const std::filesystem::path& block_texture_directory)
	{
		Release();
		backend_ = &backend;
		device_ = backend.Device().Handle();
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
		else if (!imgui_overlay_.Initialize(backend, window, gpu_chunk_renderer_.RenderPass()))
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Vulkan ImGui overlay failed; continuing without live demo controls");
		}
		image_layouts_.assign(backend.Swapchain().Images().size(), VK_IMAGE_LAYOUT_UNDEFINED);
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Vulkan voxel frame renderer initialized");
		return true;
	}

	bool VulkanFrameRenderer::CreateTimestampQueries()
	{
		const vk::PhysicalDevice physical_device = backend_->PhysicalDevice().CppHandle();
		const vk::PhysicalDeviceProperties properties = physical_device.getProperties();
		timestamp_period_ns_ = properties.limits.timestampPeriod;
		if (timestamp_period_ns_ <= 0.0f) return true;
		const std::vector<vk::QueueFamilyProperties> families = physical_device.getQueueFamilyProperties();
		const std::uint32_t graphics_family = backend_->PhysicalDevice().QueueFamilies().graphics_family;
		if (graphics_family >= families.size() || families[graphics_family].timestampValidBits == 0u)
		{
			timestamp_period_ns_ = 0.0f;
			return true;
		}

		const vk::QueryPoolCreateInfo query_info{ {}, vk::QueryType::eTimestamp, static_cast<std::uint32_t>(kFramesInFlight * 2u) };
		vk::QueryPool query_pool{};
		if (backend_->Device().CppHandle().createQueryPool(&query_info, nullptr, &query_pool) != vk::Result::eSuccess)
		{
			timestamp_query_pool_ = VK_NULL_HANDLE;
			timestamp_period_ns_ = 0.0f;
		}
		else
		{
			timestamp_query_pool_ = query_pool;
		}
		return true;
	}

	bool VulkanFrameRenderer::CreateCommandResources()
	{
		const vk::CommandPoolCreateInfo pool_info{
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			backend_->PhysicalDevice().QueueFamilies().graphics_family
		};
		vk::CommandPool command_pool{};
		if (backend_->Device().CppHandle().createCommandPool(&pool_info, nullptr, &command_pool) != vk::Result::eSuccess) return false;
		command_pool_ = command_pool;

		VkCommandBufferAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocate_info.commandPool = command_pool_;
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandBufferCount = static_cast<std::uint32_t>(command_buffers_.size());
		return vkAllocateCommandBuffers(device_, &allocate_info, command_buffers_.data()) == VK_SUCCESS;
	}

	bool VulkanFrameRenderer::CreateSynchronization()
	{
		const vk::Device device = backend_->Device().CppHandle();
		const vk::SemaphoreCreateInfo semaphore_info{};
		const vk::FenceCreateInfo fence_info{ vk::FenceCreateFlagBits::eSignaled };
		const std::size_t swapchain_image_count = backend_->Swapchain().Images().size();
		if (swapchain_image_count == 0) return false;
		render_finished_semaphores_.assign(swapchain_image_count, VK_NULL_HANDLE);
		for (std::size_t index = 0; index < kFramesInFlight; ++index)
		{
			vk::Semaphore image_available{};
			vk::Fence in_flight{};
			if (device.createSemaphore(&semaphore_info, nullptr, &image_available) != vk::Result::eSuccess ||
				device.createFence(&fence_info, nullptr, &in_flight) != vk::Result::eSuccess)
			{
				return false;
			}
			image_available_semaphores_[index] = image_available;
			in_flight_fences_[index] = in_flight;
		}
		for (VkSemaphore& semaphore : render_finished_semaphores_)
		{
			vk::Semaphore render_finished{};
			if (device.createSemaphore(&semaphore_info, nullptr, &render_finished) != vk::Result::eSuccess) return false;
			semaphore = render_finished;
		}
		return true;
	}

	std::uint32_t VulkanFrameRenderer::FindMemoryType(VkPhysicalDevice physical_device, std::uint32_t type_filter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memory_properties{};
		vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
		for (std::uint32_t index = 0; index < memory_properties.memoryTypeCount; ++index)
		{
			const bool is_allowed = (type_filter & (1u << index)) != 0;
			const bool has_properties = (memory_properties.memoryTypes[index].propertyFlags & properties) == properties;
			if (is_allowed && has_properties) return index;
		}
		return UINT32_MAX;
	}

	bool VulkanFrameRenderer::EnsureIntermediateImages(VkExtent2D extent, VkFormat format)
	{
		if (extent.width == 0u || extent.height == 0u || format == VK_FORMAT_UNDEFINED) return false;
		if (intermediate_images_.front() != VK_NULL_HANDLE &&
			intermediate_extent_.width == extent.width &&
			intermediate_extent_.height == extent.height &&
			intermediate_format_ == format)
		{
			return true;
		}

		if (device_ != VK_NULL_HANDLE) vkDeviceWaitIdle(device_);
		ReleaseIntermediateImages();

		VkFormatProperties format_properties{};
		vkGetPhysicalDeviceFormatProperties(backend_->PhysicalDevice().Handle(), format, &format_properties);
		upscale_filter_ = (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) != 0u ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;

		for (std::size_t index = 0; index < kFramesInFlight; ++index)
		{
			VkImageCreateInfo image_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
			image_info.imageType = VK_IMAGE_TYPE_2D;
			image_info.format = format;
			image_info.extent = { extent.width, extent.height, 1u };
			image_info.mipLevels = 1;
			image_info.arrayLayers = 1;
			image_info.samples = VK_SAMPLE_COUNT_1_BIT;
			image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
			image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			if (vkCreateImage(device_, &image_info, nullptr, &intermediate_images_[index]) != VK_SUCCESS)
			{
				ReleaseIntermediateImages();
				return false;
			}

			VkMemoryRequirements requirements{};
			vkGetImageMemoryRequirements(device_, intermediate_images_[index], &requirements);
			const std::uint32_t memory_type = FindMemoryType(backend_->PhysicalDevice().Handle(), requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			if (memory_type == UINT32_MAX)
			{
				ReleaseIntermediateImages();
				return false;
			}

			VkMemoryAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
			allocate_info.allocationSize = requirements.size;
			allocate_info.memoryTypeIndex = memory_type;
			if (vkAllocateMemory(device_, &allocate_info, nullptr, &intermediate_image_memory_[index]) != VK_SUCCESS ||
				vkBindImageMemory(device_, intermediate_images_[index], intermediate_image_memory_[index], 0) != VK_SUCCESS)
			{
				ReleaseIntermediateImages();
				return false;
			}
			intermediate_image_layouts_[index] = VK_IMAGE_LAYOUT_UNDEFINED;
		}

		intermediate_extent_ = extent;
		intermediate_format_ = format;
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, std::string("Allocated Vulkan internal upscale image: ") +
			std::to_string(extent.width) + "x" + std::to_string(extent.height));
		return true;
	}

	bool VulkanFrameRenderer::EnsureFrameBuffer(VkExtent2D extent)
	{
		if (!rasterizer_.Resize(extent, demo_settings_)) return false;
		const VkExtent2D render_extent = rasterizer_.RenderExtent();
		const VkDeviceSize byte_size = static_cast<VkDeviceSize>(render_extent.width) * static_cast<VkDeviceSize>(render_extent.height) * sizeof(std::uint32_t);
		const bool size_changed = upload_buffers_.front().Size() != byte_size;
		for (VulkanUploadBuffer& upload_buffer : upload_buffers_)
		{
			if (!upload_buffer.Ensure(*backend_, byte_size)) return false;
		}
		if (!EnsureIntermediateImages(render_extent, backend_->Swapchain().ImageFormat())) return false;
		if (size_changed)
		{
			VE_LOG_CATEGORY_INFO(ve::log::category::Render, std::string("Allocated Vulkan CPU upload framebuffer: ") +
				std::to_string(render_extent.width) + "x" + std::to_string(render_extent.height));
		}
		return true;
	}

	bool VulkanFrameRenderer::UploadFramePixels(VulkanFrameTiming& timing, std::size_t frame_index)
	{
		if (frame_index >= upload_buffers_.size()) return false;
		const std::span<const std::uint32_t> pixels = rasterizer_.RenderPixels();
		const auto start = std::chrono::steady_clock::now();
		const bool copied = upload_buffers_[frame_index].CopyFrom(device_, pixels.data(), pixels.size_bytes());
		const auto end = std::chrono::steady_clock::now();
		timing.upload_cpu_ms = std::chrono::duration<double, std::milli>(end - start).count();
		return copied;
	}

	bool VulkanFrameRenderer::RecordSoftwareCommandBuffer(VkCommandBuffer command_buffer, std::uint32_t image_index, std::size_t frame_index)
	{
		if (frame_index >= intermediate_images_.size() || intermediate_images_[frame_index] == VK_NULL_HANDLE) return false;
		VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) return false;

		const std::uint32_t first_query = static_cast<std::uint32_t>(frame_index * 2u);
		if (timestamp_query_pool_ != VK_NULL_HANDLE)
		{
			vkCmdResetQueryPool(command_buffer, timestamp_query_pool_, first_query, 2u);
			vkCmdWriteTimestamp(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, timestamp_query_pool_, first_query);
		}

		VkImageMemoryBarrier internal_to_transfer{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		internal_to_transfer.oldLayout = intermediate_image_layouts_[frame_index];
		internal_to_transfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		internal_to_transfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		internal_to_transfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		internal_to_transfer.image = intermediate_images_[frame_index];
		internal_to_transfer.subresourceRange = ColorSubresourceRange();
		internal_to_transfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &internal_to_transfer);

		const VkExtent2D source_extent = rasterizer_.RenderExtent();
		VkBufferImageCopy copy_region{};
		copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy_region.imageSubresource.layerCount = 1;
		copy_region.imageExtent = { source_extent.width, source_extent.height, 1 };
		vkCmdCopyBufferToImage(command_buffer, upload_buffers_[frame_index].Buffer(), intermediate_images_[frame_index],
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

		VkImageMemoryBarrier internal_to_source{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		internal_to_source.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		internal_to_source.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		internal_to_source.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		internal_to_source.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		internal_to_source.image = intermediate_images_[frame_index];
		internal_to_source.subresourceRange = ColorSubresourceRange();
		internal_to_source.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		internal_to_source.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &internal_to_source);

		VkImageMemoryBarrier to_transfer{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		to_transfer.oldLayout = image_layouts_[image_index];
		to_transfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		to_transfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		to_transfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		to_transfer.image = backend_->Swapchain().Images()[image_index];
		to_transfer.subresourceRange = ColorSubresourceRange();
		to_transfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &to_transfer);

		const VkExtent2D destination_extent = rasterizer_.Extent();
		VkImageBlit blit_region{};
		blit_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit_region.srcSubresource.layerCount = 1;
		blit_region.srcOffsets[1] = {
			static_cast<std::int32_t>(source_extent.width),
			static_cast<std::int32_t>(source_extent.height),
			1
		};
		blit_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit_region.dstSubresource.layerCount = 1;
		blit_region.dstOffsets[1] = {
			static_cast<std::int32_t>(destination_extent.width),
			static_cast<std::int32_t>(destination_extent.height),
			1
		};
		vkCmdBlitImage(command_buffer,
			intermediate_images_[frame_index],
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			backend_->Swapchain().Images()[image_index],
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&blit_region,
			upscale_filter_);

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
		to_present.subresourceRange = ColorSubresourceRange();
		to_present.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0, 0, nullptr, 0, nullptr, 1, &to_present);
		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) return false;
		intermediate_image_layouts_[frame_index] = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		image_layouts_[image_index] = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		return true;
	}

	bool VulkanFrameRenderer::RecordGpuCommandBuffer(VkCommandBuffer command_buffer, std::uint32_t image_index, std::size_t frame_index, const Camera& camera)
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
		const VulkanOverlayRecordCallback overlay_callback = imgui_overlay_.IsInitialized() ? RecordImguiOverlay : nullptr;
		void* overlay_user_data = imgui_overlay_.IsInitialized() ? &imgui_overlay_ : nullptr;
		if (!gpu_chunk_renderer_.Record(command_buffer, image_index, camera, overlay_callback, overlay_user_data)) return false;
		if (timestamp_query_pool_ != VK_NULL_HANDLE)
		{
			vkCmdWriteTimestamp(command_buffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, timestamp_query_pool_, first_query + 1u);
		}
		return vkEndCommandBuffer(command_buffer) == VK_SUCCESS;
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
		const VulkanDemoInput& input,
		VulkanMinecraftDemoSettings& minecraft_demo_settings)
	{
		// TODO: Give the GPU path its own explicit input/overlay contract instead of silently ignoring VulkanDemoInput.
		if (gpu_chunk_renderer_.IsInitialized()) return DrawGpuFrame(world, camera, displayed_fps, delta_seconds, minecraft_demo_settings);
		return DrawSoftwareFrame(world, camera, displayed_fps, delta_seconds, input);
	}

	bool VulkanFrameRenderer::DrawGpuFrame(const ve::world::World& world,
		const Camera& camera,
		int displayed_fps,
		double delta_seconds,
		VulkanMinecraftDemoSettings& minecraft_demo_settings)
	{
		if (backend_ == nullptr || device_ == VK_NULL_HANDLE) return false;
		const VkFence fence = in_flight_fences_[current_frame_];
		if (vkWaitForFences(device_, 1, &fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) return false;
		VulkanFrameTiming completed_frame_timing = previous_frame_timing_;
		CaptureCompletedGpuTiming(current_frame_, completed_frame_timing);
		if (!gpu_chunk_renderer_.EnsureWorldMesh(world)) return false;
		imgui_overlay_.BeginFrame(minecraft_demo_settings, VulkanMinecraftDemoStats{
			displayed_fps,
			delta_seconds,
			completed_frame_timing.gpu_copy_ms,
			completed_frame_timing.present_cpu_ms,
			gpu_chunk_renderer_.IndexCount(),
			world.Revision(),
			completed_frame_timing.has_gpu_copy_timing,
			true
		});

		std::uint32_t image_index = 0;
		const auto present_start = std::chrono::steady_clock::now();
		const VkResult acquire_result = vkAcquireNextImageKHR(device_, backend_->Swapchain().Handle(), UINT64_MAX,
			image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &image_index);
		if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR) return false;
		if (image_index >= render_finished_semaphores_.size()) return false;

		if (vkResetFences(device_, 1, &fence) != VK_SUCCESS) return false;
		VkCommandBuffer command_buffer = command_buffers_[current_frame_];
		if (vkResetCommandBuffer(command_buffer, 0) != VK_SUCCESS || !RecordGpuCommandBuffer(command_buffer, image_index, current_frame_, camera))
		{
			return false;
		}

		const VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		const VkSemaphore image_available = image_available_semaphores_[current_frame_];
		const VkSemaphore render_finished = render_finished_semaphores_[image_index];
		VkSubmitInfo submit_info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submit_info.waitSemaphoreCount = 1u;
		submit_info.pWaitSemaphores = &image_available;
		submit_info.pWaitDstStageMask = &wait_stage;
		submit_info.commandBufferCount = 1u;
		submit_info.pCommandBuffers = &command_buffer;
		submit_info.signalSemaphoreCount = 1u;
		submit_info.pSignalSemaphores = &render_finished;
		if (vkQueueSubmit(backend_->Device().GraphicsQueue(), 1u, &submit_info, fence) != VK_SUCCESS) return false;

		VkPresentInfoKHR present_info{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		present_info.waitSemaphoreCount = 1u;
		present_info.pWaitSemaphores = &render_finished;
		const VkSwapchainKHR swapchain = backend_->Swapchain().Handle();
		present_info.swapchainCount = 1u;
		present_info.pSwapchains = &swapchain;
		present_info.pImageIndices = &image_index;
		const VkResult present_result = vkQueuePresentKHR(backend_->Device().PresentQueue(), &present_info);
		const auto present_end = std::chrono::steady_clock::now();

		VulkanFrameTiming current_timing{};
		current_timing.gpu_copy_ms = completed_frame_timing.gpu_copy_ms;
		current_timing.has_gpu_copy_timing = completed_frame_timing.has_gpu_copy_timing;
		current_timing.present_cpu_ms = std::chrono::duration<double, std::milli>(present_end - present_start).count();
		current_timing.render_extent = backend_->Swapchain().Extent();
		current_timing.sample_step = 1u;
		current_timing.worker_count = 0u;
		if (timestamp_query_pool_ != VK_NULL_HANDLE) timestamp_query_valid_[current_frame_] = true;
		previous_frame_timing_ = current_timing;
		current_frame_ = (current_frame_ + 1u) % kFramesInFlight;
		if (!logged_first_frame_)
		{
			VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Presented first Vulkan GPU chunk frame");
			logged_first_frame_ = true;
		}
		return present_result == VK_SUCCESS || present_result == VK_SUBOPTIMAL_KHR;
	}

	bool VulkanFrameRenderer::WantsMouseInput() const noexcept
	{
		return imgui_overlay_.WantsMouseInput();
	}

	bool VulkanFrameRenderer::DrawSoftwareFrame(const ve::world::World& world,
		const Camera& camera,
		int displayed_fps,
		double delta_seconds,
		const VulkanDemoInput& input)
	{
		if (backend_ == nullptr || device_ == VK_NULL_HANDLE) return false;
		const VkFence fence = in_flight_fences_[current_frame_];
		if (vkWaitForFences(device_, 1, &fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) return false;
		VulkanFrameTiming completed_frame_timing = previous_frame_timing_;
		CaptureCompletedGpuTiming(current_frame_, completed_frame_timing);

		const VkExtent2D extent = backend_->Swapchain().Extent();
		if (!EnsureFrameBuffer(extent)) return false;
		rasterizer_.Render(VulkanSoftwareVoxelRasterizerFrame{ world, camera, extent, backend_->Swapchain().ImageFormat(), displayed_fps, delta_seconds, completed_frame_timing, input, demo_settings_ });
		VulkanFrameTiming current_timing = rasterizer_.LastTiming();
		current_timing.gpu_copy_ms = completed_frame_timing.gpu_copy_ms;
		current_timing.has_gpu_copy_timing = completed_frame_timing.has_gpu_copy_timing;
		if (!UploadFramePixels(current_timing, current_frame_)) return false;

		std::uint32_t image_index = 0;
		const auto present_start = std::chrono::steady_clock::now();
		const VkResult acquire_result = vkAcquireNextImageKHR(device_, backend_->Swapchain().Handle(), UINT64_MAX,
			image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &image_index);
		if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR) return false;
		if (image_index >= image_layouts_.size()) return false;
		if (image_index >= render_finished_semaphores_.size()) return false;

		if (vkResetFences(device_, 1, &fence) != VK_SUCCESS) return false;
		VkCommandBuffer command_buffer = command_buffers_[current_frame_];
		if (vkResetCommandBuffer(command_buffer, 0) != VK_SUCCESS || !RecordSoftwareCommandBuffer(command_buffer, image_index, current_frame_))
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

	void VulkanFrameRenderer::ReleaseIntermediateImages()
	{
		for (std::size_t index = 0; index < kFramesInFlight; ++index)
		{
			if (intermediate_images_[index] != VK_NULL_HANDLE) vkDestroyImage(device_, intermediate_images_[index], nullptr);
			if (intermediate_image_memory_[index] != VK_NULL_HANDLE) vkFreeMemory(device_, intermediate_image_memory_[index], nullptr);
			intermediate_images_[index] = VK_NULL_HANDLE;
			intermediate_image_memory_[index] = VK_NULL_HANDLE;
			intermediate_image_layouts_[index] = VK_IMAGE_LAYOUT_UNDEFINED;
		}
		intermediate_extent_ = {};
		intermediate_format_ = VK_FORMAT_UNDEFINED;
		upscale_filter_ = VK_FILTER_NEAREST;
	}

	void VulkanFrameRenderer::Release()
	{
		if (device_ != VK_NULL_HANDLE) vkDeviceWaitIdle(device_);
		ReleaseIntermediateImages();
		for (VulkanUploadBuffer& upload_buffer : upload_buffers_)
		{
			upload_buffer.Release(device_);
		}
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
		imgui_overlay_.Release();
		gpu_chunk_renderer_.Release();
		rasterizer_.Release();
		previous_frame_timing_ = {};
		demo_settings_ = {};
		device_ = VK_NULL_HANDLE;
		backend_ = nullptr;
		current_frame_ = 0;
		logged_first_frame_ = false;
	}
}
