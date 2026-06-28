#pragma once

#include <volk.h>

namespace ve::rendering
{
	[[nodiscard]] VkImageSubresourceRange ColorSubresourceRange() noexcept;
	[[nodiscard]] VkImageMemoryBarrier ColorImageBarrier(VkImage image,
		VkImageLayout old_layout,
		VkImageLayout new_layout,
		VkAccessFlags src_access,
		VkAccessFlags dst_access) noexcept;
	[[nodiscard]] VkImageBlit FullImageBlit(VkExtent2D source_extent, VkExtent2D destination_extent) noexcept;
	void RecordImguiOverlay(VkCommandBuffer command_buffer, void* user_data);
}
