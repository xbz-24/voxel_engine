#include "VulkanSoftwareVoxelRasterizer.h"

namespace ve::rendering
{
	void VulkanSoftwareVoxelRasterizer::Release()
	{
		{
			std::lock_guard lock(work_mutex_);
			stop_workers_ = true;
			++work_generation_;
		}
		work_available_.notify_all();
		workers_.clear();

		{
			std::lock_guard lock(work_mutex_);
			stop_workers_ = false;
			active_workers_ = 0;
			current_work_ = {};
		}
		pixels_.clear();
		render_pixels_.clear();
		outline_pixels_.clear();
		upscale_x_ranges_.clear();
		upscale_y_ranges_.clear();
		ray_cache_.clear();
		world_snapshot_ = {};
		extent_ = {};
		render_extent_ = {};
		ray_cache_valid_ = false;
		last_timing_ = {};
	}
}
