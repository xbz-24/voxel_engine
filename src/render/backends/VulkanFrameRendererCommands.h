#pragma once

#include <volk.h>

namespace ve::rendering
{
	[[nodiscard]] VkImageSubresourceRange ColorSubresourceRange() noexcept;
	void RecordImguiOverlay(VkCommandBuffer command_buffer, void* user_data);
}
