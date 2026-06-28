#include "VulkanGpuChunkRenderer.h"

#include "Logger.h"
#include "VulkanBackend.h"

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::Initialize(VulkanBackend& backend,
		VkCommandPool command_pool,
		const std::filesystem::path& block_texture_directory,
		const std::filesystem::path& shader_directory)
	{
		Release();
		backend_ = &backend;
		device_ = backend.Device().Handle();
		physical_device_ = backend.PhysicalDevice().Handle();
		command_pool_ = command_pool;
		if (device_ == VK_NULL_HANDLE || physical_device_ == VK_NULL_HANDLE || command_pool_ == VK_NULL_HANDLE) return false;
		(void)block_texture_directory;
		if (!CreateRenderPass() || !CreatePipeline(shader_directory) || !CreateSwapchainResources())
		{
			Release();
			return false;
		}
		initialized_ = true;
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Vulkan GPU chunk renderer initialized");
		return true;
	}
	bool VulkanGpuChunkRenderer::IsInitialized() const noexcept { return initialized_; }
	std::uint32_t VulkanGpuChunkRenderer::IndexCount() const noexcept { return index_count_; }
	VkRenderPass VulkanGpuChunkRenderer::RenderPass() const noexcept { return render_pass_; }
	std::uint32_t VulkanGpuChunkRenderer::FindMemoryType(VkPhysicalDevice physical_device, std::uint32_t type_filter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memory_properties{};
		vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
		for (std::uint32_t index = 0; index < memory_properties.memoryTypeCount; ++index)
		{
			const bool is_allowed = (type_filter & (1u << index)) != 0u;
			const bool has_properties = (memory_properties.memoryTypes[index].propertyFlags & properties) == properties;
			if (is_allowed && has_properties) return index;
		}
		return UINT32_MAX;
	}
}
