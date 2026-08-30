#pragma once

#include "CoreTypes.h"

#include <volk.h>

#include <vector>

namespace ve::rendering
{
	class VulkanBackend;

	struct VulkanGpuShaderFrameResources
	{
		VkBuffer uniform_buffer = VK_NULL_HANDLE;
		VkDeviceMemory uniform_memory = VK_NULL_HANDLE;
		VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
		void* mapped_uniform_data = nullptr;
		VkImage shadow_image = VK_NULL_HANDLE;
		VkDeviceMemory shadow_memory = VK_NULL_HANDLE;
		VkImageView shadow_view = VK_NULL_HANDLE;
		VkFramebuffer shadow_framebuffer = VK_NULL_HANDLE;
	};

	struct VulkanGpuChunkRendererResourceState
	{
		VulkanBackend* backend_ = nullptr;
		VkDevice device_ = VK_NULL_HANDLE;
		VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
		VkCommandPool command_pool_ = VK_NULL_HANDLE;
		VkRenderPass render_pass_ = VK_NULL_HANDLE;
		VkRenderPass shadow_render_pass_ = VK_NULL_HANDLE;
		VkDescriptorSetLayout shader_descriptor_set_layout_ = VK_NULL_HANDLE;
		VkDescriptorPool shader_descriptor_pool_ = VK_NULL_HANDLE;
		VkSampler shadow_sampler_ = VK_NULL_HANDLE;
		VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
		VkPipeline voxel_pipeline_ = VK_NULL_HANDLE;
		VkPipeline sky_pipeline_ = VK_NULL_HANDLE;
		VkPipeline shadow_pipeline_ = VK_NULL_HANDLE;
		VkImage depth_image_ = VK_NULL_HANDLE;
		VkDeviceMemory depth_memory_ = VK_NULL_HANDLE;
		VkImageView depth_view_ = VK_NULL_HANDLE;
		ve::core::DynamicArray<VkImageView> swapchain_image_views_;
		ve::core::DynamicArray<VkFramebuffer> framebuffers_;
		VkExtent2D extent_{};
		VkFormat color_format_ = VK_FORMAT_UNDEFINED;
		VkFormat depth_format_ = VK_FORMAT_D32_SFLOAT;
		std::vector<VulkanGpuShaderFrameResources> shader_frame_resources_;
		bool initialized_ = false;
	};
}
