#include "VulkanFrameOrchestratorCommands.h"

#include "CoreTypes.h"
#include "VulkanImGuiOverlay.h"

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] std::int32_t ImageOffsetCoordinate(std::uint32_t coordinate) noexcept
		{
			return ve::core::NumericCast<std::int32_t>(coordinate);
		}

		[[nodiscard]] VkOffset3D ImageOffset(VkExtent2D extent) noexcept
		{
			return {
				ImageOffsetCoordinate(extent.width),
				ImageOffsetCoordinate(extent.height),
				1
			};
		}

		[[nodiscard]] VulkanImGuiOverlay& ImGuiOverlayFromUserData(void* user_data) noexcept
		{
			return *static_cast<VulkanImGuiOverlay*>(user_data);
		}
	}

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
		blit.srcOffsets[1] = ImageOffset(source_extent);
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.layerCount = 1;
		blit.dstOffsets[1] = ImageOffset(destination_extent);
		return blit;
	}

	void RecordImguiOverlay(VkCommandBuffer command_buffer, void* user_data)
	{
		if (user_data == nullptr) return;
		ImGuiOverlayFromUserData(user_data).Record(command_buffer);
	}
}
