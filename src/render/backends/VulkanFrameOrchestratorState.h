#pragma once

#include "CoreTypes.h"
#include "VulkanGpuChunkRenderer.h"
#include "VulkanImGuiOverlay.h"
#include "VulkanRendererOverlay.h"
#include "VulkanSoftwareVoxelRasterizer.h"
#include "VulkanUploadBuffer.h"

#include <volk.h>

#include <array>
#include <cstddef>

namespace ve::rendering
{
	class VulkanBackend;

	struct VulkanGpuFrameControls
	{
		bool overlay_enabled = false;
		bool toggle_overlay = false;
	};

	struct VulkanFrameResources
	{
		VulkanUploadBuffer upload_buffer;
		VkImage intermediate_image = VK_NULL_HANDLE;
		VkDeviceMemory intermediate_image_memory = VK_NULL_HANDLE;
		VkImageLayout intermediate_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkCommandBuffer command_buffer = VK_NULL_HANDLE;
		VkSemaphore image_available = VK_NULL_HANDLE;
		VkSemaphore render_finished = VK_NULL_HANDLE;
		VkFence in_flight = VK_NULL_HANDLE;
		bool timestamp_query_valid = false;
	};

	class VulkanFrameOrchestratorState
	{
	protected:
		using FrameResources = VulkanFrameResources;
		static constexpr std::size_t kFramesInFlight = 2;

		VulkanBackend* backend_ = nullptr;
		VkDevice device_ = VK_NULL_HANDLE;
		VkCommandPool command_pool_ = VK_NULL_HANDLE;
		VkQueryPool timestamp_query_pool_ = VK_NULL_HANDLE;
		float timestamp_period_ns_ = 0.0f;
		std::array<FrameResources, kFramesInFlight> frames_{};
		VulkanGpuChunkRenderer gpu_chunk_renderer_;
		VulkanImGuiOverlay imgui_overlay_;
		VulkanSoftwareVoxelRasterizer rasterizer_;
		VulkanFrameTiming previous_frame_timing_{};
		VulkanSoftwareRasterizerSettings software_rasterizer_settings_{};
		VkExtent2D intermediate_extent_{};
		VkFormat intermediate_format_ = VK_FORMAT_UNDEFINED;
		VkFilter upscale_filter_ = VK_FILTER_NEAREST;
		ve::core::DynamicArray<VkImageLayout> image_layouts_;
		ve::core::DynamicArray<VkFence> images_in_flight_;
		std::size_t current_frame_ = 0;
		double shader_elapsed_seconds_ = 0.0;
		bool imgui_overlay_enabled_ = true;
		bool logged_first_frame_ = false;
	};
}
