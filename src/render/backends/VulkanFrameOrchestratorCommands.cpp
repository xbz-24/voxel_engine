#include "VulkanFrameOrchestratorCommands.h"

#include "VulkanImGuiOverlay.h"

namespace ve::rendering
{
	VkImageSubresourceRange ColorSubresourceRange() noexcept
	{
		VkImageSubresourceRange range{};
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.levelCount = 1;
		range.layerCount = 1;
		return range;
	}

	VkImageMemoryBarrier ColorImageBarrier(VkImage image,
		VkImageLayout old_layout,
		VkImageLayout new_layout,
		VkAccessFlags src_access,
		VkAccessFlags dst_access) noexcept
	{
		VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier.oldLayout = old_layout;
		barrier.newLayout = new_layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange = ColorSubresourceRange();
		barrier.srcAccessMask = src_access;
		barrier.dstAccessMask = dst_access;
		return barrier;
	}

	VkImageBlit FullImageBlit(VkExtent2D source_extent, VkExtent2D destination_extent) noexcept
	{
		VkImageBlit blit{};
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.layerCount = 1;
		blit.srcOffsets[1] = {
			static_cast<std::int32_t>(source_extent.width),
			static_cast<std::int32_t>(source_extent.height),
			1
		};
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.layerCount = 1;
		blit.dstOffsets[1] = {
			static_cast<std::int32_t>(destination_extent.width),
			static_cast<std::int32_t>(destination_extent.height),
			1
		};
		return blit;
	}

	void RecordImguiOverlay(VkCommandBuffer command_buffer, void* user_data)
	{
		if (user_data == nullptr) return;
		static_cast<VulkanImGuiOverlay*>(user_data)->Record(command_buffer);
	}
}
