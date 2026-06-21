#include "VulkanFrameRendererCommands.h"

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

	void RecordImguiOverlay(VkCommandBuffer command_buffer, void* user_data)
	{
		if (user_data == nullptr) return;
		static_cast<VulkanImGuiOverlay*>(user_data)->Record(command_buffer);
	}
}
