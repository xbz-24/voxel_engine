#include "VulkanFrameOrchestrator.h"

#include "Logger.h"
#include "VulkanBackend.h"
#include "VulkanFrameOrchestratorCommands.h"

namespace ve::rendering
{
	bool VulkanFrameOrchestrator::RecordSoftwareCommandBuffer(VkCommandBuffer command_buffer, std::uint32_t image_index, std::size_t frame_index)
	{
		if (frame_index >= frames_.size() || frames_[frame_index].intermediate_image == VK_NULL_HANDLE) return false;
		VkCommandBufferBeginInfo begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Render, "Failed to begin Vulkan GPU command buffer");
			return false;
		}

		const std::uint32_t first_query = static_cast<std::uint32_t>(frame_index * 2u);
		if (timestamp_query_pool_ != VK_NULL_HANDLE)
		{
			vkCmdResetQueryPool(command_buffer, timestamp_query_pool_, first_query, 2u);
			vkCmdWriteTimestamp(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, timestamp_query_pool_, first_query);
		}
		VkImageMemoryBarrier internal_to_transfer = ColorImageBarrier(frames_[frame_index].intermediate_image,
			frames_[frame_index].intermediate_image_layout,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			0,
			VK_ACCESS_TRANSFER_WRITE_BIT);
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &internal_to_transfer);
		const VkExtent2D source_extent = rasterizer_.RenderExtent();
		VkBufferImageCopy copy_region{};
		copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy_region.imageSubresource.layerCount = 1;
		copy_region.imageExtent = { source_extent.width, source_extent.height, 1 };
		vkCmdCopyBufferToImage(command_buffer, frames_[frame_index].upload_buffer.Buffer(), frames_[frame_index].intermediate_image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);
		VkImageMemoryBarrier internal_to_source = ColorImageBarrier(frames_[frame_index].intermediate_image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_TRANSFER_READ_BIT);
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &internal_to_source);
		VkImageMemoryBarrier to_transfer = ColorImageBarrier(backend_->Swapchain().Images()[image_index],
			image_layouts_[image_index],
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			0,
			VK_ACCESS_TRANSFER_WRITE_BIT);
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &to_transfer);
		const VkExtent2D destination_extent = rasterizer_.Extent();
		VkImageBlit blit_region = FullImageBlit(source_extent, destination_extent);
		vkCmdBlitImage(command_buffer,
			frames_[frame_index].intermediate_image,
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
		VkImageMemoryBarrier to_present = ColorImageBarrier(backend_->Swapchain().Images()[image_index],
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			0);
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0, 0, nullptr, 0, nullptr, 1, &to_present);
		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) return false;
		frames_[frame_index].intermediate_image_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		image_layouts_[image_index] = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		return true;
	}
}
