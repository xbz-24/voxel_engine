#include "VulkanFrameRenderer.h"

#include "BlockDefinitions.h"
#include "Camera.h"
#include "Logger.h"
#include "VulkanBackend.h"
#include "World.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <string>

namespace ve::rendering
{
	namespace
	{
		struct Rgb
		{
			std::uint8_t r;
			std::uint8_t g;
			std::uint8_t b;
		};

		struct RayHit
		{
			bool hit = false;
			ve::blocks::BlockId block = ve::blocks::BlockId::Air;
			glm::ivec3 normal{ 0, 1, 0 };
			float distance = 0.0f;
		};

		constexpr float kMaxRayDistance = 150.0f;
		constexpr int kMaxRaySteps = 256;

		bool IsBgraFormat(VkFormat format) noexcept
		{
			return format == VK_FORMAT_B8G8R8A8_UNORM ||
				format == VK_FORMAT_B8G8R8A8_SRGB ||
				format == VK_FORMAT_B8G8R8A8_SNORM;
		}

		std::uint32_t PackColor(Rgb color, VkFormat format) noexcept
		{
			constexpr std::uint32_t alpha = 255u;
			if (IsBgraFormat(format))
			{
				return (alpha << 24u) |
					(static_cast<std::uint32_t>(color.r) << 16u) |
					(static_cast<std::uint32_t>(color.g) << 8u) |
					static_cast<std::uint32_t>(color.b);
			}
			return (alpha << 24u) |
				(static_cast<std::uint32_t>(color.b) << 16u) |
				(static_cast<std::uint32_t>(color.g) << 8u) |
				static_cast<std::uint32_t>(color.r);
		}

		std::uint8_t Scale(std::uint8_t value, float amount) noexcept
		{
			const float scaled = std::clamp(static_cast<float>(value) * amount, 0.0f, 255.0f);
			return static_cast<std::uint8_t>(scaled);
		}

		Rgb Mix(Rgb left, Rgb right, float amount) noexcept
		{
			const float t = std::clamp(amount, 0.0f, 1.0f);
			const auto lerp = [t](std::uint8_t a, std::uint8_t b)
			{
				return static_cast<std::uint8_t>((static_cast<float>(a) * (1.0f - t)) + (static_cast<float>(b) * t));
			};
			return { lerp(left.r, right.r), lerp(left.g, right.g), lerp(left.b, right.b) };
		}

		Rgb SkyColor(float direction_y) noexcept
		{
			const float t = std::clamp((direction_y + 0.15f) * 0.75f, 0.0f, 1.0f);
			return Mix({ 94, 133, 177 }, { 160, 205, 255 }, t);
		}

		bool IsSolid(ve::blocks::BlockId block) noexcept
		{
			const auto index = static_cast<std::size_t>(block);
			return index < static_cast<std::size_t>(ve::blocks::BlockId::Count) &&
				ve::blocks::BuiltInBlockDefinitions[index].isSolid;
		}

		Rgb BaseBlockColor(ve::blocks::BlockId block) noexcept
		{
			using enum ve::blocks::BlockId;
			if (block == Grass) return { 92, 159, 74 };
			if (block == Dirt) return { 112, 76, 45 };
			if (block == Stone) return { 124, 124, 124 };
			if (block == Cobblestone) return { 105, 105, 105 };
			if (block == OakLog || block == BirchLog || block == SpruceLog) return { 123, 89, 50 };
			if (block == OakPlanks || block == BirchPlanks || block == SprucePlanks) return { 174, 134, 82 };
			if (block == Sand) return { 214, 198, 129 };
			if (block == Bricks) return { 151, 74, 60 };
			if (block == DiamondOre) return { 92, 196, 211 };
			if (block == CoalOre) return { 54, 54, 54 };
			if (block == IronOre) return { 216, 170, 130 };
			if (block == Obsidian) return { 42, 32, 65 };
			if (block == OakLeaves || block == BirchLeaves) return { 67, 137, 58 };
			if (block == Snow) return { 235, 242, 246 };
			if (block == MossBlock) return { 73, 126, 58 };
			if (block == Pumpkin) return { 198, 112, 33 };
			if (block == Melon) return { 104, 158, 53 };
			if (block == HayBlock) return { 203, 174, 63 };
			if (block == AmethystBlock) return { 137, 89, 178 };
			if (block == Glass) return { 148, 214, 219 };
			return { 132, 132, 132 };
		}

		int FloorToInt(float value) noexcept
		{
			return static_cast<int>(std::floor(value));
		}

		float InitialAxisDistance(float origin, float direction) noexcept
		{
			if (direction > 0.0f) return (std::floor(origin) + 1.0f - origin) / direction;
			if (direction < 0.0f) return (origin - std::floor(origin)) / -direction;
			return std::numeric_limits<float>::infinity();
		}

		float AxisDelta(float direction) noexcept
		{
			return direction == 0.0f ? std::numeric_limits<float>::infinity() : std::abs(1.0f / direction);
		}

		glm::vec3 FindAirCameraOrigin(const ve::world::World& world, glm::vec3 origin)
		{
			for (int attempts = 0; attempts < 32; ++attempts)
			{
				const auto block = world.GetBlock(FloorToInt(origin.x), FloorToInt(origin.y), FloorToInt(origin.z));
				if (!IsSolid(block)) return origin;
				origin.y += 1.0f;
			}
			return origin;
		}

		RayHit TraceWorld(const ve::world::World& world, const glm::vec3& origin, const glm::vec3& direction)
		{
			glm::ivec3 block{ FloorToInt(origin.x), FloorToInt(origin.y), FloorToInt(origin.z) };
			const glm::ivec3 step{
				direction.x > 0.0f ? 1 : (direction.x < 0.0f ? -1 : 0),
				direction.y > 0.0f ? 1 : (direction.y < 0.0f ? -1 : 0),
				direction.z > 0.0f ? 1 : (direction.z < 0.0f ? -1 : 0)
			};
			glm::vec3 max_distance{
				InitialAxisDistance(origin.x, direction.x),
				InitialAxisDistance(origin.y, direction.y),
				InitialAxisDistance(origin.z, direction.z)
			};
			const glm::vec3 delta{
				AxisDelta(direction.x),
				AxisDelta(direction.y),
				AxisDelta(direction.z)
			};
			glm::ivec3 normal{ 0, 1, 0 };
			float distance = 0.0f;

			for (int ray_step = 0; ray_step < kMaxRaySteps && distance <= kMaxRayDistance; ++ray_step)
			{
				const ve::blocks::BlockId block_id = world.GetBlock(block);
				if (IsSolid(block_id)) return { true, block_id, normal, distance };

				if (max_distance.x < max_distance.y && max_distance.x < max_distance.z)
				{
					block.x += step.x;
					distance = max_distance.x;
					max_distance.x += delta.x;
					normal = { -step.x, 0, 0 };
				}
				else if (max_distance.y < max_distance.z)
				{
					block.y += step.y;
					distance = max_distance.y;
					max_distance.y += delta.y;
					normal = { 0, -step.y, 0 };
				}
				else
				{
					block.z += step.z;
					distance = max_distance.z;
					max_distance.z += delta.z;
					normal = { 0, 0, -step.z };
				}
			}
			return {};
		}

		Rgb ShadeHit(const RayHit& hit, const glm::vec3& direction) noexcept
		{
			float shade = 0.72f;
			if (hit.normal.y > 0) shade = 1.05f;
			else if (hit.normal.y < 0) shade = 0.45f;
			else if (hit.normal.x != 0) shade = 0.78f;

			Rgb color = BaseBlockColor(hit.block);
			color = { Scale(color.r, shade), Scale(color.g, shade), Scale(color.b, shade) };
			const float fog = std::clamp(hit.distance / kMaxRayDistance, 0.0f, 1.0f);
			return Mix(color, SkyColor(direction.y), fog * 0.55f);
		}

		std::uint32_t FindMemoryType(VkPhysicalDevice physical_device, std::uint32_t type_filter, VkMemoryPropertyFlags properties)
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
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Vulkan software voxel frame renderer initialized");
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
		if (extent.width == 0 || extent.height == 0) return false;
		const VkDeviceSize byte_size = static_cast<VkDeviceSize>(extent.width) * static_cast<VkDeviceSize>(extent.height) * sizeof(std::uint32_t);
		if (pixel_extent_.width == extent.width && pixel_extent_.height == extent.height && upload_buffer_ != VK_NULL_HANDLE)
		{
			return true;
		}

		DestroyUploadBuffer();
		pixel_extent_ = extent;
		pixels_.assign(static_cast<std::size_t>(extent.width) * static_cast<std::size_t>(extent.height), 0u);
		if (!CreateUploadBuffer(byte_size)) return false;
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, std::string("Allocated Vulkan CPU upload framebuffer: ") +
			std::to_string(extent.width) + "x" + std::to_string(extent.height));
		return true;
	}

	bool VulkanFrameRenderer::CreateUploadBuffer(VkDeviceSize byte_size)
	{
		VkBufferCreateInfo buffer_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		buffer_info.size = byte_size;
		buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (vkCreateBuffer(device_, &buffer_info, nullptr, &upload_buffer_) != VK_SUCCESS) return false;

		VkMemoryRequirements requirements{};
		vkGetBufferMemoryRequirements(device_, upload_buffer_, &requirements);
		const std::uint32_t memory_type = FindMemoryType(backend_->PhysicalDevice().Handle(), requirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (memory_type == UINT32_MAX) return false;

		VkMemoryAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocate_info.allocationSize = requirements.size;
		allocate_info.memoryTypeIndex = memory_type;
		if (vkAllocateMemory(device_, &allocate_info, nullptr, &upload_memory_) != VK_SUCCESS) return false;
		if (vkBindBufferMemory(device_, upload_buffer_, upload_memory_, 0) != VK_SUCCESS) return false;
		upload_buffer_size_ = byte_size;
		return true;
	}

	bool VulkanFrameRenderer::UploadFramePixels()
	{
		void* mapped_memory = nullptr;
		if (vkMapMemory(device_, upload_memory_, 0, upload_buffer_size_, 0, &mapped_memory) != VK_SUCCESS) return false;
		std::memcpy(mapped_memory, pixels_.data(), pixels_.size() * sizeof(std::uint32_t));
		vkUnmapMemory(device_, upload_memory_);
		return true;
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

		VkBufferImageCopy copy_region{};
		copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy_region.imageSubresource.layerCount = 1;
		copy_region.imageExtent = { pixel_extent_.width, pixel_extent_.height, 1 };
		vkCmdCopyBufferToImage(command_buffer, upload_buffer_, backend_->Swapchain().Images()[image_index],
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

	void VulkanFrameRenderer::RenderSoftwareWorld(const ve::world::World& world, const Camera& camera)
	{
		const VkFormat format = backend_->Swapchain().ImageFormat();
		const float width = static_cast<float>(pixel_extent_.width);
		const float height = static_cast<float>(pixel_extent_.height);
		const float aspect = width / height;
		const float tan_half_fov = std::tan(glm::radians(35.0f));
		const glm::vec3 origin = FindAirCameraOrigin(world, camera.GetPosition());
		const glm::vec3 forward = glm::normalize(camera.GetForward());
		const glm::vec3 right = glm::normalize(camera.GetRight());
		const glm::vec3 up = glm::normalize(camera.GetUp());
		const std::uint64_t pixel_count = static_cast<std::uint64_t>(pixel_extent_.width) * static_cast<std::uint64_t>(pixel_extent_.height);
		std::uint32_t sample_step = 2u;
		if (pixel_count > (1920ull * 1080ull)) sample_step = 5u;
		else if (pixel_count > (1280ull * 720ull)) sample_step = 4u;

		for (std::uint32_t y = 0; y < pixel_extent_.height; y += sample_step)
		{
			for (std::uint32_t x = 0; x < pixel_extent_.width; x += sample_step)
			{
				const float screen_x = ((static_cast<float>(x) + 0.5f) / width) * 2.0f - 1.0f;
				const float screen_y = 1.0f - ((static_cast<float>(y) + 0.5f) / height) * 2.0f;
				const glm::vec3 direction = glm::normalize(forward +
					(right * screen_x * tan_half_fov * aspect) +
					(up * screen_y * tan_half_fov));
				const RayHit hit = TraceWorld(world, origin, direction);
				const Rgb color = hit.hit ? ShadeHit(hit, direction) : SkyColor(direction.y);
				const std::uint32_t packed = PackColor(color, format);
				const std::uint32_t y_end = std::min(y + sample_step, pixel_extent_.height);
				const std::uint32_t x_end = std::min(x + sample_step, pixel_extent_.width);
				for (std::uint32_t fill_y = y; fill_y < y_end; ++fill_y)
				{
					std::uint32_t* row = pixels_.data() + (static_cast<std::size_t>(fill_y) * pixel_extent_.width);
					for (std::uint32_t fill_x = x; fill_x < x_end; ++fill_x)
					{
						row[fill_x] = packed;
					}
				}
			}
		}

		const std::uint32_t center_x = pixel_extent_.width / 2u;
		const std::uint32_t center_y = pixel_extent_.height / 2u;
		const std::uint32_t crosshair = PackColor({ 235, 235, 235 }, format);
		for (std::uint32_t offset = 0; offset < 8u; ++offset)
		{
			if (center_x + offset < pixel_extent_.width) pixels_[(center_y * pixel_extent_.width) + center_x + offset] = crosshair;
			if (center_x >= offset) pixels_[(center_y * pixel_extent_.width) + center_x - offset] = crosshair;
			if (center_y + offset < pixel_extent_.height) pixels_[((center_y + offset) * pixel_extent_.width) + center_x] = crosshair;
			if (center_y >= offset) pixels_[((center_y - offset) * pixel_extent_.width) + center_x] = crosshair;
		}
	}

	bool VulkanFrameRenderer::DrawFrame(const ve::world::World& world, const Camera& camera)
	{
		if (backend_ == nullptr || device_ == VK_NULL_HANDLE) return false;
		if (!EnsureFrameBuffer(backend_->Swapchain().Extent())) return false;
		RenderSoftwareWorld(world, camera);
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

	void VulkanFrameRenderer::DestroyUploadBuffer()
	{
		if (upload_buffer_ != VK_NULL_HANDLE) vkDestroyBuffer(device_, upload_buffer_, nullptr);
		if (upload_memory_ != VK_NULL_HANDLE) vkFreeMemory(device_, upload_memory_, nullptr);
		upload_buffer_ = VK_NULL_HANDLE;
		upload_memory_ = VK_NULL_HANDLE;
		upload_buffer_size_ = 0;
	}

	void VulkanFrameRenderer::Release()
	{
		if (device_ != VK_NULL_HANDLE) vkDeviceWaitIdle(device_);
		DestroyUploadBuffer();
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
		pixels_.clear();
		pixel_extent_ = {};
		device_ = VK_NULL_HANDLE;
		backend_ = nullptr;
		current_frame_ = 0;
		logged_first_frame_ = false;
	}
}
