#include "VulkanFrameOrchestrator.h"

#include "VulkanBackend.h"

#include <array>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace ve::rendering
{
	bool VulkanFrameOrchestrator::CreateTimestampQueries()
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
	bool VulkanFrameOrchestrator::CreateCommandResources()
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
		allocate_info.commandBufferCount = static_cast<std::uint32_t>(frames_.size());
		std::array<VkCommandBuffer, kFramesInFlight> command_buffers{};
		if (vkAllocateCommandBuffers(device_, &allocate_info, command_buffers.data()) != VK_SUCCESS) return false;
		for (std::size_t index = 0; index < frames_.size(); ++index)
		{
			frames_[index].command_buffer = command_buffers[index];
		}
		return true;
	}
	bool VulkanFrameOrchestrator::CreateSynchronization()
	{
		const vk::Device device = backend_->Device().CppHandle();
		const vk::SemaphoreCreateInfo semaphore_info{};
		const vk::FenceCreateInfo fence_info{ vk::FenceCreateFlagBits::eSignaled };
		const std::size_t swapchain_image_count = backend_->Swapchain().Images().size();
		if (swapchain_image_count == 0) return false;
		images_in_flight_.assign(swapchain_image_count, VK_NULL_HANDLE);
		for (std::size_t index = 0; index < kFramesInFlight; ++index)
		{
			vk::Semaphore image_available{};
			if (device.createSemaphore(&semaphore_info, nullptr, &image_available) != vk::Result::eSuccess) return false;
			frames_[index].image_available = image_available;

			vk::Semaphore render_finished{};
			if (device.createSemaphore(&semaphore_info, nullptr, &render_finished) != vk::Result::eSuccess) return false;
			frames_[index].render_finished = render_finished;

			vk::Fence in_flight{};
			if (device.createFence(&fence_info, nullptr, &in_flight) != vk::Result::eSuccess) return false;
			frames_[index].in_flight = in_flight;
		}
		return true;
	}
	std::uint32_t VulkanFrameOrchestrator::FindMemoryType(VkPhysicalDevice physical_device, std::uint32_t type_filter, VkMemoryPropertyFlags properties)
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
