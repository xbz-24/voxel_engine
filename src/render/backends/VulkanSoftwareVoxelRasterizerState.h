#pragma once

#include "VulkanFrameTypes.h"
#include "VulkanSoftwareVoxelRasterizerData.h"

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

#include <glm/glm.hpp>

namespace ve::rendering
{
	class VulkanSoftwareVoxelRasterizerState
	{
	protected:
		VkExtent2D extent_{};
		VkExtent2D render_extent_{};
		std::vector<std::uint32_t> pixels_;
		std::vector<std::uint32_t> render_pixels_;
		std::vector<std::uint32_t> outline_pixels_;
		std::vector<VulkanRasterUpscaleRange> upscale_x_ranges_;
		std::vector<VulkanRasterUpscaleRange> upscale_y_ranges_;
		std::vector<VulkanRasterCachedSampleRay> ray_cache_;
		VulkanRasterFrameWorldSnapshot world_snapshot_;
		glm::vec3 cached_forward_{ 0.0f, 0.0f, -1.0f };
		glm::vec3 cached_right_{ 1.0f, 0.0f, 0.0f };
		glm::vec3 cached_up_{ 0.0f, 1.0f, 0.0f };
		std::uint32_t cached_sample_step_ = 0;
		bool ray_cache_valid_ = false;
		VulkanFrameTiming last_timing_{};
		VulkanRasterTextureLibrary texture_library_;
		std::vector<std::jthread> workers_;
		std::mutex work_mutex_;
		std::condition_variable work_available_;
		std::condition_variable work_complete_;
		VulkanRasterWork current_work_{};
		std::atomic_size_t next_sample_index_{ 0 };
		std::uint64_t work_generation_ = 0;
		std::size_t active_workers_ = 0;
		bool stop_workers_ = false;
	};
}
