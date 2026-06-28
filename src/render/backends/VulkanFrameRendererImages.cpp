#include "VulkanFrameRenderer.h"

#include "Logger.h"
#include "VulkanBackend.h"

#include <string>

namespace ve::rendering
{
	bool VulkanFrameRenderer::EnsureIntermediateImages(VkExtent2D extent, VkFormat format)
	{
		if (extent.width == 0u || extent.height == 0u || format == VK_FORMAT_UNDEFINED) return false;
		if (frames_.front().intermediate_image != VK_NULL_HANDLE &&
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
			if (vkCreateImage(device_, &image_info, nullptr, &frames_[index].intermediate_image) != VK_SUCCESS)
			{
				ReleaseIntermediateImages();
				return false;
			}

			VkMemoryRequirements requirements{};
			vkGetImageMemoryRequirements(device_, frames_[index].intermediate_image, &requirements);
			const std::uint32_t memory_type = FindMemoryType(backend_->PhysicalDevice().Handle(), requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			if (memory_type == UINT32_MAX)
			{
				ReleaseIntermediateImages();
				return false;
			}

			VkMemoryAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
			allocate_info.allocationSize = requirements.size;
			allocate_info.memoryTypeIndex = memory_type;
			if (vkAllocateMemory(device_, &allocate_info, nullptr, &frames_[index].intermediate_image_memory) != VK_SUCCESS ||
				vkBindImageMemory(device_, frames_[index].intermediate_image, frames_[index].intermediate_image_memory, 0) != VK_SUCCESS)
			{
				ReleaseIntermediateImages();
				return false;
			}
			frames_[index].intermediate_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
		}

		intermediate_extent_ = extent;
		intermediate_format_ = format;
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, std::string("Allocated Vulkan internal upscale image: ") +
			std::to_string(extent.width) + "x" + std::to_string(extent.height));
		return true;
	}
	void VulkanFrameRenderer::ReleaseIntermediateImages()
	{
		for (std::size_t index = 0; index < kFramesInFlight; ++index)
		{
			if (frames_[index].intermediate_image != VK_NULL_HANDLE) vkDestroyImage(device_, frames_[index].intermediate_image, nullptr);
			if (frames_[index].intermediate_image_memory != VK_NULL_HANDLE) vkFreeMemory(device_, frames_[index].intermediate_image_memory, nullptr);
			frames_[index].intermediate_image = VK_NULL_HANDLE;
			frames_[index].intermediate_image_memory = VK_NULL_HANDLE;
			frames_[index].intermediate_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
		}
		intermediate_extent_ = {};
		intermediate_format_ = VK_FORMAT_UNDEFINED;
		upscale_filter_ = VK_FILTER_NEAREST;
	}
}
